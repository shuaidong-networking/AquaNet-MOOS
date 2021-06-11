/************************************************************/
/*    NAME: Shuai Dong                                              */
/*    ORGN: UA, Tuscaloosa AL                               */
/*    FILE: OdometrySubcriber.h                                          */
/*    DATE: May 3rd, 2021                             */
/************************************************************/

#ifndef OdometrySubcriber_HEADER
#define OdometrySubcriber_HEADER

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
   double m_total_distance;
   double m_distanceDiff;
   double m_requiredDistance;
 protected: // Standard AppCastingMOOSApp function to overload 
   bool buildReport();

 protected:
   void registerVariables();

 private: // Configuration variables
 void receiveServerMessage();
 int m_socket = -1;
 private: // State variables
};

#endif 
