/*
 * bs2b XMMS effect plugin
 * Copyright (C) 2007, Sebastian Pipping <sebastian@pipping.org>
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

#ifdef HAVE_CONFIG_H
# include <config.h>
#else
# define VERSION "?.?.?"
#endif



#include <plugin.h>
#include <bs2b.h>
#include <string.h>
#include <stdio.h>



t_bs2bdp bs2b = NULL;



void init() {
    bs2b = bs2b_open();
    if (bs2b == NULL) {
        return;
    }
    bs2b_set_level(bs2b, BS2B_DEFAULT_CLEVEL);
}



void cleanup() {
    if (bs2b == NULL) {
        return;
    }
    bs2b_close(bs2b);
}



void nothing() {
    /* NOOP */
}



int mod_samples(gpointer * d, gint length, AFormat afmt, gint srate, gint nch) {
    if (bs2b == NULL) {
        return length;
    }

    /* Stereo only */
    if (nch != 2) {
        return length;
    }

	bs2b_set_srate(bs2b, srate);

    switch (afmt) {
    case FMT_U8:
		{
        	gint num = length / sizeof(unsigned char) / 2;
			unsigned char * sptr = *d;

			while (num--) {
				bs2b_cross_feed_u8(bs2b, sptr);
				sptr += 2;
			}
		}
        return length;

    case FMT_S8:
		{
        	gint num = length / sizeof(char) / 2;
			char * sptr = *d;

			while (num--) {
				bs2b_cross_feed_s8(bs2b, sptr);
				sptr += 2;
			}
		}
        return length;

    case FMT_S16_LE:
		{
        	gint num = length / sizeof(short) / 2;
			short * sptr = *d;

			while (num--) {
				bs2b_cross_feed_16(bs2b, sptr);
				sptr += 2;
			}
		}
        return length;

    /* TODO */
    case FMT_S16_BE:
    case FMT_S16_NE:
    case FMT_U16_LE:
    case FMT_U16_BE:
    case FMT_U16_NE:
    default:
        return length;

    }
}



void query_format(AFormat * fmt, gint * rate, gint * nch) {
	*fmt = FMT_S16_LE;
	*rate = BS2B_DEFAULT_SRATE;
	*nch = 2;
}



EffectPlugin xmmsBs2b = {
	NULL, /* handle */
	NULL, /* filename */
	NULL, /* description */
	init, /* init */
	cleanup, /* cleanup */
	nothing, /* about */
	nothing, /* configure */
	mod_samples,
	query_format
};



EffectPlugin * get_eplugin_info() {
	const char * const version_format = "Bauer stereophonic-to-binaural "
			VERSION " (%s)";
	const int format_len = strlen(version_format);
	const int bs2b_version_len = strlen(bs2b_runtime_version());
    const int final_len = format_len - 2 + bs2b_version_len;

	if (xmmsBs2b.description != NULL) {
		free(xmmsBs2b.description);
	}
	xmmsBs2b.description = malloc(final_len + 1);
	if (xmmsBs2b.description != NULL) {
		snprintf(xmmsBs2b.description, final_len + 1, version_format,
				bs2b_runtime_version());
		xmmsBs2b.description[final_len] = '\0';
	}

	return &xmmsBs2b;
}
