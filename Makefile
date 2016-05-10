# ttymato Makefile
# See LICENCE file
#

SRC = $(wildcard src/*.c)
HDR = $(wildcard include/*.h)
OBJ = $(patsubst %.c,%.o,$(SRC))
CC  = gcc
BIN = ttymato
LDFLAGS = -lncurses -lnotify `pkg-config --libs --cflags glib-2.0 gtk+-2.0`
CFLAGS  = -Wall -g -Iinclude `pkg-config --cflags --libs libnotify`

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
