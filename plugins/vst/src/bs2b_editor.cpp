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

enum
{
	// bitmaps
	kBackgroundId = 128,
	kFaderBodyId,
	kFaderHandleId,

	// positions
	kFaderX   = 10,
	kFaderY   = 30,
	kFaderInc = 50,

	kDisplayX = 10,
	kDisplayY = 15,
	kDisplayXWidth = 90,
	kDisplayHeight = 14
};

void feed_StringConvert( float value, char *string )
{
	int feed =
		( int )( value * ( BS2B_MAXFEED - BS2B_MINFEED ) ) + BS2B_MINFEED;

	sprintf( string, "%d.%d dB", feed / 10, feed % 10 );
}

void fcut_StringConvert( float value, char *string )
{
	int fcut =
		( int )( value * ( BS2B_MAXFCUT - BS2B_MINFCUT ) ) + BS2B_MINFCUT;

	sprintf( string, "%d Hz, %d us", fcut, bs2b_level_delay( fcut ) );
}

bs2b_editor::bs2b_editor( AudioEffect *effect )
: AEffGUIEditor( effect )
{
	feed_fader = 0;
	fcut_fader = 0;

	feed_display = 0;
	fcut_display = 0;

	hBackground = new CBitmap( kBackgroundId );

	rect.left   = 0;
	rect.top    = 0;
	rect.right  = ( short )hBackground->getWidth();
	rect.bottom = ( short )hBackground->getHeight();
}

bs2b_editor::~bs2b_editor()
{
	if( hBackground ) hBackground->forget();
	hBackground = 0;
}

bool bs2b_editor::open( void *ptr )
{
	AEffGUIEditor::open( ptr );
	
	CBitmap *hFaderBody   = new CBitmap( kFaderBodyId );
	CBitmap *hFaderHandle = new CBitmap( kFaderHandleId );

	CRect size( 0, 0, hBackground->getWidth(), hBackground->getHeight() );
	CFrame *lFrame = new CFrame( size, ptr, this );
	lFrame->setBackground( hBackground );

	int minPos = kFaderX;
	int maxPos = kFaderX + hFaderBody->getWidth() - hFaderHandle->getWidth();
	CPoint point( 0, 0 );
	CPoint offset( 0, 0 );

	// Feed level slider
	size( kFaderX, kFaderY,
		kFaderX + hFaderBody->getWidth(), kFaderY + hFaderBody->getHeight() );
	feed_fader = new CHorizontalSlider( size, this,
		PARAM_LEVEL_FEED, minPos, maxPos, hFaderHandle, hFaderBody, point, kLeft );
	feed_fader->setOffsetHandle( offset );
	feed_fader->setValue( effect->getParameter( PARAM_LEVEL_FEED ) );
	lFrame->addView( feed_fader );

	// Cut frequency slider
	size.offset( 0, kFaderInc + hFaderBody->getHeight() );
	fcut_fader = new CHorizontalSlider( size, this,
		PARAM_LEVEL_FCUT, minPos, maxPos, hFaderHandle, hFaderBody, point, kLeft );
	fcut_fader->setOffsetHandle( offset );
	fcut_fader->setValue( effect->getParameter( PARAM_LEVEL_FCUT ) );
	lFrame->addView( fcut_fader );

	// Feed level display
	size( kDisplayX, kDisplayY,
		kDisplayX + kDisplayXWidth, kDisplayY + kDisplayHeight );
	feed_display = new CParamDisplay( size, 0, kCenterText );
	feed_display->setFont( kNormalFontSmall );
	feed_display->setFontColor( kWhiteCColor );
	feed_display->setBackColor( kBlackCColor );
	feed_display->setFrameColor( kGreyCColor );
	feed_display->setValue( effect->getParameter( PARAM_LEVEL_FEED ) );
	feed_display->setStringConvert( feed_StringConvert );
	lFrame->addView( feed_display );

	// Cut frequency display
	size.offset( 0, kFaderInc + hFaderBody->getHeight() );
	fcut_display = new CParamDisplay( size, 0, kCenterText );
	fcut_display->setFont( kNormalFontSmall );
	fcut_display->setFontColor( kWhiteCColor );
	fcut_display->setBackColor( kBlackCColor );
	fcut_display->setFrameColor( kGreyCColor );
	fcut_display->setValue( effect->getParameter( PARAM_LEVEL_FCUT ) );
	fcut_display->setStringConvert( fcut_StringConvert );
	lFrame->addView( fcut_display );

	hFaderBody->forget();
	hFaderHandle->forget();

	frame = lFrame;

	return true;
}

void bs2b_editor::close()
{
	delete frame;
	frame = 0;
}

void bs2b_editor::setParameter( VstInt32 index, float value )
{
	if( frame == 0 ) return;

	switch( index )
	{
	case PARAM_LEVEL_FEED:
		if( feed_fader )
			feed_fader->setValue( effect->getParameter( index ) );
		if( feed_display )
			feed_display->setValue( effect->getParameter( index ) );
		break;

	case PARAM_LEVEL_FCUT:
		if( fcut_fader )
			fcut_fader->setValue( effect->getParameter( index ) );
		if( fcut_display )
			fcut_display->setValue( effect->getParameter( index ) );
		break;
	}
}

void bs2b_editor::valueChanged( CDrawContext *context, CControl *control )
{
	long tag = control->getTag();
	switch( tag )
	{
	case PARAM_LEVEL_FEED:
	case PARAM_LEVEL_FCUT:
		effect->setParameterAutomated( tag, control->getValue() );
		control->setDirty();
		break;
	}
}
