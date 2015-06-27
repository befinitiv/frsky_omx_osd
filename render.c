#include "conf.h"

#include "telemetry.h"

#ifdef RENDER

const float MIN_VOLTAGE = 10.5;


void render_init(render_data_t *rd, char *font_path) {
	bcm_host_init();
	gx_graphics_init(font_path);
	graphics_get_display_size(0, &rd->width, &rd->height);
	gx_create_window(0, rd->width, rd->height, GRAPHICS_RESOURCE_RGBA32, &rd->img);

	// transparent before display to avoid screen flash
	graphics_resource_fill(rd->img, 0, 0, rd->width, rd->height, GRAPHICS_RGBA32(0,0,0,0x00));
	graphics_display_resource(rd->img, 0, LAYER, 0, 0, GRAPHICS_RESOURCE_WIDTH, GRAPHICS_RESOURCE_HEIGHT, VC_DISPMAN_ROT0, 1);
}

void render_text(char *text, render_data_t *rd, int x, int y, int fsize, uint8_t r, uint8_t g, uint8_t b) {
	graphics_resource_render_text_ext(*rd->img, x, y,
	GRAPHICS_RESOURCE_WIDTH,
	GRAPHICS_RESOURCE_HEIGHT,
	GRAPHICS_RGBA32(b,g,r, 0xff), /* fg */
	GRAPHICS_RGBA32(0,0,0,0x80), /* bg */
	text, strlen(text), fsize);

}

void render(telemetry_data_t *td, render_data_t *rd) {
	char text[256];
	int text_y = rd->height - 30;


	graphics_resource_fill(*rd->img, 0, 0, rd->width, rd->height, GRAPHICS_RGBA32(0,0,0,0x00));

	//green line
	//graphics_resource_fill(*img, 0, height-40, width, 1, GRAPHICS_RGBA32(0,0xff,0,0xff));

	
	sprintf(text, "%.2fV", td->voltage);
	render_text(text, rd, 2, text_y, 20, 0xff, td->voltage<MIN_VOLTAGE?0:0xff, td->voltage<MIN_VOLTAGE?0:0xff);
	
	sprintf(text, "%dm", (int)td->altitude);
	render_text(text, rd, 120, text_y, 20, 0xff, 0xff, 0xff);

	sprintf(text, "%dkm/h", (int)td->speed);
	render_text(text, rd, 220, text_y, 20, 0xff, 0xff, 0xff);

	sprintf(text, "%d deg", (int)td->heading);
	render_text(text, rd, 320, text_y, 20, 0xff, 0xff, 0xff);

	sprintf(text, "%f", td->latitude);
	render_text(text, rd, 420, text_y, 20, 0xff, 0xff, 0xff);

	sprintf(text, "%f", td->longitude);
	render_text(text, rd, 620, text_y, 20, 0xff, 0xff, 0xff);


	graphics_update_displayed_resource(*rd->img, 0, 0, 0, 0);

}
#endif

