# Copyright (c) 2022, Marvin Borner <dev@marvinborner.de>
# SPDX-License-Identifier: MIT

CC = gcc
TG = ctags

BUILD = ${CURDIR}/build
SRC = ${CURDIR}/src
INC = ${CURDIR}/inc
SRCS = $(wildcard $(SRC)/*.c) $(wildcard $(SRC)/*/*.c)
OBJS = $(patsubst $(SRC)/%.c, $(BUILD)/%.o, $(SRCS))

CFLAGS_DEBUG = -Wno-error -g -O0 -Wno-unused -fsanitize=address,undefined,leak
CFLAGS_WARNINGS = -Wall -Wextra -Wshadow -Wpointer-arith -Wwrite-strings -Wredundant-decls -Wnested-externs -Wmissing-declarations -Wstrict-prototypes -Wmissing-prototypes -Wcast-qual -Wswitch-default -Wswitch-enum -Wunreachable-code -Wundef -Wold-style-definition -pedantic -Wno-switch-enum -Wframe-larger-than=512 -Wstack-usage=1024
CFLAGS = $(CFLAGS_WARNINGS) -std=c99 -Ofast -I$(INC) -lm

ifdef DEBUG # TODO: Somehow clean automagically
CFLAGS += $(CFLAGS_DEBUG)
endif

ifeq ($(PREFIX),)
    PREFIX := /usr/local
endif

all: compile

full: all sync

compile: $(BUILD) $(OBJS) $(BUILD)/sharing

clean:
	@rm -rf $(BUILD)/*

install:
	@install -m 755 $(BUILD)/sharing $(DESTDIR)$(PREFIX)/bin/

sync: # Ugly hack
	@$(MAKE) $(BUILD)/sharing --always-make --dry-run | grep -wE 'gcc|g\+\+' | grep -w '\-c' | jq -nR '[inputs|{directory:".", command:., file: match(" [^ ]+$$").string[1:]}]' >compile_commands.json
	@$(TG) -R --exclude=.git --exclude=build .

$(BUILD)/%.o: $(SRC)/%.c
	@mkdir -p $(@D)
	@$(CC) -c -o $@ $(CFLAGS) $<

$(BUILD)/sharing: $(OBJS)
	@$(CC) -o $@ $(CFLAGS) $^

.PHONY: all compile clean sync

$(BUILD):
	@mkdir -p $@
