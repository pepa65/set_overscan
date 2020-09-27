// overscan.c version 1.1.0 - Get/set overscan values on the fly
//
// This program needs a mailbox device. If not present, create it by:
//   sudo mknod /dev/mailbox c 100 0
// The setoverscan script does this for you if needed.
//
// Returns 0 if successful, positive integer for failure:
//   1: Unable to open device, 2: Ioctl error
//
// v1.1.0 Allow changing just some Overscan coordinates,
//   error on wrong number of arguments (not 0 or 4).
// v1.0.1 Complete rewrite from scratch of version 0.7,
//   some semblance of error checking added.

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define	GET_OVERSCAN 0x0004000a
#define	SET_OVERSCAN 0x0004800a
#define ZZZS 0x00000010
#define END_TAG 0

// Send property message to mailbox using ioctl
static int mailbox_property(int file_desc, void *buf) {
	int return_value = ioctl(file_desc, _IOWR(100, 0, char *), buf);
	// Ioctl error of some kind
	if (return_value < 0){
		close(file_desc);
		exit(2);
	}
	return return_value;
}

// Get the current values for overscan
static unsigned get_overscan(int file_desc, unsigned coord[4]) {
	int i=0;
	unsigned property[32];
	property[i++] = 0;
	property[i++] = 0;
	property[i++] = GET_OVERSCAN;
	property[i++] = ZZZS;
	property[i++] = 0;
	property[i++] = 0;
	property[i++] = 0;
	property[i++] = 0;
	property[i++] = 0;
	property[i++] = END_TAG;
	property[0] = i * sizeof *property;
	mailbox_property(file_desc, property);
	coord[0] = property[5]; // Top
	coord[1] = property[6]; // Bottom
	coord[2] = property[7]; // Left
	coord[3] = property[8]; // Right
	return 0;
}

// Set overscan values. No check is done that the values are sane or the
// operation is successful. (Check values by running the program again.)
static unsigned set_overscan(int file_desc, unsigned coord[4]) {
	int i=0;
	unsigned property[32];
	property[i++] = 0;
	property[i++] = 0;
	property[i++] = SET_OVERSCAN;
	property[i++] = ZZZS;
	property[i++] = ZZZS;
	property[i++] = coord[0]; // Top
	property[i++] = coord[1]; // Bottom
	property[i++] = coord[2]; // Left
	property[i++] = coord[3]; // Right
	property[i++] = END_TAG;
	property[0] = i * sizeof *property;
	mailbox_property(file_desc, property);
	coord[0] = property[5]; // Top
	coord[1] = property[6]; // Bottom
	coord[2] = property[7]; // Left
	coord[3] = property[8]; // Right
	return 0;
}

// Start program
int main(int argc, char *argv[]) {
	int file_desc;
	unsigned coord[4];
	file_desc = open("/dev/vcio", 0);
	if (file_desc == -1) exit(1);
	// Order of coordinates (commandline, output): top, bottom, left, right
	if (argc == 5) {
		get_overscan(file_desc, coord);
		unsigned c;
		for (int i=0; i<4; i++) {
			c = strtoul(argv[1+i], 0, 0);
			if (c == 0 && *argv[1+i] != '0') continue;
			coord[i] = c;
		}
		set_overscan(file_desc, coord);
	} else if (argc == 1) {
		get_overscan(file_desc, coord);
		printf("%d %d %d %d\n", coord[0], coord[1], coord[2], coord[3]);
	} else printf("Error: need 0 (Get) or 4 (Set) arguments, not: %d\n", argc-1);
	close(file_desc);
	return 0;
}
