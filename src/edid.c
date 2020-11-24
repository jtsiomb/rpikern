#include <stdio.h>
#include "edid.h"

#define HAVE_DIM(edid) ((edid)->hsize && (edid)->vsize)

static void decode_vendor(char *id, uint16_t v);
static int decode_detailed(struct edid_timdet *det, struct edid_vmode *vm);
const char *inp_digital_interface(unsigned char type);
int inp_digital_coldepth(unsigned char type);

static struct edid_vmode est_vm[] = {
	{800, 600, 60},
	{800, 600, 56},
	{640, 480, 75},
	{640, 480, 72},
	{640, 480, 67},
	{640, 480, 60},
	{720, 400, 88},
	{720, 400, 70},

	{1280, 1024, 75},
	{1024, 768, 75},
	{1024, 768, 70},
	{1024, 768, 60},
	{1024, 768, 87},
	{832, 624, 75},
	{800, 600, 75},
	{800, 600, 72},

	{1152, 870, 75}
};

int edid_best_mode(struct edid *edid, struct edid_vmode *vm)
{
	return decode_detailed(edid->tim_det, vm);
}

void print_edid(struct edid *edid)
{
	int i, xsz, ysz, vrate;
	char vid[4];
	unsigned char *ptr;

	printf("EDID v%d.%d\n", edid->ver, edid->rev);

	decode_vendor(vid, edid->vendor);
	printf("Vendor: %s\n", vid);

	if(edid->inptype & EDID_INPTYPE_DIGITAL) {
		int cbits = inp_digital_coldepth(edid->inptype);
		printf("Digital signal: %s", inp_digital_interface(edid->inptype));
		if(cbits >= 0) {
			printf(" (color depth: %dbit)\n", cbits);
		} else {
			putchar('\n');
		}
	} else {
		printf("Analog signal\n");
	}

	if(HAVE_DIM(edid)) {
		printf("dimensions: %dx%d cm\n", edid->hsize, edid->vsize);
	}

	printf("Established modes:\n");
	ptr = &edid->tim_est1;
	for(i=0; i<sizeof est_vm / sizeof *est_vm; i++) {
		int bit = i & 7;

		if(*ptr & (1 << bit)) {
			printf(" %dx%d %dhz\n", est_vm[i].width, est_vm[i].height, est_vm[i].vrate);
		}

		if(bit == 7) ptr++;
	}

	printf("Standard modes:\n");
	ptr = edid->tim_std;
	for(i=0; i<8; i++) {
		if(!(ptr[0] | ptr[1])) continue;
		if(i > 0 && ptr[0] == ptr[-2] && ptr[1] == ptr[-1]) continue;

		xsz = ((int)ptr[0] + 31) << 3;
		switch(ptr[1] >> 6) {
		case 0:
			ysz = xsz * 10 / 16;
			break;
		case 1:
			ysz = xsz * 3 / 4;
			break;
		case 2:
			ysz = xsz * 4 / 5;
			break;
		case 3:
			ysz = xsz * 9 / 16;
		}
		vrate = (ptr[1] & 0x3f) + 60;

		printf(" %dx%d %dhz\n", xsz, ysz, vrate);
		ptr += 2;
	}

	printf("Detailed modes:\n");
	for(i=0; i<4; i++) {
		struct edid_vmode vm;

		if(decode_detailed(edid->tim_det + i, &vm) != -1) {
			printf(" %dx%d %dhz\n", vm.width, vm.height, vm.vrate);
		}
	}
}

static void decode_vendor(char *id, uint16_t v)
{
	unsigned char *ptr = (unsigned char*)&v;

	id[0] = ((ptr[0] & 0x7f) >> 2) + 'A' - 1;
	id[1] = (((ptr[0] & 3) << 2) | ((ptr[1] & 0xe0) >> 5)) + 'A' - 1;
	id[2] = (ptr[1] & 0x1f) + 'A' - 1;
	id[3] = 0;
}

static int decode_detailed(struct edid_timdet *det, struct edid_vmode *vm)
{
	int pck, hfull, vfull;

	if(det->pclock == 0) {
		return -1;	/* it's a display descriptor */
	}

	pck = (int)det->pclock * 10000;
	vm->width = det->hpixels_low | ((int)(det->hhigh & 0xf0) << 4);
	vm->height = det->vpixels_low | ((int)(det->vhigh & 0xf0) << 4);

	hfull = vm->width + (det->hblank_low | ((int)(det->hhigh & 0xf) << 8));
	vfull = vm->height + (det->vblank_low | ((int)(det->vhigh & 0xf) << 8));

	vm->vrate = (pck / hfull) / vfull;
	return 0;
}

const char *inp_digital_interface(unsigned char type)
{
	switch(type & 0xf) {
	case 1: return "DVI";
	case 2: return "HDMI-a";
	case 3: return "HDMI-b";
	case 4: return "MDDI";
	case 5: return "DisplayPort";
	default:
		break;
	}
	return "unknown";
}

int inp_digital_coldepth(unsigned char type)
{
	switch((type >> 4) & 7) {
	case 1: return 6;
	case 2: return 8;
	case 3: return 10;
	case 4: return 12;
	case 5: return 14;
	case 6: return 16;
	default:
		break;
	}
	return -1;
}
