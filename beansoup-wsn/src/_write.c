//
// This file is part of the ÂµOS++ III distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// Do not include on semihosting and when freestanding
#if !defined(OS_USE_SEMIHOSTING) && !(__STDC_HOSTED__ == 0)

// ----------------------------------------------------------------------------


#include <errno.h>
#include "diag/Trace.h"
#include <stdio.h>
#include "string.h"
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"

#include "usbd_core.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if_template.h"
#include "usbd_desc.h"
# define PRECISION 5

extern USBD_HandleTypeDef USBD_Device;

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

char *itos(int n, int *p); //int to string (base 10)
char *itos0x(int n, int *p); //int to string (base 16)
char* floatToString(float num);    //float to string
char *itosOct(int n, int *p);     //int to octal string



ssize_t
_write (int fd, const char* buf, size_t nbyte);

ssize_t
_write (int fd __attribute__((unused)), const char* buf __attribute__((unused)),
		size_t nbyte __attribute__((unused)))
{
#if defined(TRACE)
	// STDOUT and STDERR are routed to the trace device
	if (fd == 1 || fd == 2)
	{

		if (USBD_Device.dev_connection_status == 1)
		{
			return (ssize_t)VCP_write(buf, (int)nbyte);

		}

		else return 0;
		//trace_write (buf, nbyte);


	}
#endif // TRACE


	errno = ENOSYS;
	return -1;
}

// ----------------------------------------------------------------------------

#endif // !defined(OS_USE_SEMIHOSTING) && !(__STDC_HOSTED__ == 0)

int printf(const char* buf,...){
	int i=0;
	va_list pt;   //lista argomenti
	va_start(pt,buf);  //inizializza lista argomenti
	while (buf[i]!='\0'){
		while (buf[i]!='\0' && buf[i]!='%') i++; //raggiungo un % o la fine della stringa
		if (i>0) write(1,buf,i); //stampo fino al punto raggiunto, escludendo l'eventuale % o \0
		if (buf[i]=='%') {
			int n;
			char *str;
			float num;
			char c[6]="(NIL)"; //nil già pronto (come usa printf) per i puntatori NULL, altrimenti c[0] viene usato per i parametri carattere
			switch(buf[++i]){
			case 'd':    //intero
				n=va_arg(pt,int);
				str=itos(n,&n);
				write(1,str,n);
				free(str);
				break;
			case 'i':    //intero
				n=va_arg(pt,int);
				str=itos(n,&n);
				write(1,str,n);
				free(str);
				break;
			case 'x':   //intero come esadecimale
				n=va_arg(pt,int);
				str=itos0x(n,&n);
				write(1,str,n);
				free(str);
				break;
			case 'o':    //intero come ottale
				n=va_arg(pt,int);
				str=itosOct(n,&n);
				write(1,str,n);
				break;
			case 'f':    //float
				num=(float)va_arg(pt,double);
				str=floatToString(num);
				write(1,str,strlen(str));
				free(str);
				break;
			case 's':     //stringa, cioè un char*
				str=va_arg(pt,char*);
				write(1,str,strlen(str));
				break;
			case 'c':         //carattere singolo
				c[0]=va_arg(pt,int); //int perchè va_arg converte i caratteri in interi prima di restituirli
				write(1,&c[0],1);
				break;
			case 'p':      //pointer address (se il pointer è nulla, stampa la stringa (NIL)
				n=(int)va_arg(pt,void*);
				if (n==0) write(1,c,5); //stampo (nil)
				else {
					str=itos0x(n,&n);
					write(1,str,n);
					free(str);
				}
				break;
			case '%':     //stampa carattere %
				write(1,&buf[i],1);
				break;
			default:   //se le opzioni non appartengono ad alcuna di quelle valide i 4 LED lampeggiano e il software va in un loop infinito
				while(1){
					BSP_LED_Toggle(LED3);
					BSP_LED_Toggle(LED4);
					BSP_LED_Toggle(LED5);
					BSP_LED_Toggle(LED6);
					HAL_Delay(1000);
				}
			}
			i++;
		}
		buf=buf+i; //faccio ricominciare la lettura di s o dal carattere successivo a quello dopo il % o dal '\0' e in quest'ultimo caso si uscirà dal ciclo
		i=0;  //azzero indice scorrimento stringa buf della printf
	}
	va_end(pt);

	return 0;
}


