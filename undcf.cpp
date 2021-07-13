// Qt
#include <qglobal.h>

int main(int argc, char *argv[]) {
	if (argc < 2) {
		qDebug(
			"MotoMAGX OMA DRM Hacker by EXL, v1.0, 14-Jun-2021\n"
			"https://github.com/EXL/OMA-DRM-Hacker\n\n"
			"Usage:\n"
			"\tundcf <in-file-path> <out-file-path>\n"
			"Example:\n"
			"\tundcf picture.dcf picture.png\n"
		);
	}
	return 0;
}
