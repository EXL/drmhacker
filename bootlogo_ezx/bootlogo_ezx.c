/*
 * EZX Boot Logo Extractor by EXL
 *
 * Converting CG39.smg code group to the GIF images.
 *
 * Created: 25-Sep-2021
 *
 * Compile:
 *  $ gcc -Wall -pedantic -O2 bootlogo_ezx.c -o bootlogo_ezx
 */

#include <stdio.h>
#include <stdint.h>
#include <limits.h>

static const uint32_t GIF_HEADER_PATTERN = 0x38464947; /* "GIF8" in GIF87a or GIF89a header magic. */
static const uint32_t GIF_ENDING_PATTERN = 0xFFFFFF3B; /* "3B FF FF FF" GIF ending sequence. */

static int32_t ErrUsage(void) {
	fprintf(
		stderr,
		"|EZX Boot Logo Extractor| by EXL, v1.0, 25-Sep-2021\n\n"
		"Usage:\n"
		"\t./bootlogo_ezx <CG39 SMG file>\n\n"
		"Example:\n"
		"\t./bootlogo_ezx CG39.smg\n"
	);
	return 1;
}

static int32_t ErrFile(const char *aFileName, const char *aMode) {
	fprintf(stderr, "Cannot open '%s' file for %s.\n", aFileName, aMode);
	return 1;
}

static uint32_t GetNeedle(FILE *aReadFile, FILE *aWriteFile, uint64_t *aOffset, uint8_t aCanWriteFlag) {
	const uint64_t lStartPos = ftell(aReadFile) - 1;
	fseek(aReadFile, lStartPos, SEEK_SET);
	uint32_t lNeedle;
	fread(&lNeedle, sizeof(uint32_t), 1, aReadFile);
	if (aCanWriteFlag && (lNeedle != GIF_ENDING_PATTERN)) {
		fseek(aWriteFile, ftell(aWriteFile) - 1, SEEK_SET);
		fwrite(&lNeedle, sizeof(uint32_t), 1, aWriteFile);
	}
	*aOffset = lStartPos;
	return lNeedle;
}

static int32_t FindImagesInFile(FILE *aReadFile) {
	uint8_t lCanWriteFlag = 0;
	uint8_t lByte = 0x00;
	uint32_t lCounter = 0;
	uint64_t lOffset;
	char lWriteFileName[PATH_MAX] = { '\0' };
	FILE *lGifFile = NULL;
	while (fread(&lByte, 1, 1, aReadFile)) {
		if (lCanWriteFlag)
			fwrite(&lByte, 1, 1, lGifFile);
		if (lByte == 0x47 && !lCanWriteFlag /* 'G' in GIF87a or GIF89a header magic. */
				&& (GetNeedle(aReadFile, lGifFile, &lOffset, lCanWriteFlag) == GIF_HEADER_PATTERN)) {
			++lCounter;
			fprintf(stdout, "Found GIF image #%d on offset '%#lx'...\n", lCounter, lOffset);
			fseek(aReadFile, lOffset, SEEK_SET);
			lCanWriteFlag = 1;
			snprintf(lWriteFileName, PATH_MAX, "bootlogo_%d.gif", lCounter);
			lGifFile = fopen(lWriteFileName, "wb");
		} else if (lByte == 0x3B && lCanWriteFlag /* ';' is ending byte. */
				&& (GetNeedle(aReadFile, lGifFile, &lOffset, lCanWriteFlag) == GIF_ENDING_PATTERN)) {
			fprintf(stdout, "Found GIF ending sequence on offset '%#lx'...\n", lOffset);
			lCanWriteFlag = 0;
			fclose(lGifFile);
		}
	}
	fclose(aReadFile);
	return 0;
}

int main(int argc, char *argv[]) {
	if (argc < 2)
		return ErrUsage();

	FILE *lSmgFile = fopen(argv[1], "rb");
	if (!lSmgFile)
		return ErrFile(argv[1], "read");

	return FindImagesInFile(lSmgFile);
}
