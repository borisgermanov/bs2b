/*-
 * Copyright (c) 2005 Boris Mikhaylov
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef BS2B_H
#define BS2B_H

#include "bs2bversion.h"
#include "bs2btypes.h"

/* Number of crossfeed levels */
#define BS2B_CLEVELS           3

/* Normal crossfeed levels */
#define BS2B_HIGH_CLEVEL       3
#define BS2B_MIDDLE_CLEVEL     2
#define BS2B_LOW_CLEVEL        1

/* Easy crossfeed levels */
#define BS2B_HIGH_ECLEVEL      BS2B_HIGH_CLEVEL    + BS2B_CLEVELS
#define BS2B_MIDDLE_ECLEVEL    BS2B_MIDDLE_CLEVEL  + BS2B_CLEVELS
#define BS2B_LOW_ECLEVEL       BS2B_LOW_CLEVEL     + BS2B_CLEVELS

/* Default crossfeed levels */
#define BS2B_DEFAULT_CLEVEL    BS2B_HIGH_ECLEVEL
/* Default sample rate (Hz) */
#define BS2B_DEFAULT_SRATE     44100

typedef struct
{
	uint32_t level;              /* Crossfeed level */
	uint32_t srate;              /* Sample rate (Hz) */
	double a0_lo, b1_lo;         /* Lowpass IIR filter coefficients */
	double a0_hi, a1_hi, b1_hi;  /* Highboost IIR filter coefficients */
	double gain;                 /* Global gain against overloading */
	/* Buffer of last filtered sample: [0] 1-st channel, [1] 2-d channel */
	struct { double asis[ 2 ], lo[ 2 ], hi[ 2 ]; } lfs;
} t_bs2bd;

typedef t_bs2bd *t_bs2bdp;

#ifdef __cplusplus
extern "C"
{
#endif	/* __cplusplus */

/* Allocates and sets a data to defaults.
 * Return NULL on error.
 */
t_bs2bdp bs2b_open( void );

/* Close */
void bs2b_close( t_bs2bdp bs2bdp );

/* Sets a new coefficients with new crossfeed level value.
 * level - crossfeed level of *LEVEL values.
 */
void bs2b_set_level( t_bs2bdp bs2bdp, uint32_t level );

/* Return current crossfeed level value */
uint32_t bs2b_get_level( t_bs2bdp bs2bdp );

/* Clear buffers and sets a new coefficients with new sample rate value.
 * srate - sample rate by Hz.
 */
void bs2b_set_srate( t_bs2bdp bs2bdp, uint32_t srate );

/* Return current sample rate value */
uint32_t bs2b_get_srate( t_bs2bdp bs2bdp );

/* Clear buffer */
void bs2b_clear( t_bs2bdp bs2bdp );

/* Return 1 if buffer is clear */
int bs2b_is_clear( t_bs2bdp bs2bdp );

/* Return bs2b version string */
char const *bs2b_runtime_version( void );

/* Return bs2b version integer */
uint32_t bs2b_runtime_version_int( void );

/* 'bs2b_cross_feed_*' crossfeeds buffer of 'n' stereo samples
 * pointed by 'sample'.
 * sample[i]   - first channel,
 * sample[i+1] - second channel.
 * Where 'i' is ( i = 0; i < n * 2; i += 2 )
 */

/* sample poits to double floats native endians */
void bs2b_cross_feed_d( t_bs2bdp bs2bdp, double *sample, int n );

/* sample poits to double floats big endians */
void bs2b_cross_feed_dbe( t_bs2bdp bs2bdp, double *sample, int n );

/* sample poits to double floats little endians */
void bs2b_cross_feed_dle( t_bs2bdp bs2bdp, double *sample, int n );

/* sample poits to floats native endians */
void bs2b_cross_feed_f( t_bs2bdp bs2bdp, float *sample, int n );

/* sample poits to floats big endians */
void bs2b_cross_feed_fbe( t_bs2bdp bs2bdp, float *sample, int n );

/* sample poits to floats little endians */
void bs2b_cross_feed_fle( t_bs2bdp bs2bdp, float *sample, int n );

/* sample poits to 32bit signed integers native endians */
void bs2b_cross_feed_s32( t_bs2bdp bs2bdp, int32_t *sample, int n );

/* sample poits to 32bit unsigned integers native endians */
void bs2b_cross_feed_u32( t_bs2bdp bs2bdp, uint32_t *sample, int n );

/* sample poits to 32bit signed integers big endians */
void bs2b_cross_feed_s32be( t_bs2bdp bs2bdp, int32_t *sample, int n );

/* sample poits to 32bit unsigned integers big endians */
void bs2b_cross_feed_u32be( t_bs2bdp bs2bdp, uint32_t *sample, int n );

/* sample poits to 32bit signed integers little endians */
void bs2b_cross_feed_s32le( t_bs2bdp bs2bdp, int32_t *sample, int n );

/* sample poits to 32bit unsigned integers little endians */
void bs2b_cross_feed_u32le( t_bs2bdp bs2bdp, uint32_t *sample, int n );

/* sample poits to 16bit signed integers native endians */
void bs2b_cross_feed_s16( t_bs2bdp bs2bdp, int16_t *sample, int n );

/* sample poits to 16bit unsigned integers native endians */
void bs2b_cross_feed_u16( t_bs2bdp bs2bdp, uint16_t *sample, int n );

/* sample poits to 16bit signed integers big endians */
void bs2b_cross_feed_s16be( t_bs2bdp bs2bdp, int16_t *sample, int n );

/* sample poits to 16bit unsigned integers big endians */
void bs2b_cross_feed_u16be( t_bs2bdp bs2bdp, uint16_t *sample, int n );

/* sample poits to 16bit signed integers little endians */
void bs2b_cross_feed_s16le( t_bs2bdp bs2bdp, int16_t *sample, int n );

/* sample poits to 16bit unsigned integers little endians */
void bs2b_cross_feed_u16le( t_bs2bdp bs2bdp, uint16_t *sample, int n );

/* sample poits to 8bit signed integers */
void bs2b_cross_feed_s8( t_bs2bdp bs2bdp, int8_t *sample, int n );

/* sample poits to 8bit unsigned integers */
void bs2b_cross_feed_u8( t_bs2bdp bs2bdp, uint8_t *sample, int n );

/* sample poits to 24bit signed integers native endians */
void bs2b_cross_feed_s24( t_bs2bdp bs2bdp, bs2b_int24_t *sample, int n );

/* sample poits to 24bit unsigned integers native endians */
void bs2b_cross_feed_u24( t_bs2bdp bs2bdp, bs2b_uint24_t *sample, int n );

/* sample poits to 24bit signed integers be endians */
void bs2b_cross_feed_s24be( t_bs2bdp bs2bdp, bs2b_int24_t *sample, int n );

/* sample poits to 24bit unsigned integers be endians */
void bs2b_cross_feed_u24be( t_bs2bdp bs2bdp, bs2b_uint24_t *sample, int n );

/* sample poits to 24bit signed integers little endians */
void bs2b_cross_feed_s24le( t_bs2bdp bs2bdp, bs2b_int24_t *sample, int n );

/* sample poits to 24bit unsigned integers little endians */
void bs2b_cross_feed_u24le( t_bs2bdp bs2bdp, bs2b_uint24_t *sample, int n );

#ifdef __cplusplus
}	/* extern "C" */
#endif /* __cplusplus */

#endif	/* BS2B_H */
