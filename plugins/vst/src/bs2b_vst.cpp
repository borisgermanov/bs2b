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

#include "bs2b_effect.h"
#include "bs2b_editor.h"

AudioEffect *createEffectInstance( audioMasterCallback audioMaster )
{
	return new bs2b_effect( audioMaster );
}

//
bs2b_effect::bs2b_effect( audioMasterCallback audioMaster )
: AudioEffectX( audioMaster, 1, PARAM__MAX )  // 1 program
{
	setNumInputs( 2 );		// stereo in
	setNumOutputs( 2 );		// stereo out
	setUniqueID( 'bs2b' );	// identify
	canProcessReplacing();	// supports replacing output
	canDoubleReplacing();	// supports double precision processing

	// EDITOR
	editor = new bs2b_editor( this );

	vst_strncpy( m_programName, "Default", kVstMaxProgNameLen );
}

bs2b_effect::~bs2b_effect() {}

void bs2b_effect::setProgramName( char *name )
{
	vst_strncpy( m_programName, name, kVstMaxProgNameLen );
}

void bs2b_effect::getProgramName( char *name )
{
	vst_strncpy( name, m_programName, kVstMaxProgNameLen );
}

void bs2b_effect::setParameter( VstInt32 index, float value )
{
	int crossf = bs2b.get_crossf();
	int easy   = bs2b.get_easy();

	switch( index )
	{
	case PARAM_CROSSF:
		crossf = ( int )( value * ( BS2B_CLEVELS - 1 ) + 0.5f ) + 1;
		break;
	case PARAM_EASY:
		easy = value >= 0.5f;
		break;
	}

	bs2b.set_crossf_easy( crossf, easy );

	if( editor )
		( ( AEffGUIEditor * )editor )->setParameter( index, value );
}

float bs2b_effect::getParameter( VstInt32 index )
{
	switch( index )
	{
	case PARAM_CROSSF:
		return ( float )( bs2b.get_crossf() - 1 ) / ( float )( BS2B_CLEVELS - 1 );
	case PARAM_EASY:
		return ( float )( bs2b.get_easy() );
	default:
		return .0f;
	}
}

void bs2b_effect::getParameterName( VstInt32 index, char *label )
{
	switch( index )
	{
	case PARAM_CROSSF:
		vst_strncpy( label, "Feed", kVstMaxParamStrLen );
		break;
	case PARAM_EASY:
		vst_strncpy( label, "Easy", kVstMaxParamStrLen );
		break;
	}
}

void bs2b_effect::getParameterDisplay( VstInt32 index, char *text )
{
	switch( index )
	{
	case PARAM_CROSSF:
		switch( bs2b.get_crossf() )
		{
		case BS2B_HIGH_CLEVEL:
			vst_strncpy( text, "max", kVstMaxParamStrLen );
			break;
		case BS2B_MIDDLE_CLEVEL:
			vst_strncpy( text, "mid", kVstMaxParamStrLen );
			break;
		case BS2B_LOW_CLEVEL:
			vst_strncpy( text, "low", kVstMaxParamStrLen );
			break;
		}
		break;
	case PARAM_EASY:
		vst_strncpy( text, bs2b.get_easy() ? "on" : "off",
			kVstMaxParamStrLen );
		break;
	}
}

void bs2b_effect::getParameterLabel( VstInt32 index, char *label )
{
	// no label
	*label = '\0';
}

void bs2b_effect::setSampleRate( float sampleRate )
{
	bs2b.set_srate( ( long )sampleRate );
}

bool bs2b_effect::getEffectName( char *name )
{
	vst_strncpy( name, "bs2b", kVstMaxEffectNameLen );
	return true;
}

bool bs2b_effect::getProductString( char *text )
{
	vst_strncpy( text,
		"Bauer stereophonic-to-binaural DSP",
		kVstMaxProductStrLen );
	return true;
}

bool bs2b_effect::getVendorString( char *text )
{
	vst_strncpy( text, "bs2b.sourceforge.net", kVstMaxVendorStrLen );
	return true;
}

VstInt32 bs2b_effect::getVendorVersion()
{
	// Version 1.2.0
	return ( 1 << 16 ) | ( 2 << 8 ) | ( 0 );
}

void bs2b_effect::processReplacing( float **inputs, float **outputs,
								   VstInt32 sampleFrames )
{
	float sample[ 2 ];
	float *in1, *in2, *out1, *out2;

	in1  = inputs[ 0 ];
	in2  = inputs[ 1 ];
	out1 = outputs[ 0 ];
	out2 = outputs[ 1 ];

	while( sampleFrames-- )
	{
		sample[ 0 ] = *( in1++ );
		sample[ 1 ] = *( in2++ );

		bs2b.cross_feed( sample );

		*( out1++ ) = sample[ 0 ];
		*( out2++ ) = sample[ 1 ];
	}
}

void bs2b_effect::processDoubleReplacing( double **inputs, double **outputs,
										 VstInt32 sampleFrames )
{
	double sample[ 2 ];
	double *in1, *in2, *out1, *out2;

	in1  = inputs[ 0 ];
	in2  = inputs[ 1 ];
	out1 = outputs[ 0 ];
	out2 = outputs[ 1 ];

	while( sampleFrames-- )
	{
		sample[ 0 ] = *( in1++ );
		sample[ 1 ] = *( in2++ );

		bs2b.cross_feed( sample );

		*( out1++ ) = sample[ 0 ];
		*( out2++ ) = sample[ 1 ];
	}
}
