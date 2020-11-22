#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "rpi.h"
#include "video.h"
#include "mem.h"
#include "debug.h"
#include "sysctl.h"

static void *fb_pixels;
static int scr_width, scr_height;
static int fb_width, fb_height, fb_depth, fb_size, fb_pitch;
static int fb_xoffs, fb_yoffs;

int video_init(void)
{
	int i, j;
	struct rpi_prop *prop;
	uint32_t *fbptr;

	scr_width = 1024;
	scr_height = 600;
	/*fb_width = 1920;
	fb_height = 1024;*/
	fb_width = scr_width;
	fb_height = scr_height;
	fb_depth = 32;

	printf("Requesting video mode: %dx%d %d bpp (fb:%dx%d)\n", scr_width, scr_height,
			fb_depth, fb_width, fb_height);

	rpi_prop(RPI_TAG_ALLOCFB, 16);
	rpi_prop(RPI_TAG_SETFBVIRT, scr_width, scr_height);
	rpi_prop(RPI_TAG_SETFBPHYS, fb_width, fb_height);
	rpi_prop(RPI_TAG_SETFBDEPTH, fb_depth);

	rpi_prop_send();

	while((prop = rpi_prop_next())) {
		switch(prop->id) {
		case RPI_TAG_SETFBPHYS:
			fb_width = prop->data[0];
			fb_height = prop->data[1];
			break;

		case RPI_TAG_SETFBVIRT:
			scr_width = prop->data[0];
			scr_height = prop->data[1];
			break;

		case RPI_TAG_SETFBDEPTH:
			fb_depth = prop->data[0];
			break;

		case RPI_TAG_ALLOCFB:
			fb_pixels = (void*)(prop->data[0] & 0x3fffffff);
			fb_size = prop->data[1];
			break;

		default:
			break;
		}
	}

	if(!fb_pixels) {
		rpi_prop(RPI_TAG_ALLOCFB, 16);
		if(rpi_prop_send() == -1 || !(prop = rpi_prop_find(RPI_TAG_ALLOCFB))) {
			printf("Failed to allocate framebuffer\n");
			return -1;
		}
		fb_pixels = (void*)(prop->data[0] & 0x3fffffff);
		fb_size = prop->data[1];
	}

	rpi_prop(RPI_TAG_GETFBPITCH);
	rpi_prop(RPI_TAG_GETFBOFFS);
	rpi_prop_send();
	/*
	if(rpi_prop_send() == -1) {
		printf("Failed to get pitch\n");
		return -1;
	}
	*/

	while((prop = rpi_prop_next())) {
		switch(prop->id) {
		case RPI_TAG_GETFBPITCH:
			fb_pitch = prop->data[0];
			break;

		case RPI_TAG_GETFBOFFS:
			fb_xoffs = prop->data[0];
			fb_yoffs = prop->data[1];
			break;

		default:
			break;
		}
	}

	printf("Got video mode: %dx%d (%d bpp)\n", scr_width, scr_height, fb_depth);
	printf("Framebuffer: %dx%d at %p (%d bytes)\n", fb_width, fb_height, fb_pixels, fb_size);
	printf("  virtual offset: %d, %d\n", fb_xoffs, fb_yoffs);
	printf("  scanline pitch: %d\n", fb_pitch);

	fbptr = fb_pixels;
	for(i=0; i<fb_height; i++) {
		for(j=0; j<fb_width; j++) {
			int r = i ^ j;
			int g = (i ^ j) >> 1;
			int b = (i ^ j) >> 2;
			fbptr[j] = b | (g << 8) | (r << 16) | 0xff000000;
		}
		fbptr += fb_pitch >> 2;
	}

	sysctl_dcache_clean_inval((uint32_t)fb_pixels, fb_size);
	return 0;
}

int video_scroll(int x, int y)
{
	struct rpi_prop *prop;

	rpi_prop(RPI_TAG_SETFBOFFS, x, y);
	if(rpi_prop_send() == -1 || !(prop = rpi_prop_find(RPI_TAG_SETFBOFFS))) {
		return -1;
	}

	fb_xoffs = prop->data[0];
	fb_yoffs = prop->data[1];
	return 0;
}

void video_update(int dt)
{
	static int dirx = 1, diry = 1;
	int nx, ny;

	nx = fb_xoffs + dirx * dt;
	ny = fb_yoffs + diry * dt;

	if(nx < 0) {
		nx = 0;
		dirx = -dirx;
	} else if(nx + scr_width >= fb_width) {
		nx = fb_width - scr_width;
		dirx = -dirx;
	}
	if(ny < 0) {
		ny = 0;
		diry = -diry;
	} else if(ny + scr_height >= fb_height) {
		ny = fb_height - scr_height;
		diry = -diry;
	}

	video_scroll(nx, ny);
}
