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

#include <stdio.h>

#include "bs2b_effect.h"
#include "bs2b_editor.h"

AudioEffect *createEffectInstance( audioMasterCallback audioMaster )
{
	return new bs2b_effect( audioMaster );
}

bs2b_effect::bs2b_effect( audioMasterCallback audioMaster )
: AudioEffectX( audioMaster, 1, PARAM__MAX ) // 1 program
{
	setNumInputs( 2 );     // stereo in
	setNumOutputs( 2 );    // stereo out
	setUniqueID( 'bs2b' ); // identify
	canProcessReplacing(); // supports replacing output
	canDoubleReplacing();  // supports double precision processing

	param_defs = 0;
	vst_strncpy( m_programName, "Default", kVstMaxProgNameLen );

	// EDITOR
	editor = new bs2b_editor( this );
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
	switch( index )
	{
	case PARAM_LEVEL_FEED:
		if( ! param_defs )
			bs2b.set_level_feed_norm( value );
		break;
	case PARAM_LEVEL_FCUT:
		if( ! param_defs )
			bs2b.set_level_fcut_norm( value );
		break;
	case PARAM_LEVEL_DEFS:
		param_defs = ( int )( value * PARAM_LEVEL_DEF__MAX - 0.01f );
		switch( param_defs )
		{
		case PARAM_LEVEL_DEF_DEFAULT:
			bs2b.set_level( BS2B_DEFAULT_CLEVEL );
			break;
		case PARAM_LEVEL_DEF_CMOY:
			bs2b.set_level( BS2B_CMOY_CLEVEL );
			break;
		case PARAM_LEVEL_DEF_JMEIER:
			bs2b.set_level( BS2B_JMEIER_CLEVEL );
			break;
		default:
			param_defs = 0;
			break;
		} // switch( param_defs )
		break;
	} // switch( index )

	if( editor )
		( ( AEffGUIEditor * )editor )->setParameter( index, value );
}

float bs2b_effect::getParameter( VstInt32 index )
{
	switch( index )
	{
	case PARAM_LEVEL_FEED:
		return bs2b.get_level_feed_norm();
	case PARAM_LEVEL_FCUT:
		return bs2b.get_level_fcut_norm();
	case PARAM_LEVEL_DEFS:
		return ( float )param_defs / ( PARAM_LEVEL_DEF__MAX - 1 );
	default:
		return 0.0f;
	} // switch( index )
}

void bs2b_effect::getParameterName( VstInt32 index, char *label )
{
	switch( index )
	{
	case PARAM_LEVEL_FEED:
		vst_strncpy( label, "Feed", kVstMaxParamStrLen );
		break;
	case PARAM_LEVEL_FCUT:
		vst_strncpy( label, "FCut", kVstMaxParamStrLen );
		break;
	case PARAM_LEVEL_DEFS:
		vst_strncpy( label, "Defs", kVstMaxParamStrLen );
		break;
	} // switch( index )
}

void bs2b_effect::getParameterDisplay( VstInt32 index, char *text )
{
	char string[ 64 ];

	switch( index )
	{
	case PARAM_LEVEL_FEED:
		{
			int feed = bs2b.get_level_feed();
			sprintf( string, "%d.%d", feed / 10, feed % 10 );
			vst_strncpy( text, string, kVstMaxParamStrLen );
		}
		break;
	case PARAM_LEVEL_FCUT:
		{
			sprintf( string, "%d/%d",
				bs2b.get_level_fcut(), bs2b.get_level_delay() );
			vst_strncpy( text, string, kVstMaxParamStrLen );
		}
		break;
	case PARAM_LEVEL_DEFS:
		{
			vst_strncpy( text, level_def_name[ param_defs ], kVstMaxParamStrLen );
		}
		break;
	} // switch( index )
}

void bs2b_effect::getParameterLabel( VstInt32 index, char *label )
{
	switch( index )
	{
	case PARAM_LEVEL_FEED:
		vst_strncpy( label, " dB", kVstMaxParamStrLen );
		break;
	case PARAM_LEVEL_FCUT:
		vst_strncpy( label, " Hz/us", kVstMaxParamStrLen );
		break;
	case PARAM_LEVEL_DEFS:
		vst_strncpy( label, " ", kVstMaxParamStrLen );
		break;
	} // switch( index )
}

void bs2b_effect::setSampleRate( float sampleRate )
{
	bs2b.set_srate( ( uint32_t )sampleRate );
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
	// Version 2.1.0
	return(
		( ( VstInt32 )2 << 16 ) |
		( ( VstInt32 )1 << 8 ) |
		( ( VstInt32 )0 ) );
}

void bs2b_effect::processReplacing( float **inputs, float **outputs,
								   VstInt32 sampleFrames )
{
	float sample[ 2 ];
	float *in1, *in2, *out1, *out2;

	if( sampleFrames <= 0 ) return;

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

	if( sampleFrames <= 0 ) return;

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
