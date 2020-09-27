CC = gcc
CFLAGS  = -g -Wall -std=c99 -fPIC
prefix = /usr
TARGET = overscan
script = setoverscan

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

clean:
	-rm -f $(TARGET)

distclean: clean

install: $(TARGET)
	install -D $(TARGET) $(DESTDIR)$(prefix)/bin/$(TARGET)
	install -D $(script) $(DESTDIR)$(prefix)/bin/$(script)
	install -D doc/$(TARGET).1 $(DESTDIR)$(prefix)/share/man/man1/$(TARGET).1
	install -D doc/$(script).8 $(DESTDIR)$(prefix)/share/man/man8/$(script).8

uninstall:
	-rm -f $(DESTDIR)$(prefix)/bin/$(TARGET)
	-rm -f $(DESTDIR)$(prefix)/bin/$(script)
	-rm -f $(DESTDIR)$(prefix)/share/man/man1/$(TARGET).1
	-rm -f $(DESTDIR)$(prefix)/share/man/man8/$(script).8

.PHONY: all clean install
