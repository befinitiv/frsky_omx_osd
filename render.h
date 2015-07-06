#pragma once

#ifdef RENDER

#include "bcm_host.h"
#include "vgfont.h"


#include "telemetry.h"

typedef struct {
	GRAPHICS_RESOURCE_HANDLE img;
	uint32_t width;
	uint32_t height;
} render_data_t;

void render_init(render_data_t *rd, char *font_path);
void render(telemetry_data_t *td, render_data_t *rd);

#endif
