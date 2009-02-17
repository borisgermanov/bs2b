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

enum
{
	// bitmaps
	kBackgroundId = 128,
	kFaderBodyId,
	kFaderHandleId,

	// positions
	kFaderX   = 10,
	kFaderY   = 30,
	kFaderInc = 50
};

bs2b_editor::bs2b_editor( AudioEffect *effect )
: AEffGUIEditor( effect )
{
	crossf_fader   = 0;
	easy_fader     = 0;

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

	// Cross Feed
	size( kFaderX, kFaderY,
		kFaderX + hFaderBody->getWidth(), kFaderY + hFaderBody->getHeight() );
	crossf_fader = new CHorizontalSlider( size, this,
		PARAM_CROSSF, minPos, maxPos, hFaderHandle, hFaderBody, point, kLeft );
	crossf_fader->setOffsetHandle( offset );
	crossf_fader->setValue( effect->getParameter( PARAM_CROSSF ) );
	lFrame->addView( crossf_fader );

	// Easy
	size.offset( 0, kFaderInc + hFaderBody->getHeight() );
	easy_fader = new CHorizontalSlider( size, this,
		PARAM_EASY, minPos, maxPos, hFaderHandle, hFaderBody, point, kLeft );
	easy_fader->setOffsetHandle( offset );
	easy_fader->setValue( effect->getParameter( PARAM_EASY ) );
	lFrame->addView( easy_fader );

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
	case PARAM_CROSSF:
		if( crossf_fader )
			crossf_fader->setValue( effect->getParameter( index ) );
		break;

	case PARAM_EASY:
		if( easy_fader )
			easy_fader->setValue( effect->getParameter( index ) );
		break;
	}
}

void bs2b_editor::valueChanged( CDrawContext *context, CControl *control )
{
	long tag = control->getTag();
	switch( tag )
	{
	case PARAM_CROSSF:
	case PARAM_EASY:
		effect->setParameterAutomated( tag, control->getValue() );
		control->setDirty();
		break;
	}
}
