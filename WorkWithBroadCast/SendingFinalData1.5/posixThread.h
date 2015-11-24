#ifndef _POSIX_THREAD_H_
#define _POSIX_THREAD_H_

#define NO_OF_THREADS 3
int *sendDataOverPosixThread( void *ptr);
int createPosixThread(char *IPAddress[], char *HWAddress[], int *IPIndex);

#endif
