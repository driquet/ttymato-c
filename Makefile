# ttymato Makefile
# See LICENCE file
#

SRC = $(wildcard *.c)
HDR = $(wildcard *.h)
OBJ = $(patsubst %.c,%.o,$(SRC))
CC  = gcc
BIN = ttymato
LDFLAGS = -lncurses
CFLAGS  = -Wall -g

all: ${BIN}

%.o: %.c ${HDR}
	@echo "[*] Building $@"
	@${CC} -o $@ ${CFLAGS} -c $<

${BIN}: ${OBJ}
	@echo "[*] Building $@"
	@${CC} $^ -o $@ ${LDFLAGS}

clean:
	@echo "[*] Cleaning ${BIN}"
	rm ${BIN} ${OBJ}
