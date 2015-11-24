#ifndef _AUTHENTICATION_H_
#define _AUTHENTICATION_H_

#include "IPHWStructure.h"

#define  AUTHENTICATION  	0
#define  UNAUTHENTICATION  	-1

int athentication(IPHWAddress *addresses); 

#ifdef BROADCASTER 
int sendOTP(IPHWAddress *addresses);
#endif

#ifdef LISTENER
int recieveOTP(void);
#endif

#endif 
