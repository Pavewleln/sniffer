TARGET = sniffer

CC = gcc

CFLAGS = -Wall -Wextra

RM = rm -rf

SRCS = main.c erproc.c

OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c include/*.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS) $(TARGET)