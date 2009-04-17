/*-
 * Copyright (c) 2007 Boris Mikhaylov
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

#ifndef BS2B_EFFECT_H
#define BS2B_EFFECT_H

#include <audioeffectx.h>
#include <bs2bclass.h>

#if BS2B_VERSION_MAJOR < 3
#error libbs2b version 3.0.0 or higher required.
#endif

// Parameters
enum
{
	PARAM_LEVEL_FEED,
	PARAM_LEVEL_FCUT,
	PARAM_LEVEL_DEFS,
	PARAM__MAX
};

// Switch of default sets
enum
{
	PARAM_LEVEL_DEF_OFF,
	PARAM_LEVEL_DEF_DEFAULT,
	PARAM_LEVEL_DEF_CMOY,
	PARAM_LEVEL_DEF_JMEIER,
	PARAM_LEVEL_DEF__MAX
};

static char const *level_def_name[] = {
	"Off",
	"Def",
	"C.Moy",
	"J.Meier"
};

class bs2b : public bs2b_base
{
public:
	float get_level_feed_norm()
	{
		return(
			( float )( get_level_feed() - BS2B_MINFEED ) /
			( float )( BS2B_MAXFEED - BS2B_MINFEED ) );
	}

	void set_level_feed_norm( float feed_f )
	{
		set_level_feed(
			( int )( feed_f * ( BS2B_MAXFEED - BS2B_MINFEED ) ) +
			BS2B_MINFEED );
	}

	float get_level_fcut_norm()
	{
		return(
			( float )( get_level_fcut() - BS2B_MINFCUT ) /
			( float )( BS2B_MAXFCUT - BS2B_MINFCUT ) );
	}

	void set_level_fcut_norm( float fcut_f )
	{
		set_level_fcut(
			( int )( fcut_f * ( BS2B_MAXFCUT - BS2B_MINFCUT ) ) +
			BS2B_MINFCUT );
	}
};

class bs2b_effect : public AudioEffectX
{
private:
	bs2b bs2b;

public:
	bs2b_effect( audioMasterCallback audioMaster );
	~bs2b_effect();

	// Processing
	virtual void processReplacing(
		float **inputs, float **outputs,
		VstInt32 sampleFrames );
	virtual void processDoubleReplacing(
		double **inputs, double **outputs,
		VstInt32 sampleFrames );

	// Program
	virtual void setProgramName( char *name );
	virtual void getProgramName( char *name );

	// Parameters
	virtual void setParameter( VstInt32 index, float value );
	virtual float getParameter( VstInt32 index );
	virtual void getParameterLabel( VstInt32 index, char *label );
	virtual void getParameterDisplay( VstInt32 index, char *text );
	virtual void getParameterName( VstInt32 index, char *text );

	virtual void setSampleRate( float sampleRate );

	virtual bool getEffectName( char *name );
	virtual bool getVendorString( char *text );
	virtual bool getProductString( char *text );
	virtual VstInt32 getVendorVersion();

protected:
	int param_defs;
	char m_programName[ kVstMaxProgNameLen + 1 ];
};

#endif // BS2B_EFFECT_H
