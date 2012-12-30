/*
 * base54.c
 *
 *  Created on: Aug 30, 2011
 *      Author: ed
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "base64.h"

static char enc_tab[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'};
static char *decoding_table = NULL;
//static int mod_table[] = {0, 2, 1};

/**
 *
 * @param data
 * @param input_length
 * @param output_length
 * @return char * to output - free upon use of return value...
 */
char *base64_encode(const char *data,
                    size_t input_length,
                    size_t *output_length) {

//	*output_length = (size_t) (4.0 * ceil((double) input_length / 3.0));
	*output_length = (input_length / 3);
	if((input_length % 3) > 0)
		*output_length += 1;
    *output_length = *output_length * 4;

    char *encoded_data = malloc(*output_length);
    if (encoded_data == NULL) return NULL;

    int i; int j = 0;
    int len;

//    for (i = 0, j = 0; i < input_length;) {
    for (i = 0; i < input_length;) {

/*        uint32_t octet_a = i < input_length ? data[i++] : 0;
        uint32_t octet_b = i < input_length ? data[i++] : 0;
        uint32_t octet_c = i < input_length ? data[i++] : 0;

        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
*/

    	len = ((input_length-i) > 2) ? 3 : input_length-i;
//    	fprintf(stderr,"len: %d %d data[i]=%x\n",len,j,data[i]);
        encoded_data[j] = enc_tab[ ((unsigned char) data[i]) >> 2 ];
//        fprintf(stderr,"%c (%x) ",encoded_data[j],encoded_data[j]);
        j++;
        encoded_data[j] = enc_tab[ ((data[i] & 0x03) << 4) | ((data[i+1] & 0xf0) >> 4) ];
//       fprintf(stderr,"%c (%x) ",encoded_data[j],encoded_data[j]);
        j++;
        encoded_data[j] = (unsigned char) (len > 1 ? enc_tab[ ((data[i+1] & 0x0f) << 2) | ((data[i+2] & 0xc0) >> 6) ] : '=');
//        fprintf(stderr,"%c (%x) ",encoded_data[j],encoded_data[j]);
        j++;
        encoded_data[j] = (unsigned char) (len > 2 ? enc_tab[ data[i+2] & 0x3f ] : '=');
//        fprintf(stderr,"%c (%x) \n",encoded_data[j],encoded_data[j]);
        j++;
    	i+= 3;
    }

//    for (i = 0; i < mod_table[input_length % 3]; i++)
//       encoded_data[*output_length - 1 - i] = '=';
    *output_length = j;
//    fprintf(stderr,"final len: %d\n",*output_length);
    return encoded_data;
}


char *base64_decode(const char *data,
                    size_t input_length,
                    size_t *output_length) {

    if (decoding_table == NULL) build_decoding_table();

    if (input_length % 4 != 0) return NULL;

    *output_length = input_length / 4 * 3;
    if (data[input_length - 1] == '=') (*output_length)--;
    if (data[input_length - 2] == '=') (*output_length)--;

    char *decoded_data = malloc(*output_length);
    if (decoded_data == NULL) return NULL;
    int i; int j;
    for (i = 0, j = 0; i < input_length;) {

        uint32_t sextet_a = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_b = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_c = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_d = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];

        uint32_t triple = (sextet_a << 3 * 6)
                        + (sextet_b << 2 * 6)
                        + (sextet_c << 1 * 6)
                        + (sextet_d << 0 * 6);

        if (j < *output_length) decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
        if (j < *output_length) decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
        if (j < *output_length) decoded_data[j++] = (triple >> 0 * 8) & 0xFF;

/*
 *
 *  http://base64.sourceforge.net/b64.c - probably faster...
        v = (unsigned char) ((v < 43 || v > 122) ? 0 : cd64[ v - 43 ]);
        decoded_data[ 0 ] = (unsigned char ) (in[j] << 2 | in[j+1] >> 4);
        decoded_data[ 1 ] = (unsigned char ) (in[j+1] << 4 | in[j+2] >> 2);
        decoded_data[ 2 ] = (unsigned char ) (((in[j+2] << 6) & 0xc0) | in[j+3]);
        j+=4;
*/
    }

    return decoded_data;
}


void build_decoding_table() {

    decoding_table = malloc(256);
    int i;
    for (i = 0; i < 0x40; i++)
        decoding_table[enc_tab[i]] = i;
}


void base64_cleanup() {
    free(decoding_table);
}

char *stringify_bytes(char *b, int len) {
	char *ret = malloc(len*5+2);
	*ret = '\0';
	int x = 0;
	unsigned char out;
	if(ret) {
	for(x=0;x<len;x++) {
		out = (unsigned char) *(b+x) & 0xFF;
		snprintf((ret + (strlen(ret))), (len*5+2)-strlen(ret),"%#x,", out);
	}
	} // else malloc failed
	return ret;
}
