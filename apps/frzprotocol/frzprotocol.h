/*
 * frzprotocol.h
 *
 *  Created on: Sep 7, 2011
 *      Author: ed
 */

#ifndef FRZPROTOCOL_H_
#define FRZPROTOCOL_H_

#include <stdint.h>

// Protocol layout:
// UDP:
// [MESSAGE_ID /16][SEQ ID     /16][FLAGS      /16][OPTIONAL   /16][PAYLOAD ................]
// 0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF------------------------->
// First Message: Sender -> Recv
// [MESSAGE ID /16][SEQ ID (0) /16][FIRST FLAG    ][TOTAL SEQ     ][First payload           ]

// Last Message: Sender -> Recv
// [MESSAGE ID /16][SEQ ID (#) /16][LAST FLAG     ][TOTAL SEQ     ][First payload           ]

// Full Message Received: Recv -> Sender
// [MESSAGE ID /16][SEQ ID (#) /16][LAST FLAG  | ACK   ][ null    ]

// Lost Pack: Recv -> Sender     (SEQ ID is last received, but SEQ LOST is the one Recv needs)
// [MESSAGE ID /16][SEQ ID (#) /16][LOST FLAG     ][SEQ LOST    ]

// Lost Pack - Resend all from seq num #: Recv -> Sender
// [MESSAGE ID /16][SEQ ID (#) /16][LOST | RESEND_ALL][ null ]


#define FRZ_FLAG_ACK      0x0001
#define FRZ_FLAG_FIRST    0x0002
#define FRZ_FLAG_LAST     0x0004
#define FRZ_FLAG_LOST     0x1000
#define FRZ_FLAG_RESET    0x2000  // tells far-end to reset everything they know about previous states
#define FRZ_FLAG_FAIL     0x4000  // tells far-end to reset everything they know about previous states

#define FRZ_ERROR_TOO_BIG 0x0010  // transfer too big

// size in bytes of header
#define FRZ_HEADER_SIZE     8

// offsets are in bytes
#define MESSAGE_ID_OFFSET   0
#define SEQ_ID_OFFSET       2
#define FRZ_FLAGS_OFFSET    4
#define FRZ_OPTIONAL_OFFSET 6

//typedef uint16_t FRZ_UINT16;


// NOTE: this may cause memory alignment issues on ARM
#define FRZ_SEQ_ID( hdr ) (*((uint16_t *) ((char *)hdr+SEQ_ID_OFFSET)))
#define FRZ_MSG_ID( hdr ) (*((uint16_t *) ((char *)hdr+MESSAGE_ID_OFFSET)))
#define FRZ_FLAGS( hdr ) (*((uint16_t *) ((char *)hdr+FRZ_FLAGS_OFFSET)))
#define FRZ_OPTIONAL( hdr ) (*((uint16_t *) ((char *)hdr+FRZ_OPTIONAL_OFFSET)))
#define FRZ_PAYLOAD( hdr ) (((char *)hdr)+FRZ_HEADER_SIZE)

// Use these on ARM:
#define SET_FRZ_SEQ_ID( hdr, v ) { *((uint32_t *) hdr) = ((uint32_t) (v) & 0xFF) | (*((uint32_t *) hdr) & 0xFF00); }
#define SET_FRZ_MSG_ID( hdr, v ) { *((uint32_t *) hdr) = ((uint32_t) (v) << 16) | (*((uint32_t *) hdr) & 0x00FF); }
#define SET_FRZ_FLAGS( hdr, v ) { *((uint32_t *) hdr + 1) = ((uint32_t) (v) & 0xFF) | (*((uint32_t *) hdr) & 0xFF00); }
#define SET_FRZ_OPTIONAL( hdr, v ) { *((uint32_t *) hdr + 1) = ((uint32_t) (v) << 16) | (*((uint32_t *) hdr) & 0x00FF); }

#define GET_FRZ_SEQ_ID( hdr ) (*((uint32_t *) hdr) & 0xFF)
#define GET_FRZ_MSG_ID( hdr ) (*((uint32_t *) hdr) << 16)
#define GET_FRZ_FLAGS( hdr )  (*((uint32_t *) hdr + 1) & 0xFF)
#define GET_FRZ_OPTIONAL( hdr ) (*((uint32_t *) hdr + 1) << 16)

// PROTOCOL STATES
#define FRZ_IN_TRANSFER 2
#define FRZ_WAITING     0
#define FRZ_PROCESSING  4

//
#define FRZ_MAX_TRANSFER_LEN 120*7


// BLASTER:

/**
 * Main blast function.  Pass this function the array
 *
 * Array format
 * All stored integers are expected to be uint16_t
 * Positions 0..10 are instructions (see example below)
 * Positions 11..n are on/off pulse times measured in uS/5.333 (thats microseconds div 5.33333)
 * Last position in the array is always an OFF, and represents the time to wait before repeating
 * Position 0 points to the last array position
 * pulsetrain[0]=30;		 	//Array length always at pos 0
  pulsetrain[1]=36000; 		//FREQ always at pos 1
  pulsetrain[2]=0x7FFF;		//duty always at pos 2
  pulsetrain[3]=3; 			//Repeat alaways at pos 3
  pulsetrain[4]=0b111111;	//Blast mask: 0b00xxxxxx
  pulsetrain[5]=0b010001;	//Led mask:
  pulsetrain[6]=0;			//Reserved
  pulsetrain[7]=0;			//Reserved
  pulsetrain[8]=0;			//Reserved
  pulsetrain[9]=0;			//Reserved
  pulsetrain[10]=0;			//Reserved
  pulsetrain[11]=167;		//First on duration
  pulsetrain[12]=167;		//First off duration
  pulsetrain[13]=333;		//Second ...
  pulsetrain[14]=167;
  pulsetrain[15]=167;
  pulsetrain[16]=167;
  pulsetrain[17]=167;
  pulsetrain[18]=333;
  pulsetrain[19]=333;
  pulsetrain[20]=333;
  pulsetrain[21]=167;
  pulsetrain[22]=167;
  pulsetrain[23]=167;
  pulsetrain[24]=167;
  pulsetrain[25]=333;
  pulsetrain[26]=333;
  pulsetrain[27]=333;
  pulsetrain[28]=333;
  pulsetrain[29]=167;
  pulsetrain[30]=0xffff;	//End with the duration between repeats (this must always be an off)
 */

// 11 elements needed at beginning of array
#define FRZ_BLAST_ARRAY_HEADER 11


#endif /* FRZPROTOCOL_H_ */
