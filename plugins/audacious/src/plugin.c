/*
 * Audacious bs2b effect plugin
 * Copyright (C) 2009, Sebastian Pipping <sebastian@pipping.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <audacious/plugin.h>
#include <bs2b.h>
#include <string.h>
#include <stdio.h>

#ifdef HAVE_CONFIG_H
# include <config.h>
#else
# define VERSION "?.?.?"
#endif

#define AB_LITTLE_ENDIAN 0
#define AB_BIG_ENDIAN 1

#ifdef WORDS_BIGENDIAN
# define SYSTEM_ENDIAN AB_BIG_ENDIAN
#else
# define SYSTEM_ENDIAN AB_LITTLE_ENDIAN
#endif


#define AB_SWAP_16(x) (\
	  ((x & 0x00ff) << 8)\
	| ((x & 0xff00) >> 8))

#define AB_SWAP_32(x) (\
	  (((x & 0x000000ff) >>  0) << 24)\
	| (((x & 0x0000ff00) >>  8) << 16)\
	| (((x & 0x00ff0000) >> 16) <<  8)\
	| (((x & 0xff000000) >> 24) <<  0))


static t_bs2bdp bs2b = NULL;


static void swap_16(void * pair) {
	unsigned short * p = (unsigned short *)pair;
	p[0] = AB_SWAP_16(p[0]);
	p[1] = AB_SWAP_16(p[1]);
}

static void swap_32(void * pair) {
	unsigned long * p = (unsigned long *)pair;
	p[0] = AB_SWAP_32(p[0]);
	p[1] = AB_SWAP_32(p[1]);
}


static void cleanup() {
	if (bs2b == NULL) {
		return;
	}
	bs2b_close(bs2b);
}


static gint mod_samples(gpointer * data, gint length, AFormat fmt, gint srate, gint nch) {
	if ((data == NULL) || (*data == NULL) || (nch != 2)) {
		return length;
	}

	switch (fmt) {
	#define ENABLE_CASE_C
	#include "case.c"
	#undef ENABLE_CASE_C
        default:
		;
	}
	return length;
}


static void init();


static EffectPlugin audaciousBs2b = {
	.description = "Bauer stereophonic-to-binaural " VERSION,
	.init = init,
	.cleanup = cleanup,
	.mod_samples = mod_samples
};


void init() {
	bs2b = bs2b_open();
	if (bs2b == NULL) {
		return;
	}
	bs2b_set_level(bs2b, BS2B_DEFAULT_CLEVEL);
}


static EffectPlugin * plugins[] = { &audaciousBs2b, NULL };
SIMPLE_EFFECT_PLUGIN(bs2b, plugins);
