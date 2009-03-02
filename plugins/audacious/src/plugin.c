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


static t_bs2bdp bs2b = NULL;


static void cleanup() {
	if (bs2b == NULL) {
		return;
	}
	bs2b_close(bs2b);
}


#define CASE_BS2B(label, dataType, functionToCall, data, length) \
	case label: \
	{ \
		gint num = length / sizeof(dataType) / 2; \
		dataType * sample = (dataType *)*data; \
		while (num--) { \
			functionToCall(bs2b, sample); \
			sample += 2; \
		} \
	} \
	break;


static gint mod_samples(gpointer * data, gint length, AFormat fmt, gint srate, gint nch) {
	if ((data == NULL) || (*data == NULL) || (nch != 2)) {
		return length;
	}

	switch (fmt) {
	CASE_BS2B(FMT_S8,     char,          bs2b_cross_feed_s8,  data, length)
	CASE_BS2B(FMT_U8,     unsigned char, bs2b_cross_feed_u8,  data, length)
/*
	CASE_BS2B(FMT_U16_LE, unsigned short, bs2b_cross_feed_16,  data, length)
	CASE_BS2B(FMT_U16_BE, unsigned short, bs2b_cross_feed_16,  data, length)
	CASE_BS2B(FMT_U16_NE, unsigned short, bs2b_cross_feed_16,  data, length)
*/
	CASE_BS2B(FMT_S16_LE, short,         bs2b_cross_feed_16,  data, length)
	CASE_BS2B(FMT_S16_BE, short,         bs2b_cross_feed_16,  data, length)
	CASE_BS2B(FMT_S16_NE, short,         bs2b_cross_feed_16,  data, length)

/*
	CASE_BS2B(FMT_U24_LE, unsigned long, bs2b_cross_feed_24,  data, length)
	CASE_BS2B(FMT_U24_BE, unsigned long, bs2b_cross_feed_24,  data, length)
	CASE_BS2B(FMT_U24_NE, unsigned long, bs2b_cross_feed_24,  data, length)
*/
	CASE_BS2B(FMT_S24_LE, long,          bs2b_cross_feed_24,  data, length)
	CASE_BS2B(FMT_S24_BE, long,          bs2b_cross_feed_24,  data, length)
	CASE_BS2B(FMT_S24_NE, long,          bs2b_cross_feed_24,  data, length)

/*
	CASE_BS2B(FMT_U32_LE, unsigned long, bs2b_cross_feed_32,  data, length)
	CASE_BS2B(FMT_U32_BE, unsigned long, bs2b_cross_feed_32,  data, length)
	CASE_BS2B(FMT_U32_NE, unsigned long, bs2b_cross_feed_32,  data, length)
*/
	CASE_BS2B(FMT_S32_LE, long,          bs2b_cross_feed_32,  data, length)
	CASE_BS2B(FMT_S32_BE, long,          bs2b_cross_feed_32,  data, length)
	CASE_BS2B(FMT_S32_NE, long,          bs2b_cross_feed_32,  data, length)

	CASE_BS2B(FMT_FLOAT,  float,         bs2b_cross_feed_f32, data, length)
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
