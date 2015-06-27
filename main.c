/*
Copyright (c) 2015, befinitiv
Copyright (c) 2012, Broadcom Europe Ltd
All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
* Neither the name of the copyright holder nor the
names of its contributors may be used to endorse or promote products
derived from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CA

#include <stdio.h>ND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


#include "bcm_host.h"
#include "vgfont.h"


#include "frsky.h"


const float MIN_VOLTAGE = 10.5;

#define DEBUG 1
#define debug_print(fmt, ...) \
            do { if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)


int sm_state = 0;
uint8_t pkg[256];
int pkg_pos = 0;


float voltage = -1;
float altitude = -1;

double longitude = 0.0;
double latitude = 0.0;

float heading = -1;

float speed = -1;


int frsky_interpret_packet(void) {
	//printf("%x\n", pkg[0]);
	uint16_t data;
	int new_data = 1;

	data = *(uint16_t*)(pkg+1);
	switch(pkg[0]) {
		case ID_VOLT:
		{
			uint16_t val = (pkg[2] >> 8) | ((pkg[1] & 0xf) << 8);
			float battery = 3.0 * val / 500;
			voltage = battery;
			debug_print("Battery voltage: %f\n", voltage);
		}
		break;

		case ID_GPS_ALTIDUTE_BP:
			altitude = data;
			debug_print("Altitude: %f\n", altitude);
			break;

		case ID_LONGITUDE_BP:
			longitude = data / 100;
			longitude += 1.0 * (data - longitude * 100) / 60;
			break;
		case ID_LONGITUDE_AP:
			longitude +=  1.0 * data / 60 / 10000;
			debug_print("Longitude: %f\n", longitude);
			break;

		case ID_LATITUDE_BP:
			latitude = data / 100;
			latitude += 1.0 * (data - latitude * 100) / 60;
			break;
		case ID_LATITUDE_AP:
			latitude +=  1.0 * data / 60 / 10000;
			debug_print("Latitude: %f\n", latitude);
			break;


		case ID_COURSE_BP:
			heading = data;
			debug_print("Heading: %f\n", heading);
			break;

		case ID_GPS_SPEED_BP:
			speed = 1.0 * data / 0.0194384449;
			break;

		case ID_GPS_SPEED_AP:
			speed += 1.0 * data / 1.94384449; //now we are in cm/s
			speed = speed / 100 / 1000 * 3600; //now we are in km/h
			debug_print("Speed: %f\n", speed);
			break;

		default:
			new_data = 0;
			//printf("%x\n", pkg[0]);
		break;
	}

	return new_data;
}

int parse_frsky_buffer(uint8_t *buf, int buflen) {
	int new_data = 0;


	int i;
	for(i=0; i<buflen; ++i) {
		uint8_t ch = buf[i];
		//printf("parse %x state %d\n", ch, sm_state);
		switch(sm_state) {
			case 0:
				if(ch == 0x5e)
					sm_state = 1;
				break;

			case 1:
				if(ch == 0x5e)
					sm_state = 2;
				else
					sm_state = 0;
				break;
			
			case 2:
				if(ch == 0x5e) {
					pkg_pos = 0;
					new_data = new_data | frsky_interpret_packet();
				}
				else {
					if(pkg_pos >= sizeof(pkg)) {
						pkg_pos = 0;
						sm_state = 0;
					} else {
						pkg[pkg_pos] = ch;
						pkg_pos++;
					}
				}
			break;

			default:
				sm_state = 0;
			break;
		}
	}

	return new_data;
}




void render_text(char *text, GRAPHICS_RESOURCE_HANDLE *img, int x, int y, int fsize, uint8_t r, uint8_t g, uint8_t b) {
	graphics_resource_render_text_ext(*img, x, y,
	GRAPHICS_RESOURCE_WIDTH,
	GRAPHICS_RESOURCE_HEIGHT,
	GRAPHICS_RGBA32(b,g,r, 0xff), /* fg */
	GRAPHICS_RGBA32(0,0,0,0x80), /* bg */
	text, strlen(text), fsize);

}

void render(GRAPHICS_RESOURCE_HANDLE *img, int width, int height) {
	char text[256];
	int text_y = height - 30;


	graphics_resource_fill(*img, 0, 0, width, height, GRAPHICS_RGBA32(0,0,0,0x00));

//green line
graphics_resource_fill(*img, 0, height-40, width, 1, GRAPHICS_RGBA32(0,0xff,0,0xff));

	



	sprintf(text, "%.2fV", voltage);
	render_text(text, img, 2, text_y, 20, 0xff, voltage<MIN_VOLTAGE?0:0xff, voltage<MIN_VOLTAGE?0:0xff);
	
	sprintf(text, "%dm", (int)altitude);
	render_text(text, img, 120, text_y, 20, 0xff, 0xff, 0xff);

	sprintf(text, "%dkm/h", (int)speed);
	render_text(text, img, 220, text_y, 20, 0xff, 0xff, 0xff);

	sprintf(text, "%d deg", (int)heading);
	render_text(text, img, 320, text_y, 20, 0xff, 0xff, 0xff);

	sprintf(text, "%f", latitude);
	render_text(text, img, 420, text_y, 20, 0xff, 0xff, 0xff);

	sprintf(text, "%f", longitude);
	render_text(text, img, 620, text_y, 20, 0xff, 0xff, 0xff);


graphics_update_displayed_resource(*img, 0, 0, 0, 0);

}

/*
double longitude = 0.0;
double latitude = 0.0;

float heading = -1;

float speed = -1;
*/


int main(void) {
	GRAPHICS_RESOURCE_HANDLE img;
	uint32_t width, height;
	const int LAYER=1;
	uint8_t buf[256];
	size_t n;

	bcm_host_init();
	gx_graphics_init(".");
	graphics_get_display_size(0, &width, &height);
	gx_create_window(0, width, height, GRAPHICS_RESOURCE_RGBA32, &img);


	// transparent before display to avoid screen flash
	graphics_resource_fill(img, 0, 0, width, height, GRAPHICS_RGBA32(0,0,0,0x00));
	graphics_display_resource(img, 0, LAYER, 0, 0, GRAPHICS_RESOURCE_WIDTH, GRAPHICS_RESOURCE_HEIGHT, VC_DISPMAN_ROT0, 1);



	for(;;) {
		n = read(STDIN_FILENO, buf, sizeof(buf));

		if(n == 0) {
		//	break; //EOF
		}

		if(n<0) {
			perror("read");
			exit(-1);
		}

		if(parse_frsky_buffer(buf, n)) {
			render(&img, width, height);
		}


	}

	return 0;
}



