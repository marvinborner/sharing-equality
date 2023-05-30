// Copyright (c) 2023, Marvin Borner <dev@marvinborner.de>
// SPDX-License-Identifier: MIT

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <parse.h>
#include <log.h>

static char *read_file(FILE *f)
{
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	char *string = malloc(fsize + 1);
	if (!string)
		fatal("out of memory!\n");
	int ret = fread(string, fsize, 1, f);

	if (ret != 1) {
		free(string);
		fatal("can't read file: %s\n", strerror(errno));
	}

	string[fsize] = 0;
	return string;
}

static char *read_path(const char *path)
{
	debug("reading from %s\n", path);
	FILE *f = fopen(path, "rb");
	if (!f)
		fatal("can't open file %s: %s\n", path, strerror(errno));
	char *string = read_file(f);
	fclose(f);
	return string;
}

int main(int argc, char *argv[])
{
	debug_enable(1);
	if (argc != 2)
		fatal("usage: %s <file>\n", argc ? argv[0] : "sharing");

	char *term = read_path(argv[1]);

	char *orig_term = term;
	struct term *parsed = parse_blc(&term);
	free(orig_term);

	term_print(parsed);
	fprintf(stderr, "\n");

	term_free(parsed);
	return 0;
}
