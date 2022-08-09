#
# Sample Makefile for the shared buffer lab.
#

MAKEFILE_PATH := $(shell dirname "$(abspath $(lastword $(MAKEFILE_LIST)))")

#
# Ask for debugging and warnings, and enable pthreads.
#
override CFLAGS += -O2 -g -Wall -I include
override LFLAGS += -pthread


all:    exchange pi

.PHONY: all

build/%.o: src/%.c
	mkdir -p "$(MAKEFILE_PATH)/$(shell dirname $@)"
	$(CC) $(CFLAGS) -c $< -o $@

pi: build/pi/main.o build/pi/calculate_pi.o
	$(CC) $(LFLAGS) -o pi $^

exchange: build/exchange/main.o build/exchange/exchange.o build/exchange/generic_circular_buffer.o build/exchange/message_circular_buffer.o
	$(CC) $(CFLAGS) $(LFLAGS) -o exchange $^

clean:
	rm -f exchange
	rm -f pi
	rm -f *.o
	rm -rf build
	rm -f *.json
	rm -f stderr.txt
	rm -f stdout.txt
	rm -f expected.txt
	rm -f file.txt
