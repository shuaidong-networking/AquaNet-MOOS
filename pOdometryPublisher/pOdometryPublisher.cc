/* ***********************************************************/
/*    NAME: Shuai Dong                                              */
/*    ORGN: UA, Tuscaloosa AL                               */
/*    FILE: OdometryPublisher.cpp                                        */
/*    DATE: MAY 3rd, 2021                             */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "OdometryPublisher.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <execinfo.h>

//Aquanet socket

#include <sys/un.h> 
#include "aquanet_log.h" 
#include "aquanet_socket.h"
char log_file[BUFSIZE]; 
char* log_file_name = log_file;

using namespace std;

//---------------------------------------------------------
// Constructor

OdometryPublisher::OdometryPublisher()
{
  m_first_reading= 1;
  m_current_x = 0 ;
  m_current_y = 0;
  m_previous_x = 0;
  m_previous_y = 0;
  m_total_distance = 0;
}

//---------------------------------------------------------
// Destructor

OdometryPublisher::~OdometryPublisher()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool OdometryPublisher::OnNewMail(MOOSMSG_LIST &NewMail)
{

  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();

#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif
    if(key == "NAV_X")
    {
   	 m_previous_x = m_current_x;
   	 m_current_x = msg.GetDouble();
    }

    if(key == "NAV_Y")
	  {
    	m_previous_y = m_current_y;
    	m_current_y = msg.GetDouble();
	  }

  }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool OdometryPublisher::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool OdometryPublisher::Iterate()
{
  AppCastingMOOSApp::Iterate();

  // 05/04/2021 adding total distance logic into MOOSDB  ------- Shuai

  if(m_first_reading == 1)
  {
    m_total_distance =sqrt(m_current_x*m_current_x+m_current_y*m_current_y);
    Notify( "ODOMETRY_DIST",m_total_distance);
    m_first_reading=0;
  }
  else
  {
    m_total_distance =sqrt((m_current_x-m_previous_x)*(m_current_x-m_previous_x)+(m_current_y-m_previous_y)*(m_current_y-m_previous_y))+m_total_distance;
    Notify( "ODOMETRY_DIST",m_total_distance);
  }

  // Do your thing here!
  //AppCastingMOOSApp::PostReport();
  return(true);
}
void OdometryPublisher::error_handler(int sig) {
  //  void *array[10];
  //  size_t size;
  //  size = backtrace(array, 10); //get the void pointers for all of the entries
  //  cout << "Error: signal "<< sig <<":\n"; //display error signal
  //  backtrace_symbols_fd(array, size, STDERR_FILENO);
}

/*
// internet socket
void OdometryPublisher::sendServerMessage(){
  
 std::cout << "sendServerMessage!!" << std::endl;
 int PORT = 8080;
 int MAXLINE=1024;
 int ss = socket(AF_INET, SOCK_STREAM, 0);
 struct sockaddr_in server_sockaddr;
 server_sockaddr.sin_family = AF_INET;
 server_sockaddr.sin_port = htons(PORT);
 server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
 
 if(bind(ss, (struct sockaddr* ) &server_sockaddr, sizeof(server_sockaddr))==-1)
 {
 std::cout <<"bind fails" << std::endl;
 perror("bind");
 //exit(1);
 return;
 }
 if(listen(ss, 20) == -1)
 {
 std::cout <<"listen fails" << std::endl;
 perror("listen");
 //exit(1);
 return;
 }
 
 struct sockaddr_in client_addr;
 socklen_t length = sizeof(client_addr);

 std::cout<<"Blocking1"<<std::endl;
 int conn = accept(ss, (struct sockaddr*)&client_addr, &length);

 if( conn < 0 )
 {
 std::cout << " connected fails " << std::endl;
 perror("connect");
 //exit(1);
 return ;
 }
 
 char buffer[]="Shuai hello";
 
 int len = strlen(buffer);
 
 
 std::cout<<"Blocking2"<<std::endl;

 send(conn, buffer, len , 0);
 std::cout<<"Done"<<std::endl;
 close(conn);
 close(ss);
 //sleep(20);
}
*/
//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

// AquaNet Message
struct aqua_message
{
  //char request_distance[100]="shuaihello";
  double request_distance = 50.0;
};

// AquaNet socket

void OdometryPublisher::sendServerMessage()
{
  //create a aqua_soscket
  
  if ((m_socket = aqua_socket(AF_AQUANET, SOCK_AQUANET, 0)) < 0)
  {
    std::cout << "aqua_socket fails" << std::endl;
    return;
  }
  unsigned short int local_addr = 1;
  unsigned short int remote_addr = 2;
  struct sockaddr_aquanet to_addr;
  to_addr.sin_family = AF_AQUANET;
  to_addr.sin_addr.s_addr = local_addr;
  to_addr.sin_addr.d_addr = remote_addr;

  aqua_message values;

  //send through aquanet
  if (aqua_sendto(m_socket, &values, sizeof(values), 0, (struct sockaddr *) & to_addr, sizeof (to_addr)) < 0)
  {
    std::cout << "failed to send to the socket" << std::endl;
    return;
  }
  std::cout << "successfully sending the required distance:" << values.request_distance << "to the AUV" << std::endl;
}

bool OdometryPublisher::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  //std::cout << "------Shuai OnStarUp()------" << std::endl;
  std::cout << "------Shuai SocketStart()------" << std::endl;
  sendServerMessage();
  std::cout << "------Shuai SocketDone()------" << std::endl;

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams))
    reportConfigWarning("No config block found for " + GetAppName());

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;

    bool handled = false;
    if(param == "foo") {
      handled = true;
    }
    else if(param == "bar") {
      handled = true;
    }

    if(!handled)
      reportUnhandledConfigWarning(orig);

  }
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void OdometryPublisher::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  
  /*
  Register("NAV_X", 0);
  Register("NAV_Y", 0);
  */
  // Register("FOOBAR", 0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool OdometryPublisher::buildReport() 
{
  /*
  m_msgs << "============================================" << endl;
  m_msgs << "File:                                       " << endl;
  m_msgs << "============================================" << endl;

  ACTable actab(4);
  actab << "Alpha | Bravo | Charlie | Delta";
  actab.addHeaderLines();
  actab << "one" << "two" << "three" << "four";
  m_msgs << actab.getFormattedString();
  */
  return(true);
}
