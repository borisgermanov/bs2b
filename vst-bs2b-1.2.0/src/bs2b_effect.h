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

// Parameters
enum
{
	PARAM_CROSSF,
	PARAM_EASY,
	PARAM__MAX
};

class bs2b : public bs2b_base
{
public:
	inline int get_crossf()
	{
		return ( ( get_level() - 1 ) % BS2B_CLEVELS ) + 1;
	}

	inline int get_easy()
	{
		return get_level() > BS2B_CLEVELS;
	}

	inline void set_crossf_easy( int crossf, int easy )
	{
		set_level( crossf + easy * BS2B_CLEVELS );
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
	char m_programName[ kVstMaxProgNameLen + 1 ];
};

#endif // BS2B_EFFECT_H
