# ttymato Makefile
# See LICENCE file
#

SRC = $(wildcard *.c)
HDR = $(wildcard *.h)
OBJ = $(patsubst %.c,%.o,$(SRC))
CC  = gcc
BIN = ttymato
LDFLAGS = -lncurses -lnotify `pkg-config --libs --cflags glib-2.0 gtk+-2.0`
CFLAGS  = -Wall -g  `pkg-config --cflags --libs libnotify`

all: ${BIN}

%.o: %.c ${HDR}
	@echo "[*] Building $@"
	${CC} -o $@ ${CFLAGS} -c $<

${BIN}: ${OBJ}
	@echo "[*] Building $@"
	${CC} $^ -o $@ ${LDFLAGS}

clean:
	@echo "[*] Cleaning ${BIN}"
	rm ${BIN} ${OBJ}
