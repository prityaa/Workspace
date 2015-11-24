#ifndef _POSIX_THREAD_H_
#define _POSIX_THREAD_H_

#include "IPHWStructure.h"

#define NO_OF_THREADS 3
int *sendDataOverPosixThread( void *ptr);
int *createPosixThread(void *ptr);

#endif
