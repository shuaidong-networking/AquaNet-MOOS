/************************************************************/
/*    NAME: Shuai Dong                                              */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: OdometrySubscriber.cpp                                        */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "OdometrySubscriber.h"


extern "C" 
{
	#include "fjage.h"
	#include "unet.h"
  #include <stdlib.h>
  #include <unistd.h>
}

#define NBYTES 1
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

    //  if(key == "FOO") 
    //    cout << "great!";

    //  else if(key != "APPCAST_REQ") // handled by AppCastingMOOSApp
    //    reportRunWarning("Unhandled Mail: " + key);
    if (key == "NAV_X")
    {
      m_previous_x = m_current_x;
      m_current_x = msg.GetDouble();
    }
    if (key == "NAV_Y")
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
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool OdometrySubscriber::Iterate()
{
  AppCastingMOOSApp::Iterate();
  // Do your thing here!
  //AppCastingMOOSApp::PostReport();
  if(m_first_reading == 1)
  {
    m_total_distance =sqrt(m_current_x*m_current_x+m_current_y*m_current_y);
    
    m_distanceDiff = m_total_distance-m_required_distance;
    int m_distanceDiff1 = m_distanceDiff*(-1);
    std::cout << "After" << m_distanceDiff1 <<"meters return" << std::endl;
    
    Notify( "ODOMETRY_DIST1",m_distanceDiff);
    m_first_reading=0;
  }
  else
  {
    m_total_distance =sqrt((m_current_x-m_previous_x)*(m_current_x-m_previous_x)+(m_current_y-m_previous_y)*(m_current_y-m_previous_y))+m_total_distance;
    m_distanceDiff = m_total_distance-m_required_distance;
    int m_distanceDiff2 = m_distanceDiff*(-1);
    
    if (m_distanceDiff2 >= 0){
      std::cout << "After" << m_distanceDiff2 <<"meters return" << std::endl;
    }else{
      std::cout << "the required distance is reached, returning!" << std::endl;
    }
    Notify( "ODOMETRY_DIST1",m_distanceDiff);
  } 
  return(true);
}


void OdometrySubscriber::receiveMsg()
{
  unetsocket_t sock;
  fjage_msg_t ntf;
  uint8_t data[NBYTES];
  int port = 1100;
  int rv;
  printf("Connecting to %s:%d\n","192.168.1.127",port);
  sock = unetsocket_open("192.168.1.127", port);
  if (sock == NULL) return perror("Couldn't open unet socket");
  // Bind to protocol DATA
  rv = unetsocket_bind(sock, DATA);
  if (rv != 0) return perror("Error binding socket");
  // Set a timeout of 10 seconds
  unetsocket_set_timeout(sock, 20000);
  // Receive and display data
  printf("Waiting for a Datagram\n");
  ntf = unetsocket_receive(sock);
  if (fjage_msg_get_clazz(ntf) != NULL)
  {
    printf("Received a %s : [", fjage_msg_get_clazz(ntf));
    fjage_msg_get_byte_array(ntf, "data", data, NBYTES);
    for (int i = 0; i<NBYTES; i++) {
        printf("%d,", data[i]);
    }
    m_required_distance = double(data[0]);
    printf("]\n");
  }else
  {
    fjage_msg_destroy (ntf);
    printf("Error receiving data\n");
  }
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool OdometrySubscriber::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  // STRING_LIST sParams;
  // m_MissionReader.EnableVerbatimQuoting(false);
  // if(!m_MissionReader.GetConfiguration(GetAppName(), sParams))
  //   reportConfigWarning("No config block found for " + GetAppName());

  // STRING_LIST::iterator p;
  // for(p=sParams.begin(); p!=sParams.end(); p++) {
  //   string orig  = *p;
  //   string line  = *p;
  //   string param = tolower(biteStringX(line, '='));
  //   string value = line;

  //   bool handled = false;
  //   if(param == "foo") {
  //     handled = true;
  //   }
  //   else if(param == "bar") {
  //     handled = true;
  //   }

  //   if(!handled)
  //     reportUnhandledConfigWarning(orig);

  // }
  receiveMsg();
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




