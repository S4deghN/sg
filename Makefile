CFLAGS := -Wall -Wextra -g -O0
LDFLAGS := -lm

main: main.c sg.h
	cc $(CFLAGS) $(LDFLAGS) $^ -o $@
