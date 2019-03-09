#include <string.h>
#include "tonc_input.h"
#include "tonc_memmap.h"
#include "tonc_oam.h"
#include "tonc_video.h"

#define numBytes 4

#define pongPalLen 16
#define pongPalBytes pongPalLen*numBytes
										  // Palette 1
const unsigned int pongPal[pongPalLen] = {0xFFFF0000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
										  // Palette 2
										  0x001F0000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000};

#define pongTilesLen 32
#define pongTilesBytes pongTilesLen*numBytes
const unsigned int pongTiles[pongTilesLen] = {0x11112233,
											  0x11110000,
											  0x11110000,
											  0x11110000,
											  0x11110000,
											  0x11110000,
											  0x11110000,
											  0x11110000,
											  0x11110000,
											  0x11110000,
											  0x11110000,
											  0x11110000,
											  0x11110000,
											  0x11110000,
											  0x11110000,
											  0x11110000,
											  0x11110000,
											  0x11110000,
											  0x11110000,
											  0x11110000,
											  0x11110000,
											  0x11110000,
											  0x11110000,
											  0x11110000,
											  0x11110000,
											  0x11110000,
											  0x11110000,
											  0x11110000,
											  0x11110000,
											  0x11110000,
											  0x11110000,
											  0x11110000,};

OBJ_ATTR obj_buffer[128];
OBJ_AFFINE *obj_aff_buffer= (OBJ_AFFINE*)obj_buffer;

// testing a few sprite things
// D-pad: move
// SELECT: switch palette
// START: toggle mapping mode
// A: horizontal flip
// B: vertical flip
// L & R shift starting tile
void obj_test()
{
	int x=96, y=32;
	u32 palette_bank= 0;

	OBJ_ATTR *paddle= &obj_buffer[0];
	obj_set_attr(paddle,
				 ATTR0_TALL | ATTR0_4BPP,
				 ATTR1_SIZE_16,
				 ATTR2_PALBANK(palette_bank));

	// position sprite
	obj_set_pos(paddle, x, y);

	while(1)
	{
		vid_vsync();
		key_poll();

		// move left/right
		x += 2*key_tri_horz();

		// move up/down
		y += 2*key_tri_vert();

		// make it glow
		palette_bank = key_is_down(KEY_SELECT) ? 1 : 0;

		// Build paddle
		paddle->attr2= ATTR2_BUILD(0, palette_bank, 0);
		obj_set_pos(paddle, x, y);

		// Updates sprites
		oam_copy(oam_mem, obj_buffer, 1);
	}
}

int main() {
	memcpy(&tile_mem[4][0], pongTiles, pongTilesBytes);
	memcpy(pal_obj_mem, pongPal, pongPalBytes);

	oam_init(obj_buffer, 128);
	REG_DISPCNT= DCNT_OBJ | DCNT_OBJ_1D;

	obj_test();

    while(1);

    return 0;
}
