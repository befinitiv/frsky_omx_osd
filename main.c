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


#include "conf.h"

#ifdef RENDER
#include "bcm_host.h"
#include "vgfont.h"
#endif



#include "telemetry.h"
#include "frsky.h"




int main(int argc, char *argv[]) {
	#ifdef RENDER
	render_data_t rd;
	#endif

	uint8_t buf[256];
	size_t n;
	frsky_state_t fs;
	telemetry_data_t td;


	if(argc != 2) {
		printf("Usage: %s <font path>\n",argv[0]);
		return 1;
	}

	#ifdef RENDER
	render_init(&rd, argv[1]);
	#endif


	for(;;) {
		n = read(STDIN_FILENO, buf, sizeof(buf));

		if(n == 0) {
		//	break; //EOF
		}

		if(n<0) {
			perror("read");
			exit(-1);
		}

		if(frsky_parse_buffer(&fs, &td, buf, n)) {
			#ifdef RENDER
			render(&td, &rd);
			#endif
		}


	}

	return 0;
}



