TARGET = sniffer
CC = gcc
CFLAGS = -Wall -Wextra
RM = rm -rf
SRCS = main.c erproc.c utils.c transportLayer.c applicationLayer.c
BUILD_DIR = build
OBJS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: %.c include/*.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	$(RM) $(BUILD_DIR) $(TARGET)