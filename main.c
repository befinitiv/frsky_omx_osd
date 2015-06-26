#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>

#include "frsky.h"

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


void frsky_interpret_packet(void) {
	//printf("%x\n", pkg[0]);
	uint16_t data;

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
			//printf("%x\n", pkg[0]);
		break;
	}
}

void parse_frsky_buffer(uint8_t ch) {
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
				frsky_interpret_packet();
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



int main(void) {
	uint8_t buf[256];
	size_t n;


	for(;;) {
		n = read(STDIN_FILENO, buf, sizeof(buf));

		if(n == 0) {
			break; //EOF
		}

		if(n<0) {
			perror("read");
			exit(-1);
		}

		int i;
		for(i=0; i<n; ++i)
			parse_frsky_buffer(buf[i]);

	}

	return 0;
}



