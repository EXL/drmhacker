/*
 * MotoMAGX Boot Logo Extractor by EXL
 *
 * Converting CG42.smg and CG41.smg code groups to the BMP images.
 *
 * Created: 23-Sep-2021
 * Updated: 28-Jan-2022
 *
 * R         G         B         W         B         Y         C         M:
 * 00 00 FF  00 FF 00  FF 00 00  FF FF FF  00 00 00  00 FF FF  FF FF 00  FF 00 FF
 * 00 F0 03  C0 0F 00  3F 00 00  FF FF 03  00 00 00  C0 FF 03  FF 0F 00  3F F0 03
 *
 * Compile:
 *  $ gcc -Wall -Wextra -pedantic -O2 bootlogo_magx.c -o bootlogo_magx
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define RGB666_TO_RGB888(color) \
	( (((color) & (0x3F <<  0)) <<  2) | \
	(  ((color) & (0x3F <<  6)) <<  4) | \
	(  ((color) & (0x3F << 12)) <<  6))

#define RGB565_TO_RGB888(color) \
	(  (((color & 0xFF) & 0xF8) << 16) | \
	( ((((color & 0xFF) & 0x07) <<  5) | (((color >> 8) & 0xE0) >> 3)) << 8) | \
	(   ((color >> 8)   & 0x1F) <<  3))

typedef struct {
	int32_t width;
	int32_t height;
	uint32_t size;
	uint32_t depth;
} display_t;

/* See: https://en.wikipedia.org/wiki/BMP_file_format */
#pragma pack(push, 1)
typedef struct {
	/* Bitmap file header */
	uint16_t file_magic;
	uint32_t file_size;
	uint32_t bytes_reserved;
	uint32_t bitmap_start;
	/* DIB header (bitmap information header) */
	uint32_t dib_header_size;
	int32_t bitmap_width;
	int32_t bitmap_height;
	uint16_t color_planes;
	uint16_t bitmap_bpp;
	uint32_t compression_method;
	uint32_t bitmap_size;
	int32_t bitmap_width_ppm;
	int32_t bitmap_height_ppm;
	uint32_t num_of_colors;
	uint32_t num_of_important_colors;
} bmp_header_t;
#pragma pack(pop)

static int32_t ErrUsage(void) {
	fprintf(
		stderr,
		"|MotoMAGX Boot Logo Extractor| by EXL, v1.0, 28-Jan-2022\n\n"
		"Usage:\n"
		"\t./bootlogo_magx <screen size> <CG42 SMG file> <BMP image file>\n\n"
		"\t./bootlogo_magx <screen size> <CG41 SMG file> <BMP image file> <bpp>\n\n"
		"Example:\n"
		"\t./bootlogo_magx 240x320 CG42.smg bootlogo.bmp\n"
		"\t./bootlogo_magx 128x160 CG41.smg bootlogo_cli.bmp\n"
		"\t./bootlogo_magx 96x80 CG41.smg bootlogo_cli.bmp 16\n"
	);
	return 1;
}

static int32_t ErrDisplay(const display_t *aDisplay) {
	fprintf(stderr, "Wrong display size: '%dx%d'.\n", aDisplay->width, aDisplay->height);
	return 1;
}

static int32_t ErrFile(const char *aFileName, const char *aMode) {
	fprintf(stderr, "Cannot open '%s' file for %s.\n", aFileName, aMode);
	return 1;
}

static int32_t ParseDisplay(display_t *aDisplay, const char *aScreenSize) {
	sscanf(aScreenSize, "%dx%d", &(aDisplay->width), &(aDisplay->height));
	if (aDisplay->height <= 0 || aDisplay->width <= 0 || aDisplay->height > 640 || aDisplay->width > 640)
		return 1;
	aDisplay->size = aDisplay->height * aDisplay->width;
	aDisplay->depth = 24;
	return 0;
}

static uint32_t *CreateBitmapFromFile(FILE *aReadFile, const display_t *aDisplay, const int aDepth) {
	uint32_t *lBitmapRgb888 = malloc(aDisplay->size * sizeof(uint32_t));
	uint32_t res = 0;
	for (int32_t y = 0; y < aDisplay->height; ++y)
		for (int32_t x = 0; x < aDisplay->width; ++x) {
			switch (aDepth) {
				default: {
					uint32_t lPixelRgb666 = 0x000000;
					res = fread(&lPixelRgb666, 3, 1, aReadFile); /* RGB666 in RGB888, 3 bytes, 18-bit or 24-bit for color. */
					if (res) {
						lBitmapRgb888[x + y * aDisplay->width] = RGB666_TO_RGB888(lPixelRgb666);
					}
					break;
				}
				case 16: {
					uint32_t lPixelRgb565 = 0x000000;
					res = fread(&lPixelRgb565, 2, 1, aReadFile); /* RGB565 in RGB888, 2 bytes, 16-bit for color. */
					if (res) {
						lBitmapRgb888[x + y * aDisplay->width] = RGB565_TO_RGB888(lPixelRgb565);
					}
					break;
				}
			}
		}
	return lBitmapRgb888;
}

static void WriteBmpHeader(FILE *aWriteFile, const display_t *aDisplay) {
	bmp_header_t lBmpHeader = { 0x0 };
	lBmpHeader.file_magic = 0x4D42;
	lBmpHeader.file_size = aDisplay->size * 3 + 14 + 40; /* RGB888/24/3, BMP header, DIB header. */
	lBmpHeader.bitmap_start = 0x00000036;
	lBmpHeader.dib_header_size = 0x00000028;
	lBmpHeader.bitmap_width = aDisplay->width;
	lBmpHeader.bitmap_height = aDisplay->height;
	lBmpHeader.color_planes = 0x0001;
	lBmpHeader.bitmap_bpp = 0x0018;
	lBmpHeader.bitmap_size = aDisplay->size * 3; /* RGB888/24/3. */
	fwrite(&lBmpHeader, sizeof(bmp_header_t), 1, aWriteFile);
}

static void WriteBmpBitmap(FILE *aWriteFile, const display_t *aDisplay, const uint32_t *aBitmap) {
	for (int32_t y = aDisplay->height - 1; y >= 0; --y)
		for (int32_t x = 0; x < aDisplay->width; ++x) {
			uint32_t lPixelRgb888 = aBitmap[x + y * aDisplay->width];
			fwrite(&lPixelRgb888, 3, 1, aWriteFile);
		}
}

int main(int argc, char *argv[]) {
	if (argc < 4)
		return ErrUsage();

	display_t lScreen;
	if (ParseDisplay(&lScreen, argv[1]))
		return ErrDisplay(&lScreen);

	FILE *lSmgFile = fopen(argv[2], "rb");
	if (!lSmgFile)
		return ErrFile(argv[2], "read");
	uint32_t *lBitmap = CreateBitmapFromFile(lSmgFile, &lScreen, (argc == 5) ? atoi(argv[4]) : 0);
	fclose(lSmgFile);

	FILE *lBmpFile = fopen(argv[3], "wb");
	if (!lBmpFile)
		return ErrFile(argv[3], "write");
	WriteBmpHeader(lBmpFile, &lScreen);
	WriteBmpBitmap(lBmpFile, &lScreen, lBitmap);
	free(lBitmap);
	fclose(lBmpFile);

	return 0;
}
