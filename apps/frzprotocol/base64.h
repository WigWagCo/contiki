/*
 * base64.h
 *
 *  Created on: Aug 30, 2011
 *      Author: ed
 */

#ifndef BASE64_H_
#define BASE64_H_

/**
 *
 * @param data
 * @param input_length
 * @param output_length
 * @return char * to output - free upon use of return value...
 */
char *base64_encode(const char *data,
                    size_t input_length,
                    size_t *output_length);

char *base64_decode(const char *data,
                    size_t input_length,
                    size_t *output_length);

char *stringify_bytes(char *b, int len);


void build_decoding_table();

void base64_cleanup();

#endif /* BASE64_H_ */
