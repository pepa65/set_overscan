# setoverscan
**Adjust Raspberry Pi overscan interactively**

* Repo: https://github.com/pepa65/setoverscan
* Install: `make; sudo make install`
* Note: _On the Pi 4 this doesn't work and isn't needed._

## overscan
**Get/set Raspberry Pi overscan directly**

Usage: `overscan [<top> <bottom> <left> <right>]`

If the arguments are unsigned integers, they will set the overscan of
the corresponding edge, if not, that edge will not be changed.
This example modifies top and right, and leaves bottom and left unchanged:
    overscan 16 - - 24
Without arguments, the current values are displayed on stdout.

## setoverscan
**Adjust Raspberry Pi overscan interactively**

* Usage: `bash setoverscan`
* Required: coreutils(od dd head cat stty) grep sed ncurses-bin(tput clear)
libraspberrypi-bin(vcgencmd) fbset whiptail overscan

> Although I do most of my Raspberry Pi messing around on a headless Raspberry
Pi via ssh my "gaming" Raspberry Pi is not always connected to the same
monitor it was yesterday or will be tomorrow and because all my monitors are
different makes and models I often found myself squinting at the monitor to
count pixels so I could change the overscan values in /boot/config.txt and
then rebooting to find that i'd miscounted and needed to make more changes to
the overscan settings and repeating that until I got it just right.
>
> As this was beginning to get annoying I asked the Raspberry Pi kernel guru
Dom Cobley if he had an example of how to twiddle the overscan settings after
booting and he sent me the code to do that. I've taken his program mailbox.c
& made a few little changes to create overscan.c and have wrapped that in a
simple bash shell script to make changes to the Raspberry Pi's overscan
setting on the fly easy.
>
> Because I need to use mknod to create a special file the script must be run
by root or using sudo.
>
> Use the included Makefile to build overscan and then run the set-overscan
script. You currently use the arrow/cursor keys to change each setting in
turn & the q key to move to the next overscan value to change. The script
writes the final values to /boot/config.txt
>
> Russell "ukscone" Davis 2013-03-10
