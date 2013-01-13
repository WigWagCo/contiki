/*
 * Copyright (c) 2010, Mariano Alvira <mar@devl.org> and other contributors
 * to the MC1322x project (http://mc1322x.devl.org) and Contiki.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki OS.
 *
 * $Id: button-sensor.c,v 1.1 2010/06/09 14:46:30 maralvira Exp $
 *
 * **Some additions have been made by Justin Nguyen <justin_n@utexas.edu>
 *
 */




#include "contiki.h"
#include <mc1322x.h>
#include "lib/sensors.h"
#include "include/interrupt_monitor.h"
#include "gpio-util.h"
#include "mc1322x.h"
#include "gpio.h"
#include <signal.h>
#include <io.h>
#define PLUNGER KBI_2

#define KBISHIFT 20

//#define BUTTONx GPIO_28
//#define KBINUM 6
//#define KBISHIFT 22


const struct sensors_sensor button;

static struct timer debouncetimer;
static int status(int type);

void kbi4_isr(void) {
//	printf("kbi4_isr activated\n");
	if(timer_expired(&debouncetimer)) {
		printf("kbi4 if state\n");
		timer_set(&debouncetimer, CLOCK_SECOND / 4);
		sensors_changed(&button);
	}

//	clear_kbi_evnt(KBINUM);	//does same thing as below, old version
	CRM->STATUSbits.EXT_WU_EVT |= 0b0001;	//set to 1, which will cause automatic clearing (p.143)

//	printf("after %x", *CRM_STATUS);
}


static int
value(int type)
{
	printf("button: value called");
	return GPIO_READ(KBI4) || !timer_expired(&debouncetimer);
}

static int
configure(int type, int c)
{
//	printf("button: type:%i  c: %i\n",type, c);
	switch (type) {



	case SENSORS_ACTIVE:
		if (c) {
			if(!status(SENSORS_ACTIVE)) {
//				printf("button ACTIVE\n");
				timer_set(&debouncetimer, 0);
//				printf("pre-enable %x \n", *CRM_WU_CNTL);
				enable_irq_kbi(4);
//				printf("post-enable %x\n", *CRM_WU_CNTL);
			}
		} else {
//			printf("button ELSE reached: after %x\n", *CRM_STATUS);

//			disable_irq_kbi(KBINUM);	//does same thing as below, old version
			ITC->INTENABLEbits.CRM = 0; //disable external wake-up on kbi_6


		}
		return 1;
	}
	return 0;
}

static int
status(int type)
{
	printf("button status");
	switch (type) {
	case SENSORS_ACTIVE:
	case SENSORS_READY:
		//printf("crmwu_cntl %x %x\n", *CRM_WU_CNTL, bit_is_set(*CRM_WU_CNTL, KBISHIFT));
		return bit_is_set(*CRM_WU_CNTL, KBISHIFT); /* check if kbi4 irq is enabled ????*/
	}
	return 0;
}

SENSORS_SENSOR(button, BUTTON_SENSOR, value, configure, status);
