//
// This file is part of the µOS++ III distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// Do not include on semihosting and when freestanding
#if !defined(OS_USE_SEMIHOSTING) && !(__STDC_HOSTED__ == 0)

// ----------------------------------------------------------------------------

#include <errno.h>
#include "diag/Trace.h"
#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "string.h"
#include "usbd_cdc_if_template.h"
#include "usbd_cdc.h"
// ----------------------------------------------------------------------------

// When using retargetted configurations, the standard write() system call,
// after a long way inside newlib, finally calls this implementation function.

// Based on the file descriptor, it can send arrays of characters to
// different physical devices.

// Currently only the output and error file descriptors are tested,
// and the characters are forwarded to the trace device, mainly
// for demonstration purposes. Adjust it for your specific needs.

// For freestanding applications this file is not used and can be safely
// ignored.




ssize_t
_read (int fd, char* buf, size_t nbyte);

ssize_t
_read (int fd __attribute__((unused)),  char* buf __attribute__((unused)),
		size_t nbyte __attribute__((unused)))
{
#if defined(TRACE)
	// STDOUT and STDERR are routed to the trace device

	if (fd == 1 || fd == 2)
	{

		int i=0;


		do{
			if(VCP_read(buf+i*sizeof(char), 1)!=0){
				i++;
			}


		}while((int)buf[i-1]!=13);  //riceve fin quando non premo INVIO su tastiera

		buf[i-1]=0;   //elimina carattere di invio dal valore acquisito

		return (ssize_t)i ;
	}
#endif // TRACE

	errno = ENOSYS;
	return -1;
}

int scanf(const char* buf,...){

	_read(1,buf,strlen(buf));

	return 0;
}


// ----------------------------------------------------------------------------

#endif // !defined(OS_USE_SEMIHOSTING) && !(__STDC_HOSTED__ == 0)
