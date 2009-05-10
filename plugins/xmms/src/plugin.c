/*
 * XMMS bs2b effect plugin
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

#include <plugin.h>
#include <bs2b.h>
#include <string.h>
#include <stdio.h>

#include <config.h>


static t_bs2bdp bs2b = NULL;


void init() {
    bs2b = bs2b_open();
    if (bs2b == NULL) {
        return;
    }
    bs2b_set_level(bs2b, XB_EFFECT_LEVEL);
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



int mod_samples(gpointer * data, gint length, AFormat fmt, gint srate, gint nch) {
	if ((data == NULL) || (*data == NULL) || (nch != 2)) {
		return length;
	}

	bs2b_set_srate(bs2b, srate);

	switch (fmt) {
	#define ENABLE_CASE_C
	#include "case.c"
	#undef ENABLE_CASE_C
        default:
		;
	}
	return length;
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
