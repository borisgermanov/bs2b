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

#include <config.h>


static t_bs2bdp bs2b = NULL;
static gint bs2b_srate = BS2B_DEFAULT_SRATE;


static void cleanup() {
	if (bs2b == NULL) {
		return;
	}
	bs2b_close(bs2b);
	bs2b = NULL;
}


static gint mod_samples(gpointer * data, gint length, AFormat fmt, gint srate, gint nch) {
	if ((data == NULL) || (*data == NULL) || (nch != 2)) {
		return length;
	}

	if (srate != bs2b_srate) {
		bs2b_set_srate(bs2b, srate);
		bs2b_srate = srate;
	}

	switch (fmt) {
	#define ENABLE_CASE_C
	#include "case.c.inc"
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
	bs2b_set_level(bs2b, AB_EFFECT_LEVEL);
}


static EffectPlugin * plugins[] = { &audaciousBs2b, NULL };
SIMPLE_EFFECT_PLUGIN(audaciousBs2b, plugins);
