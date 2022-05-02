/************************************************************/
/*    NAME: Shuai Dong                                              */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: OdometryPublisher.h                                          */
/*    DATE: December 29th, 1963                             */
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

 protected: // Standard AppCastingMOOSApp function to overload 
   bool buildReport();

 protected:
   void registerVariables();

 private: // Configuration variables
   void sendMsg();
 private: // State variables
};

#endif 
