#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "rpi.h"
#include "video.h"
#include "mem.h"
#include "debug.h"

/* needs to by 16-byte aligned, because the address we send over the mailbox
 * interface, will have its 4 least significant bits masked off and taken over
 * by the mailbox id
 */
static char propbuf[256] __attribute__((aligned(16)));

static void *fb_pixels;
static int fb_width, fb_height, fb_depth, fb_size;

int video_init(void)
{
	int i;
	struct rpi_prop_header *hdr = (struct rpi_prop_header*)propbuf;
	struct rpi_prop *prop = (struct rpi_prop*)(hdr + 1);

	hdr->size = sizeof propbuf;
	hdr->res = 0;

	fb_width = 1024;
	fb_height = 600;
	fb_depth = 32;

	prop->id = RPI_TAG_SETFBPHYS;
	prop->size = 8;
	prop->res = 0;
	prop->data[0] = fb_width;
	prop->data[1] = fb_height;
	prop = RPI_PROP_NEXT(prop);

	prop->id = RPI_TAG_SETFBVIRT;
	prop->size = 8;
	prop->res = 0;
	prop->data[0] = fb_width;
	prop->data[1] = fb_height;
	prop = RPI_PROP_NEXT(prop);

	prop->id = RPI_TAG_SETFBDEPTH;
	prop->size = 4;
	prop->res = 0;
	prop->data[0] = fb_depth;
	prop = RPI_PROP_NEXT(prop);

	prop->id = RPI_TAG_ALLOCFB;
	prop->size = 4;
	prop->res = 0;
	prop->data[0] = 4;	/* alignment */
	prop = RPI_PROP_NEXT(prop);

	prop->id = 0;
	prop->size = 0;
	prop->res = 0;
	prop = RPI_PROP_NEXT(prop);

	printf("Requesting video mode: %dx%d (%d bpp)\n", fb_width, fb_height, fb_depth);

	rpi_mbox_send(RPI_MBOX_PROP, RPI_MEM_BUS_COHERENT(propbuf));
	while(rpi_mbox_recv(RPI_MBOX_PROP) != RPI_MEM_BUS_COHERENT(propbuf));

	hexdump(propbuf, sizeof propbuf);

	if(hdr->res != 0x80000000) {
		printf("Request failed, error: %x\n", hdr->res);
		return -1;
	}

	prop = (struct rpi_prop*)(hdr + 1);
	while(prop->id) {
		prop->size = prop->res & 0x7fffffff;

		switch(prop->id) {
		case RPI_TAG_SETFBPHYS:
			printf("setfbphys");
			break;

		case RPI_TAG_SETFBVIRT:
			printf("setfbvirt");
			fb_width = prop->data[0];
			fb_height = prop->data[1];
			break;

		case RPI_TAG_SETFBDEPTH:
			printf("setfbdepth");
			fb_depth = prop->data[0];
			break;

		case RPI_TAG_ALLOCFB:
			printf("allocfb");
			fb_pixels = (void*)prop->data[0];
			fb_size = prop->data[1];
			break;

		default:
			printf("tag %x", prop->id);
			break;
		}

		printf(" %08x (%u bytes):", prop->res, prop->size);
		for(i=0; i<prop->size / 4; i++) {
			printf(" %u", prop->data[i]);
		}
		putchar('\n');
		prop = RPI_PROP_NEXT(prop);
	}

	printf("Got video mode: %dx%d (%d bpp) at %p (%d bytes)\n", fb_width, fb_height,
			fb_depth, fb_pixels, fb_size);

	memset(fb_pixels, 0, fb_size);
	return 0;
}
