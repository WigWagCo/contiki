/*
 * quickcompress.c
 *
 *  Created on: Aug 30, 2011
 *      Author: ed
 */

#include <google/protobuf-c/protobuf-c.h>

void decompress_nums(int32_t *nums, int count) {
	int x = 0;
	while(x<count) {
		nums[x] = nums[x] << 3;
		x++;
	}
}

void compress_nums(int32_t *nums, int count) {
	int x = 0;
	while(x<count) {
		nums[x] = nums[x] >> 3;
		x++;
	}
}
