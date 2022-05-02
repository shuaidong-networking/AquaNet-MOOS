/************************************************************/
/*    NAME: Shuai Dong                                              */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: OdometryPublisher.cpp                                        */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "OdometryPublisher.h"

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

OdometryPublisher::OdometryPublisher()
{
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

     if(key == "FOO") 
       cout << "great!";

     else if(key != "APPCAST_REQ") // handled by AppCastingMOOSApp
       reportRunWarning("Unhandled Mail: " + key);
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
  // Do your thing here!
  //AppCastingMOOSApp::PostReport();
  return(true);
}


void OdometryPublisher::sendMsg(){
  unetsocket_t sock;
  //fjage_msg_t ntf;
  fjage_gw_t gw;
  uint8_t data[NBYTES] = {50};
  int address = 97;
  int port = 1100;
  int rv;
  //opening a socket
  sock = unetsocket_open("192.168.1.159", 1100);
  if (sock == NULL) return ;
  printf("Transmitting %d bytes of data to %d\n", NBYTES, address);
  rv = unetsocket_send_reliable(sock, data, NBYTES, address, DATA);
  const char *list[] = {"org.arl.unet.DatagramDeliveryNtf", "org.arl.unet.DatagramFailureNtf"};
  gw = unetsocket_get_gateway(sock);
  fjage_msg_t msg = fjage_receive_any(gw, list, 2, 60000);
  if (strcmp(fjage_msg_get_clazz(msg), "org.arl.unet.DatagramDeliveryNtf") == 0) printf("Datagram delivered succesfully at the received node! \n");
  if (strcmp(fjage_msg_get_clazz(msg), "org.arl.unet.DatagramFailureNtf") == 0) printf("Datagram delivery failed! \n");
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool OdometryPublisher::OnStartUp()
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
  sendMsg();
  //sleep(100);
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void OdometryPublisher::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  // Register("FOOBAR", 0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool OdometryPublisher::buildReport() 
{
  // m_msgs << "============================================" << endl;
  // m_msgs << "File:                                       " << endl;
  // m_msgs << "============================================" << endl;

  // ACTable actab(4);
  // actab << "Alpha | Bravo | Charlie | Delta";
  // actab.addHeaderLines();
  // actab << "one" << "two" << "three" << "four";
  // m_msgs << actab.getFormattedString();

  return(true);
}




