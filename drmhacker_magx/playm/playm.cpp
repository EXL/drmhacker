// C
#include <cstdlib>
#include <cstdio>

// EZX
#include <ezxsound.h>
#include <MP_PlayerEngine.h>

static int PlayFile(const char *aPathIn) {
	fprintf(stderr, "1\n");
	AM_NORMAL_DEV_INTERFACE *lAmNormalDevInterface = new AM_NORMAL_DEV_INTERFACE();
	fprintf(stderr, "2\n");

	MP_PlayerEngine *lMP_PlayerEngine = MP_CreateRingtoneEngine(lAmNormalDevInterface, NULL);
	fprintf(stderr, "3\n");
//	MP_PlayerEngine *lMP_PlayerEngine = MP_CreatePlayerEngine(lAmNormalDevInterface, NULL);

//	lMP_PlayerEngine->open(aPathIn, false);
//	lMP_PlayerEngine->open(aPathIn, true);
	fprintf(stderr, "4\n");
	lMP_PlayerEngine->open(aPathIn);

//	TODO: Why is there segmentation fault?
	lMP_PlayerEngine->play();
	fprintf(stderr, "6\n");
//	lMP_PlayerEngine->play(false);
//	lMP_PlayerEngine->play(true);

	lMP_PlayerEngine->close();
	fprintf(stderr, "8\n");
	delete lMP_PlayerEngine;
	fprintf(stderr, "9\n");

	lAmNormalDevInterface->close();
	fprintf(stderr, "10\n");
	delete lAmNormalDevInterface;
	fprintf(stderr, "11\n");
	return 0;
}

int main(int argc, char *argv[]) {
	if (argc < 2)
		return 1;
	return PlayFile(argv[1]);
}
