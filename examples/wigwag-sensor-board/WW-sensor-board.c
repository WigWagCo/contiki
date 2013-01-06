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



#include "random.h"

#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"

// framez
#include "irstream.pb-c.h"
#include "base64.h"
#include "frzprotocol.h"
#include "frzhwdevice.h"
#include "include/tricolor_led.h"
#include "gpio-util.h"
#include "include/interrupt_monitor.h"
//#include "dev/light-sensor.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


//#define DEBUG DEBUG_PRINT
#include "net/uip-debug.h"

//#define RANDOM_RAND_MAX 32U








#ifndef FRZ_PROTO_PORT
#define FRZ_PROTO_PORT 61617
#endif

#define UIP_IP_BUF   ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])


static struct uip_udp_conn *server_conn;

static uint16_t msg_id = 0;
static uint16_t seq_id = 0;
static char pay_buf[FRZ_MAX_TRANSFER_LEN + 1]; // pay load buffer
static char *bufmark = pay_buf;
static int proto_state = FRZ_WAITING;
static int trans_count = 0;

static resetConn() {
	msg_id = 0;
	seq_id = 0;
	bufmark = pay_buf;
	proto_state = FRZ_WAITING;
    /* Restore server connection to allow data from any node */
    memset(&server_conn->ripaddr, 0, sizeof(server_conn->ripaddr));
}

