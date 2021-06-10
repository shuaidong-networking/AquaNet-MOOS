/************************************************************/
/*    NAME: Shuai Dong                                              */
/*    ORGN: UA, Tuscaloosa AL                               */
/*    FILE: OdometryPublisher.h                                          */
/*    DATE: May 3rd, 2021                             */
/************************************************************/

#ifndef OdometryPublisher_HEADER
#define OdometryPublisher_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

class OdometryPublisher : public AppCastingMOOSApp
{
 public:
   OdometryPublisher();
   ~OdometryPublisher();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();
   bool  m_first_reading;
   double m_current_x;
   double m_current_y;
   double m_previous_x;
   double m_previous_y;
   double m_total_distance;


 protected: // Standard AppCastingMOOSApp function to overload 
   bool buildReport();

 protected:
   void registerVariables();
   

 private: // Configuration variables
 void sendServerMessage();
 void error_handler(int sig);
 int m_socket = -1;
 private: // State variables
};

#endif 
