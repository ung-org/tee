/*
 * UNG's Not GNU
 *
 * Copyright (c) 2011-2019, Jakob Kaivo <jkk@ung.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#define _POSIX_C_SOURCE 2
#include <errno.h>
#include <locale.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#if FOPEN_MAX < 13
#undef FOPEN_MAX
#define FOPEN_MAX 13
#endif

static FILE *outputs[FOPEN_MAX];
static int noutputs = 0;

static int addoutput(const char *path, const char *mode)
{
	outputs[noutputs] = fopen(path, mode);
	if (outputs[noutputs] == NULL) {
		fprintf(stderr, "tee: %s: %s\n", path, strerror(errno));
		return 1;
	}

	noutputs++;
	return 0;
}

int main(int argc, char *argv[])
{
	setlocale(LC_ALL, "");
	char *mode = "w";

	int c;
	while ((c = getopt(argc, argv, "ai")) != -1) {
		switch (c) {
		case 'a':
			mode = "a";
			break;

		case 'i':
			signal(SIGINT, SIG_IGN);
			break;

		default:
			return 1;
		}
	}

	if (argc - optind > FOPEN_MAX) {
		fprintf(stderr, "tee: too many outputs (max %d)\n", FOPEN_MAX);
		return 1;
	}

	int r = 0;
	while (optind < argc) {
		r |= addoutput(argv[optind++], mode);
	}

	while ((c = getchar()) != EOF) {
		fputc(c, stdout);
		for (int i = 0; i < noutputs; i++) {
			fputc(c, outputs[i]);
		}
	}

	return r;
}
