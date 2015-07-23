/*
 * network_config.h
 *
 *  Created on: 01/lug/2015
 *      Author: Paolo
 */

#ifndef NETWORK_CONFIG_H_
#define NETWORK_CONFIG_H_

#include <stdint.h>

typedef uint8_t* 				challenge_t;


#define MY_ADDRESS				0x0000

#if MY_ADDRESS == 0x0000
#define ROOT
#endif

#define TRAILER_LEN_BYTE				1
#define CHALLENGE_LEN_BYTE 				4
#define	SECRET_LEN_BYTE					2

#define MAX_HOP_COUNT					8

#define UPSTREAM_SND_RETRY			1
#define DOWNSTREAM_SND_RETRY		5
#define SND_TIMEOUT					1000





#define MAX_UPSTREAM_TABLE_PENALIZES	1


#endif /* NETWORK_CONFIG_H_ */
