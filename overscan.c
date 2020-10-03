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

#define	OVERSCAN 0x0004000a
#define ZZZS 0x00000010
#define END_TAG 0
#define USAGE "Usage: overscan [<top> <bottom> <left> <right>]\n\
    If an argument is an unsigned integer, the overscan of the corresponding\n\
    edge gets set. If not, the overscan for that edge will not be changed.\n\
    Without arguments, current overscan values are displayed.\n"

// Send property message to mailbox using ioctl
static int mailbox_property(void *buf) {
	int file_desc = open("/dev/vcio", 0);
	if (file_desc == -1) {
		printf("Abort: device /dev/vcio not present\n");
		exit(1);
	}
	int return_value = ioctl(file_desc, _IOWR(100, 0, char *), buf);
	// Ioctl error of some kind
	if (return_value < 0){
		close(file_desc);
		printf("Abort: device /dev/vcio not accessible\n");
		exit(2);
	}
	return return_value;
}

// Get the current values for overscan
static unsigned get_overscan(unsigned coord[4]) {
	int i=0;
	unsigned property[32];
	property[i++] = 0;
	property[i++] = 0;
	property[i++] = OVERSCAN;
	property[i++] = ZZZS;
	property[i++] = 0;
	property[i++] = 0;
	property[i++] = 0;
	property[i++] = 0;
	property[i++] = 0;
	property[i++] = END_TAG;
	property[0] = i * sizeof *property;
	mailbox_property(property);
	coord[0] = property[5]; // Top
	coord[1] = property[6]; // Bottom
	coord[2] = property[7]; // Left
	coord[3] = property[8]; // Right
	return 0;
}

// Set overscan values. No check is done that the values are sane or the
// operation is successful. (Check values by running the program again.)
static unsigned set_overscan(unsigned coord[4]) {
	int i=0;
	unsigned property[32];
	property[i++] = 0;
	property[i++] = 0;
	property[i++] = OVERSCAN;
	property[i++] = ZZZS;
	property[i++] = ZZZS;
	property[i++] = coord[0]; // Top
	property[i++] = coord[1]; // Bottom
	property[i++] = coord[2]; // Left
	property[i++] = coord[3]; // Right
	property[i++] = END_TAG;
	property[0] = i * sizeof *property;
	mailbox_property(property);
	coord[0] = property[5]; // Top
	coord[1] = property[6]; // Bottom
	coord[2] = property[7]; // Left
	coord[3] = property[8]; // Right
	return 0;
}

// Start program
int main(int argc, char *argv[]) {
	unsigned coord[4];
	if (argc == 5) {
		get_overscan(coord);
		unsigned c;
		for (int i=0; i<4; i++) {
			c = strtoul(argv[1+i], 0, 0);
			if (c == 0 && *argv[1+i] != '0') continue;
			coord[i] = c;
		}
		set_overscan(coord);
	} else if (argc == 1) {
		get_overscan(coord);
		printf("%d %d %d %d\n", coord[0], coord[1], coord[2], coord[3]);
	} else printf(USAGE);
	return 0;
}
