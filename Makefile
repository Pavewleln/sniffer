TARGET = sniffer
CC = gcc
CFLAGS = -Wall -Wextra `pkg-config --cflags gtk+-3.0`
LDFLAGS = `pkg-config --libs gtk+-3.0`
RM = rm -rf
INTERFACE_SRCS = windowInit.c
MAIN_SRCS = main.c erproc.c utils.c transportLayer.c applicationLayer.c
SRCS = $(MAIN_SRCS) interface/$(INTERFACE_SRCS)
BUILD_DIR = build
OBJS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: %.c include | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/interface/%.o: interface/%.c include | $(BUILD_DIR)/interface
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/interface:
	mkdir -p $(BUILD_DIR)/interface

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	$(RM) $(BUILD_DIR) $(TARGET)