/*
 * itos converte un intero in una stringa allocata dinamicamente
 * prende in input il numero e un indirizzo (anche null) usato per restituire il numero di caratteri escluso lo '\0'
 */
char* itos(int n, int *p) {
	int ncar=1,tmp=n/10,i,inizio=0;
	char *s=NULL;
	while (tmp) { //conto le cifre di n
		ncar++;
		tmp/=10;
	}
	if (n<0) { //metto il segno
		ncar++;
		s=(char*)malloc((ncar+1)*sizeof(char)); //+1 per il carattere terminatore
		*s='-';
		inizio=1;
		n*=-1;
	}
	else s=(char*)malloc((ncar+1)*sizeof(char));
	s[ncar]='\0';
	for (i=ncar-1; i>=inizio; n/=10, i--) s[i]=(char)(n%10 + 48); //converto la cifra in carattere ascii, 48 è lo zero
	if (p) *p=ncar;
	return s;
}

/*
 * itos0x converte un intero in num esadecimale e lo restituisce come stringa allocata dinamicamente
 * prende in input il numero e un indirizzo (anche null) usato per restituire il numero di caratteri escluso lo '\0'
 */
char *itos0x(int n, int *p){
	int ncar=0, i,tmp,j;
	char v[sizeof(int *)*2]; //con un num esadecimale di 2 cifre posso rappresentare un byte, quindi per rappresentarne sizeof(int*) mi servirà al max un vettore grande il doppio
	char *s; //stringa in output
	do { //converto il num in esadecimale
		tmp=n%16;
		if (tmp<10) v[ncar]=tmp+48;
		else v[ncar]=tmp-10 + 97; //97 è la a minuscola nella tab ascii
		n/=16;
		ncar++;
	}while (n);
	s=(char*)malloc((ncar+3)*sizeof(char)); //+3 per il 0x iniziale e lo '\0' finale
	s[0]='0';
	s[1]='x';
	for (i=2, j=ncar-1; i<ncar+2; i++, j--) s[i]=v[j]; //copio v in s in ordine inverso
	s[i]='\0';
	if (p) *p=ncar+2;
	return s;
}

//converte numero intero in una stringa ottale
char *itosOct(int n, int *p){
	int rem, i=1, octal=0;
	while (n!=0)
	{
		rem=n%8;
		n/=8;
		octal+=rem*i;
		i*=10;
	}
	return itos(octal,p);
}


//converte un float in una stringa
char*  floatToString(float num)
{
	int whole_part = num;
	int digit = 0, reminder =0;
	int log_value = log10(num), index = log_value;
	long wt =0;

	// String containg result
	//char* str = new char[20];
	char* str=(char*)malloc((20)*sizeof(char));

	//Initilise stirng to zero
	memset(str, 0 ,20);

	//Extract the whole part from float num
	for(int  i = 1 ; i < log_value + 2 ; i++)
	{
		wt  =  pow(10.0,i);
		reminder = whole_part  %  wt;
		digit = (reminder - digit) / (wt/10);

		//Store digit in string
		str[index--] = digit + 48;              // ASCII value of digit  = digit + 48
		if (index == -1)
			break;
	}

	index = log_value + 1;
	str[index] = '.';

	float fraction_part  = num - whole_part;
	float tmp1 = fraction_part,  tmp =0;

	//Extract the fraction part from  num
	for( int i= 1; i < PRECISION; i++)
	{
		wt =10;
		tmp  = tmp1 * wt;
		digit = tmp;

		//Store digit in string
		str[++index] = digit +48;           // ASCII value of digit  = digit + 48
		tmp1 = tmp - digit;
	}

	return str;
}




