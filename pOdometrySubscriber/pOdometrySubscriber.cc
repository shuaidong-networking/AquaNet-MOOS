/************************************************************/
/*    NAME: Shuai Dong                                              */
/*    ORGN: UA, Tuscallosa AL                               */
/*    FILE: OdometrySubcriber.cpp                                        */
/*    DATE: May 5th, 2021                             */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "OdometrySubscriber.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

//aquanet_socket header files
#include <sys/un.h> 
#include "aquanet_log.h" 
#include "aquanet_socket.h"
char log_file[BUFSIZE]; 
char* log_file_name = log_file;

using namespace std;

//---------------------------------------------------------
// Constructor

OdometrySubscriber::OdometrySubscriber()
{
}

//---------------------------------------------------------
// Destructor

OdometrySubscriber::~OdometrySubscriber()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool OdometrySubscriber::OnNewMail(MOOSMSG_LIST &NewMail)
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

bool OdometrySubscriber::OnConnectToServer()
{
   registerVariables();
  //  Notify( "REQUIRED_ODOMETRY_DIST",total_distance);
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool OdometrySubscriber::Iterate()
{
  AppCastingMOOSApp::Iterate();
  
  // Do your thing here!
  if(m_first_reading == 1)
  {
    m_total_distance =sqrt(m_current_x*m_current_x+m_current_y*m_current_y);
    
    m_distanceDiff = m_total_distance-m_requiredDistance;
    int m_distanceDiff1 = m_distanceDiff*(-1);
    std::cout << "After" << m_distanceDiff1 <<"meters return" << std::endl;
    
    Notify( "ODOMETRY_DIST1",m_distanceDiff);
    m_first_reading=0;
  }
  else
  {
    m_total_distance =sqrt((m_current_x-m_previous_x)*(m_current_x-m_previous_x)+(m_current_y-m_previous_y)*(m_current_y-m_previous_y))+m_total_distance;
    m_distanceDiff = m_total_distance-m_requiredDistance;
    int m_distanceDiff2 = m_distanceDiff*(-1);
    
    if (m_distanceDiff2 >= 0){
      std::cout << "After" << m_distanceDiff2 <<"meters return" << std::endl;
    }else{
      std::cout << "the required distance is reached, returning!" << std::endl;
    }
    Notify( "ODOMETRY_DIST1",m_distanceDiff);
  }

  //AppCastingMOOSApp::PostReport();
  return(true);
}


// internet socket
/*
void OdometrySubscriber::receiveServerMessage(){

 int sock_cli = socket(AF_INET,SOCK_STREAM, 0);
 

 struct sockaddr_in servaddr;
 memset(&servaddr, 0, sizeof(servaddr));
 servaddr.sin_family = AF_INET;
 servaddr.sin_port = htons(8080); 
 servaddr.sin_addr.s_addr = inet_addr("192.168.80.129"); 

 if (connect(sock_cli, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
 {
 std::cout << "connected fails" << std::endl;
 perror("connect");
 //exit(1);
 return;
 }
 int BUFFER_SIZE=1024;
 char sendbuf[BUFFER_SIZE];
 char recvbuf[BUFFER_SIZE];
 std::cout<<"Blocking.. "<< endl;
 recv(sock_cli, recvbuf, sizeof(recvbuf),0); ///接收
 std::cout<<recvbuf<<endl;
 
//  memset(sendbuf, 0, sizeof(sendbuf));
//  memset(recvbuf, 0, sizeof(recvbuf));
 close(sock_cli);
// sleep(20);
}
*/
/****************  AquaNET Socket ************************/

//aquanet message 
struct aqua_message
{
  double values;
};


// using aqua-socket  05/05/2021
void OdometrySubscriber::receiveServerMessage()
{
  
  std::cout << "waiting for the data from client" << std::endl;
  if ((m_socket = aqua_socket(AF_AQUANET, SOCK_AQUANET, 0)) < 0)
  {
    
    std::cout << "failed to create a socket" << std::endl;
    return;
    
  }
  struct sockaddr_aquanet remote_addr;
  int addr_size = sizeof (remote_addr);
  aqua_message received_values;
  
  if (aqua_recvfrom(m_socket, &received_values, sizeof(received_values), 0, (struct sockaddr *) & remote_addr, &addr_size) < 0)
  {
    std::cout << log_file_name << "failed to read from aqua_socket" << std::endl;
    
  }
  std::cout << "---------------------------------" << std::endl;
  std::cout << "the distance read from socekt is :" << received_values.values << std::endl;
  std::cout << "---------------------------------" << std::endl;
  sleep(2);
  m_requiredDistance = received_values.values;
  std::cout <<"the current m_requiredDistance is :" << m_requiredDistance*(-1) << std::endl;
  
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool OdometrySubscriber::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();
  std::cout << "----Shuai OnStarUp()-----" << std::endl;
  receiveServerMessage();
  std::cout << "----Shuai  Done()----" << std::endl;
  
  
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

void OdometrySubscriber::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("NAV_X", 0);
  Register("NAV_Y", 0);
  // Register("FOOBAR", 0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool OdometrySubscriber::buildReport() 
{
  m_msgs << "============================================" << endl;
  m_msgs << "File:                                       " << endl;
  m_msgs << "============================================" << endl;

  ACTable actab(4);
  actab << "Alpha | Bravo | Charlie | Delta";
  actab.addHeaderLines();
  actab << "one" << "two" << "three" << "four";
  m_msgs << actab.getFormattedString();

  return(true);
}
