#include <stdio.h>
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

#define paddleTilesLen 8
#define paddleTilesBytes paddleTilesLen*numBytes

const unsigned int leftPaddleTiles[paddleTilesLen] = {0x11110000,
													  0x11110000,
													  0x11110000,
													  0x11110000,
													  0x11110000,
													  0x11110000,
													  0x11110000,
													  0x11110000,};
const unsigned int rightPaddleTiles[paddleTilesLen] = {0x00001111,
													   0x00001111,
													   0x00001111,
													   0x00001111,
													   0x00001111,
													   0x00001111,
													   0x00001111,
													   0x00001111,};

#define numPaddleTiles 32

#define paddleTilePixelLength 32
#define paddleTilePixelWidth 8

#define xOffset 20

OBJ_ATTR obj_buffer[128];
OBJ_AFFINE *obj_aff_buffer= (OBJ_AFFINE*)obj_buffer;

void draw()
{
	int y=32;
	u32 palette_bank=0;

	OBJ_ATTR *paddle_left= &obj_buffer[0];
	obj_set_attr(paddle_left,
				 ATTR0_TALL | ATTR0_4BPP,
				 ATTR1_SIZE_16,
				 ATTR2_PALBANK(palette_bank));

	OBJ_ATTR *paddle_right= &obj_buffer[1];
	obj_set_attr(paddle_right,
				 ATTR0_TALL | ATTR0_4BPP,
				 ATTR1_SIZE_16,
				 ATTR2_PALBANK(palette_bank) | numPaddleTiles);

	while (1) {
		vid_vsync();
		key_poll();

		// move up/down
		y += 2*key_tri_vert();

		// make it glow
		palette_bank = key_is_down(KEY_SELECT) ? 1 : 0;

		if (y <= 0) {
			y = 1;
		} else if (y+paddleTilePixelLength >= SCREEN_HEIGHT) {
			y = SCREEN_HEIGHT-paddleTilePixelLength-1;
		}

		// Build left paddle
		paddle_left->attr2= ATTR2_BUILD(0, palette_bank, 0);
		obj_set_pos(paddle_left, xOffset, y);

		// Build right paddle
		paddle_right->attr2= ATTR2_BUILD(numPaddleTiles, palette_bank, 0);
		obj_set_pos(paddle_right, SCREEN_WIDTH-xOffset-paddleTilePixelWidth, y);

		// Updates sprites
		oam_copy(oam_mem, obj_buffer, 2);
	}
}

int main() {
	int i = 0;
	for (; i < numPaddleTiles; i += 1) {
		memcpy(tile_mem[4]+i, leftPaddleTiles, paddleTilesBytes);
	}
	for (int j = 0; j < numPaddleTiles+i; j += 1) {
		memcpy(tile_mem[4]+i+j, rightPaddleTiles, paddleTilesBytes);
	}

	memcpy(pal_obj_mem, pongPal, pongPalBytes);

	oam_init(obj_buffer, 128);
	REG_DISPCNT= DCNT_OBJ | DCNT_OBJ_1D;

	draw();

    while(1);

    return 0;
}
