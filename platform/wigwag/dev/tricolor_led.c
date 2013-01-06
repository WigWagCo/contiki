/**
 * \file
 *         Tri-Color Dome LED controller
 * \author
 *         Travis McCollum trav@wigwag.com
 */
#include "contiki.h"
#include "include/tricolor_led.h"
#include "gpio.h"
#include "gpio-util.h"
#include "daughter_board_definitions.h"

void tricolor_init(void) {
	GPIO_SETUP_OUTPUT(tricolor_LEDred);
	GPIO_SETUP_OUTPUT(tricolor_LEDblue);
	GPIO_SETUP_OUTPUT(tricolor_LEDgreen);
	GPIO_OFF(LEDred);
	GPIO_OFF(LEDblue);
	GPIO_OFF(LEDgreen);
}

void tricolor_set(uint8_t red, uint8_t green, uint8_t blue){
	printf("set called red:%i green:%i blue:%i\n",red,green,blue);
	if (red){
		GPIO_ON(LEDred);
	}
	else {
		GPIO_OFF(LEDred);
	}
	if (blue) {
		GPIO_ON(LEDblue);
	}
	else {
		GPIO_OFF(LEDblue);
	}
	if (green){
		GPIO_ON(LEDgreen);
	}
	else {
		GPIO_OFF(LEDgreen);
	}
}

void tricolor_process_array(uint32_t *ray){
	tricolor_set(ray[2],ray[3],ray[4]);
}
