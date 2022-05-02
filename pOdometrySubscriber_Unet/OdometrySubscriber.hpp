/************************************************************/
/*    NAME: Shuai Dong                                              */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: OdometrySubscriber.h                                          */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef OdometrySubscriber_HEADER
#define OdometrySubscriber_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

class OdometrySubscriber : public AppCastingMOOSApp
{
 public:
   OdometrySubscriber();
   ~OdometrySubscriber();

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
   double m_distanceDiff;
   double m_total_distance;
   double m_required_distance;
 protected: // Standard AppCastingMOOSApp function to overload 
   bool buildReport();

 protected:
   void registerVariables();

 private: // Configuration variables
   void receiveMsg();
 private: // State variables
};

#endif 
