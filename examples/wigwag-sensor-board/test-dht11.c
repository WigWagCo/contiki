/*
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
 * This file is part of the Contiki operating system.
 *
 */


#include <stdio.h>
#include "contiki.h"
#include "dev/button-sensor2.h"
#include "dev/dht11.h"
#include <stdio.h>
#include "gpio.h"
char test;






/*---------------------------------------------------------------------------*/
PROCESS(test_button_process, "Econo Test button with DHT11");
AUTOSTART_PROCESSES(&test_button_process);
/*---------------------------------------------------------------------------*/
static uint8_t active;

PROCESS_THREAD(test_button_process, ev, data)
{
  PROCESS_BEGIN();
  active = 0;
  DHT11_init_Write();
  SENSORS_ACTIVATE(button_sensor);


  while(1) {

    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event &&
			     data == &button_sensor);
    printf("Oh boy here we go\n");
    DHT11_test2();
//   	if(!DHT11_handshake()) printf("faild handshake\n");
//   	else printf ("success handshake\n");

   //	DHT11_test();
   	//else printf("success handshake");
  //test = DHT11_Fetch();
  //  test =0;
 //  	if (test) DHT11_output();
 //   else printf("test failed\n");
   	DHT11_init_Write();
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
