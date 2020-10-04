// overscan.c version 1.1.1 - Get/set overscan values directly
//
// Mailbox device required, if not present:
//   sudo mknod /dev/vcio c 100 0
// (The setoverscan script does this automatically.)
//
// Returns 0 if successful, positive integer for failure:
//   1: Unable to open device, 2: Ioctl error
//
// v1.1.1 Help message instead of error on number of arguments,
//   error on mailbox device not present or not accessible.
// v1.1.0 Allow changing just some Overscan coordinates,
//   error on wrong number of arguments (not 0 or 4).
// v1.0.1 Complete rewrite from scratch of version 0.7,
//   some semblance of error checking added.

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define TOP 0
#define BOTTOM 1
#define LEFT 2
#define RIGHT 3
#define OFFSET 5
#define PROPERTYSIZE 32
#define	OVERSCAN 0x0004000a
#define ZZZS 0x00000010
#define ZERO 0
#define ENDTAG ZERO
#define USAGE "Usage: overscan [<top> <bottom> <left> <right>]\n\
    If an argument is an integer, the overscan of the corresponding edge\n\
    gets set. If not, the overscan for that edge will not be changed.\n\
    The eventual overscan values are always displayed.\n"

// Send property message to mailbox using ioctl
static void mailbox(void *buf) {
	int fd = open("/dev/vcio", 0);
	if (fd == -1) {
		printf("Abort: device /dev/vcio not present\n");
		exit(1);
	}
	if (ioctl(fd, _IOWR(100, 0, char *), buf) < 0) {
		close(fd);
		printf("Abort: device /dev/vcio not accessible\n");
		exit(2);
	}
}

// Get the current values for overscan
static void get_overscan(__int32_t coord[4]) {
	__int32_t property[PROPERTYSIZE];
	property[0] = 10*PROPERTYSIZE;
	property[1] = ZERO;
	property[2] = OVERSCAN;
	property[3] = ZZZS;
	property[4] = ZERO;
	property[5] = ZERO; // OFFSET + TOP
	property[6] = ZERO; // OFFSET + BOTTOM
	property[7] = ZERO; // OFFSET + LEFT
	property[8] = ZERO; // OFFSET + RIGHT
	property[9] = ENDTAG;
	mailbox(property);
	coord[TOP] = property[TOP+OFFSET];
	coord[BOTTOM] = property[BOTTOM+OFFSET];
	coord[LEFT] = property[LEFT+OFFSET];
	coord[RIGHT] = property[RIGHT+OFFSET];
}

// Set overscan values. No checks for sane values or successful operation.
static void set_overscan(__int32_t coord[4]) {
	__int32_t property[32];
	property[0] = 10*PROPERTYSIZE;
	property[1] = ZERO;
	property[2] = OVERSCAN;
	property[3] = ZZZS;
	property[4] = ZZZS;
	property[5] = coord[TOP];
	property[6] = coord[BOTTOM];
	property[7] = coord[LEFT];
	property[8] = coord[RIGHT];
	property[9] = ENDTAG;
	mailbox(property);
	coord[TOP] = property[TOP+OFFSET];
	coord[BOTTOM] = property[BOTTOM+OFFSET];
	coord[LEFT] = property[LEFT+OFFSET];
	coord[RIGHT] = property[RIGHT+OFFSET];
}

// Start program
int main(int argc, char *argv[]) {
	if (argc != 5 && argc != 1) {
		printf(USAGE);
		return 3;
	}
	__int32_t coord[4];
	get_overscan(coord);
	if (argc == 5) {
		__int32_t c;
		char *endptr;
		for (int i=0; i<4; i++) {
			c = strtol(argv[i+1], &endptr, 0);
			// Only set if fully parsable
			if (*endptr == '\0' && endptr != argv[i+1]) coord[i] = c;
		}
		set_overscan(coord);
		// Get feedback on the operation
		get_overscan(coord);
	}
	printf("Overscan top/bottom/left/right: %d %d %d %d\n",
		coord[0], coord[1], coord[2], coord[3]);
	return 0;
}
