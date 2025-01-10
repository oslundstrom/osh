CC = gcc
CFLAGS = -I./unity/src -I. -g

.PHONY: all test
	
all: osh

osh: main.c
	gcc main.c -o osh

test_runner: CFLAGS += -DTESTING 
test_runner: test_osh.c main.c unity/src/unity.c
	$(CC) $(CFLAGS) $^ -o test_runner

check: test_runner
	./$<

unity/src/unity.c:
	git clone https://github.com/ThrowTheSwitch/Unity.git unity

clean:
	rm osh test_runner
