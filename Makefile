CC ?= gcc
CFLAGS = -Wall -Wextra -std=c99 -g
TARGET = stego
SRCS = test_encode.c encode.c decode.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) a.out *.exe output.txt

.PHONY: all clean
