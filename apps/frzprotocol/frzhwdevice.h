/*
 * frzhwdevice.h
 *
 *  Created on: Dec 13, 2012
 *      Author: travis
 */

#ifndef __FRZHWDEVICE_H__
#define __FRZHWDEVICE_H__

#include <stdint.h>

#define WW_TRICOLOR_LED   0x0001
#define FRZ_FLAG_FIRST    0x0002
#define FRZ_FLAG_LAST     0x0004


 /*
 array[0]=30;		 	//Array length always at pos 0
 array[1]=0x0001; 		//hardware device as described above
 array[2]=0;			//hardware device custom field (see header file of hwdevice)
 array[3]=0;			//hardware device custom field (see header file of hwdevice)
 array[4]=0;			//hardware device custom field (see header file of hwdevice)
 array[5]=0;			//hardware device custom field (see header file of hwdevice)
 array[6]=0;			//hardware device custom field (see header file of hwdevice)
 array[7]=0;			//hardware device custom field (see header file of hwdevice)
 ..
 array[xx]=0;			//hardware device custom field (see header file of hwdevice)

 */

#endif
