#ifndef EDID_H_
#define EDID_H_

#include <stdint.h>

struct edid_vmode {
	int width, height, vrate;
};

struct edid_timdet {
	uint16_t pclock;	/* pixel clock / 10000 */
	unsigned char hpixels_low, hblank_low;
	unsigned char hhigh;	/* high nibble: pixels, low nibble: blanking */
	unsigned char vpixels_low, vblank_low;
	unsigned char vhigh;
	unsigned char hfp_low, hsync_low;
	unsigned char vfp_low, vsync_low;
	unsigned char fp_sync_high;	/* [hf|hs|vf|vs] */
	unsigned char hsize_low, vsize_low;
	unsigned char size_high;	/* [horiz|vert] */
	unsigned char hborder, vborder;
} __attribute__((packed));

struct edid {
	unsigned char id[8];	/* id: 00 ff ff ff ff ff ff 00 */

	uint16_t vendor, product;
	uint32_t serial;
	unsigned char week, year;

	unsigned char ver, rev;

	unsigned char inptype;
	unsigned char hsize, vsize;
	unsigned char gamma;
	unsigned char features;

	unsigned char col_rglow, col_bwlow;
	unsigned char col_rx_high, col_ry_high;
	unsigned char col_gx_high, col_gy_high;
	unsigned char col_bx_high, col_by_high;
	unsigned char col_wx_high, col_wy_high;

	unsigned char tim_est1, tim_est2, tim_rsvd;
	unsigned char tim_std[16];
	struct edid_timdet tim_det[4];

	unsigned char count, csum;
} __attribute__((packed));

#define EDID_INPTYPE_DIGITAL	0x80

int edid_best_mode(struct edid *edid, struct edid_vmode *vm);
void print_edid(struct edid *edid);

#endif	/* EDID_H_ */
