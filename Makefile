CC="gcc"
CFLAGS="-Wall"

all:
	$(CC) -o brightness $(CFLAGS) brightness.c

clean:
	rm -f brightness

install: all
	cp brightness /usr/local/bin/brightness

uninstall:
	rm -f /usr/local/bin/brightness
