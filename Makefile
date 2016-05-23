# ttymato Makefile
# See LICENCE file
#

MANDIR = /usr/share/man/man1
SRC = $(wildcard src/*.c)
HDR = $(wildcard include/*.h)
OBJ = $(patsubst %.c,%.o,$(SRC))
CC  = gcc
BIN = ttymato
LDFLAGS = -lncurses -lnotify `pkg-config --libs --cflags glib-2.0 gtk+-2.0`
CFLAGS  = -Wall -Werror -g -Iinclude `pkg-config --cflags --libs libnotify`

all: ${BIN}

man: ${BIN}.1

install-man: man
	install -m 755 -d $(MANDIR)
	install -m 644 $(BIN).1 $(MANDIR)

uninstall-man:
	rm -f $(MANDIR)/$(BIN).1

${BIN}.1: ${BIN}.1.ronn
	ronn -w -r $<

%.o: %.c ${HDR}
	@echo "[*] Building $@"
	${CC} -o $@ ${CFLAGS} -c $<

${BIN}: ${OBJ}
	@echo "[*] Building $@"
	${CC} $^ -o $@ ${LDFLAGS}

clean:
	@echo "[*] Cleaning ${BIN}"
	rm ${BIN} ${OBJ}

.PHONY: all clean install-man uninstall-man