PROCESS(udp_server_process, "FRZ srv process");
AUTOSTART_PROCESSES(&udp_server_process);
/*---------------------------------------------------------------------------*/
static void
tcpip_handler(void)
{
	IRMessage *IRmsg;
	size_t msg_len;

	PRINTF("IN HANDLER...\n");
	if(uip_newdata()) {
		((char *)uip_appdata)[uip_datalen()] = 0;

		if(uip_datalen() < FRZ_HEADER_SIZE) {
			PRINTF("pack too small - discard\n");
			return;
		}

		PRINTF("RECV: SEQ %d on MSG %d (flags %x) len %d <-- ", FRZ_SEQ_ID(uip_appdata), FRZ_MSG_ID(uip_appdata), FRZ_FLAGS(uip_appdata), uip_datalen());
		PRINT6ADDR(&UIP_IP_BUF->srcipaddr);
		PRINTF("\n");

		if(FRZ_FLAGS(uip_appdata) & FRZ_FLAG_RESET) { // deal with protocol reset
			resetConn();
			if(uip_datalen() <= FRZ_HEADER_SIZE) {// if no data, and it was a reset - drop out - done
				uip_ipaddr_copy(&server_conn->ripaddr, &UIP_IP_BUF->srcipaddr);
				PRINTF("RESET ACK.\n");
				FRZ_FLAGS(uip_appdata) |= FRZ_FLAG_ACK; // flag ACK on this, got whole transfer
				uip_udp_packet_send(server_conn, uip_appdata, FRZ_HEADER_SIZE);
				return;
			}
		}

		if(proto_state == FRZ_PROCESSING) {
			PRINTF("BUSY\n");
			return;
		}

		if(proto_state == FRZ_WAITING) {
			if(FRZ_FLAGS(uip_appdata) & FRZ_FLAG_FIRST) {
				bufmark = pay_buf; // start bufmark out at front of static buffer
				proto_state = FRZ_IN_TRANSFER;
				seq_id = FRZ_SEQ_ID(uip_appdata);
				msg_id = FRZ_MSG_ID(uip_appdata);
				if(FRZ_SEQ_ID(uip_appdata) > 0) { // keeps malformed packs out
					PRINTF("protcol err 1\n");
					resetConn();
					return;
				}
				PRINTF("->IN-TRANS: %d\n",trans_count);
				trans_count++;
			}
		} else                                            // NOTE: we don't handle missing the first packet
			if(proto_state == FRZ_IN_TRANSFER) {
				if(FRZ_MSG_ID(uip_appdata) != msg_id) { // NOTE: we don't handle broken transfers at all
					PRINTF("discarding pack - w/ MSG ID %d unknown...", FRZ_MSG_ID(uip_appdata));
					proto_state = FRZ_WAITING;
					return;
				}
				seq_id++;
				if(seq_id != FRZ_SEQ_ID(uip_appdata)) { // NOTE: we don't handle out of order SEQ at all
					PRINTF("discarding pack - SEQ ID %d ", FRZ_SEQ_ID(uip_appdata));
					proto_state = FRZ_WAITING;
					return;
				}
			} else {
				PRINTF("discarded pack - unknown.\n");
				proto_state = FRZ_WAITING;
				return;
			}

		if(FRZ_MAX_TRANSFER_LEN - (bufmark-pay_buf) <  uip_datalen() - FRZ_HEADER_SIZE) { // do we have room??
			PRINTF("TX TOO BIG, fail.\n");
			uip_ipaddr_copy(&server_conn->ripaddr, &UIP_IP_BUF->srcipaddr);
			PRINTF("Resp w/ ACK.\n");

			FRZ_FLAGS(uip_appdata) |= (FRZ_FLAG_ACK | FRZ_FLAG_FAIL); // flag ACK, tell server we are failing transaction - too big for us
			FRZ_OPTIONAL(uip_appdata) = FRZ_ERROR_TOO_BIG;
			uip_udp_packet_send(server_conn, uip_appdata, FRZ_HEADER_SIZE);
			// NOTE: will need a RESET from server
			resetConn();
			return;
		}

		// check header to make sure this is for us... - not supporting out of order stuff yet...
		memcpy(bufmark,((char *) uip_appdata) + FRZ_HEADER_SIZE, uip_datalen() - FRZ_HEADER_SIZE);
		PRINTF("cp: %d %x\n",uip_datalen() - FRZ_HEADER_SIZE,bufmark);
		bufmark += (uip_datalen() - FRZ_HEADER_SIZE);
		if(!(FRZ_FLAGS(uip_appdata) & FRZ_FLAG_LAST)) {
			PRINTF("need more.\n");
			return;
		} else {
			//        bufmark = pay_buf; // reset bufmark
			proto_state = FRZ_PROCESSING;
		}

		uip_ipaddr_copy(&server_conn->ripaddr, &UIP_IP_BUF->srcipaddr);
		PRINTF("Resp w/ ACK.\n");

		FRZ_FLAGS(uip_appdata) |= FRZ_FLAG_ACK; // flag ACK on this, got whole transfer
		uip_udp_packet_send(server_conn, uip_appdata, FRZ_HEADER_SIZE);

		/* Restore server connection to allow data from any node */
		memset(&server_conn->ripaddr, 0, sizeof(server_conn->ripaddr));

		size_t post64len = 0;
		pay_buf[bufmark - pay_buf + 1] = '\0';
		PRINTF("b64: %s\n",pay_buf);

		char *post64buf = base64_decode(pay_buf, (unsigned int) (bufmark - pay_buf), &post64len ); // decode base64

		memcpy(pay_buf,post64buf,post64len);
		free(post64buf); // we do this to save memory... put the base64 decode back in the payload buffer (but keep the header, b/c we are going to send it back
		//    post64buf = pay_buf + FRZ_HEADER_SIZE;

		PRINTF("b64 dec: %d\n",post64len);
		/*#ifdef DEBUG
		 char *bufs = malloc(post64len+1);
		 memcpy(bufs,post64buf,post64len);
		 bufs[post64len] = '\0';
		 PRINTF("dec base64:[%s]\n", bufs);
		 free(bufs);
		 #endif
		 */
		char *str = stringify_bytes(pay_buf,post64len);
		PRINTF("decmp raw bytes (pre 64)  [%s]\n", str);
		free(str);


		IRmsg = irmessage__unpack (NULL, post64len, pay_buf); // Deserialize the serialized input

		PRINTF("n_c: %u\n",IRmsg->n_c);

		if(IRmsg->n_c < FRZ_BLAST_ARRAY_HEADER) { // don't accept malformed array
			PRINTF("msg too small.\n");
			resetConn();
		}

		decompress_nums(IRmsg->c + FRZ_BLAST_ARRAY_HEADER,IRmsg->n_c - FRZ_BLAST_ARRAY_HEADER); // decompress, skip header
		//    free(post64buf);

		// DO STUFF ... with packet:

		int i;
		for (i = 0; i < IRmsg->n_c; i++)
		{ // Iterate through all repeated int32
			if (i > 0)
				PRINTF (", ");
			PRINTF ("%d", IRmsg->c[i]);
		}
		PRINTF ("\n");


		if (IRmsg->c[1]==WW_TRICOLOR_LED)
			tricolor_process_array(IRmsg->c); // do IR blast





		irmessage__free_unpacked(IRmsg,NULL);

		resetConn();
	}
}//tcpip_handler
/*---------------------------------------------------------------------------*/
static void
print_local_addresses(void)
{
	int i;
	uint8_t state;

	PRINTF("Server IPv6 addresses: ");
	for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
		state = uip_ds6_if.addr_list[i].state;
		if(uip_ds6_if.addr_list[i].isused &&
		   (state == ADDR_TENTATIVE || state == ADDR_PREFERRED)) {
			PRINT6ADDR(&uip_ds6_if.addr_list[i].ipaddr);
			PRINTF("\n");
		}
	}
}
uint8_t buttoncount=0;
uint8_t rand1,rand2,rand3=0;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_server_process, ev, data)
{
#if UIP_CONF_ROUTER
	uip_ipaddr_t ipaddr;
#endif /* UIP_CONF_ROUTER */

	PROCESS_BEGIN();

	printf("just set buttoncount %i:\n",buttoncount);
	PRINTF("FRZ UDP server started\n");
	timer1_init();
	tricolor_init();
	random_init(555);
	SENSORS_ACTIVATE(button);



#if UIP_CONF_ROUTER
	PRINTF("UIP Conf router\n");
	uip_ip6addr(&ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0);
	uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
	uip_ds6_addr_add(&ipaddr, 0, ADDR_AUTOCONF);
#endif /* UIP_CONF_ROUTER */

	print_local_addresses();

	//  server_conn = udp_new(NULL, UIP_HTONS(FRZ_PROTO_PORT), NULL);
	//  udp_bind(server_conn, UIP_HTONS(FRZ_PROTO_PORT+1));
	server_conn = udp_new(NULL, UIP_HTONS(3000), NULL);
	udp_bind(server_conn, UIP_HTONS(3001));

	build_decoding_table(); // for base64 decode
	resetConn(); // setup initial protocol vars

	while(1) {
		PROCESS_YIELD();
		if(ev == tcpip_event) {
			tcpip_handler();
		}

	//	if (ev == sensors_event && data == &button_sensor_4) {
			if (ev == sensors_event ) {
			buttoncount=buttoncount+1;
			printf("The buttoncount: %i\n",buttoncount);
			//change_color_progressive(32500,random_rand(),random_rand(),random_rand());
			if (buttoncount ==1 ){
				tricolor_set(1,0,0);
			}
			else if (buttoncount ==2 ){
				tricolor_set(0,1,0);
			}
			else if (buttoncount ==3 ){
				tricolor_set(0,0,1);
			}
			else if (buttoncount ==4 ){
				tricolor_set(1,1,0);
			}
			else if (buttoncount ==5 ){
				tricolor_set(0,1,1);
			}
			else if (buttoncount ==6 ){
				tricolor_set(1,0,1);
			}
			else if (buttoncount ==7 ){
				tricolor_set(1,1,1);
			}
		}
	}

	PROCESS_END();
}
/*---------------------------------------------------------------------------*/
