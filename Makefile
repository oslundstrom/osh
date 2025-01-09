CC = gcc
CFLAGS = -I./unity/src -I.

.PHONY: all test
	
all: osh

osh: main.c
	gcc main.c -o osh

test_runner: CFLAGS += -DTESTING -g
test_runner: test_osh.c main.c unity/src/unity.c | unity
	$(CC) $(CFLAGS) $^ -o test_runner

test: test_runner
	./$<

unity:
	git clone https://github.com/ThrowTheSwitch/Unity.git unity

clean:
	rm osh test_runner
