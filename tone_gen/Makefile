CC=gcc
CFLAGS= -Wall -Werror
INCLUDES=
LIBS=-lm
PLUGIN_NAME=tone_gen
BUILD_DIR=build
PLUGIN_DIR = $(BUILD_DIR)/$(PLUGIN_NAME).lv2
DIRS= $(BUILD_DIR) $(PLUGIN_DIR)
$(shell mkdir -p $(DIRS))

SRC_DIR = src

SOURCES = $(SRC_DIR)/tone_gen.c

DATA = manifest.ttl \
			 tone_gen.ttl

PLUGIN_DATA = $(patsubst %,$(PLUGIN_DIR)/%,$(DATA))

_OBJS = $(SOURCES:.c=.o)
OBJS = $(patsubst $(SRC_DIR)/%,$(BUILD_DIR)/%,$(_OBJS))

.PHONY: all

all: $(PLUGIN_DIR)/$(PLUGIN_NAME).so $(PLUGIN_DATA)

$(BUILD_DIR)/%.o : $(SRC_DIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(PLUGIN_DIR)/$(PLUGIN_NAME).so : $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS) $(CFLAGS) -shared

$(PLUGIN_DIR)/%.ttl : %.ttl
	cp $< $@


clean:
	rm -rf $(BUILD_DIR)

install:
	cp -r $(PLUGIN_DIR) ~/.lv2/
