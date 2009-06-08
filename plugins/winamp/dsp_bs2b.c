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

#include <windows.h>
#include <commctrl.h>

#include <bs2b.h>

#include "dsp.h"
#include "resource.h"

#if BS2B_VERSION_MAJOR < 3
#error libbs2b version 3.0.0 or higher required.
#endif

static char _bs2b_version[]   = "3.1.0";
static char _bs2b_name[]      = "bs2b";
static char _bs2b_full_name[] = "Bauer stereophonic-to-binaural DSP";
static char _bs2b_about[]     =
"The Bauer stereophonic-to-binaural DSP (bs2b) is designed\n"
"to improve headphone listening of stereo audio records.\n"
"Project source code and description is available at\n"
"http://bs2b.sourceforge.net/\n";

static char _ininame_bs2b_level_feed[] = "bs2b_crossfeed_level_feed";
static char _ininame_bs2b_level_fcut[] = "bs2b_crossfeed_level_fcut";

static int _cfg_feed = ( BS2B_DEFAULT_CLEVEL & 0xffff0000 ) >> 16;
static int _dlg_feed = ( BS2B_DEFAULT_CLEVEL & 0xffff0000 ) >> 16;
static int _cfg_fcut = BS2B_DEFAULT_CLEVEL & 0xffff;
static int _dlg_fcut = BS2B_DEFAULT_CLEVEL & 0xffff;

static t_bs2bdp _bs2bdp = NULL;

/* module getter. */
static winampDSPModule *get_module( int which );

static void config( struct winampDSPModule *this_mod );
static int init( struct winampDSPModule *this_mod );
static void quit( struct winampDSPModule *this_mod );
static int modify_samples( struct winampDSPModule *this_mod,
						  void *samples, int numframes,
						  int bps, int nch, int srate );
static void config_getinifn( struct winampDSPModule *this_mod, char *ini_file );
static void config_read( struct winampDSPModule *this_mod );
static void config_write( struct winampDSPModule *this_mod );

/* Module header, includes version, description,
 * and address of the module retriever function
 */
static winampDSPHeader hdr =
{
	DSP_HDRVER,
	_bs2b_full_name,
	get_module
};

static winampDSPModule mod =
{
	_bs2b_name,
	NULL,	/* hwndParent */
	NULL,	/* hDllInstance */
	config,
	init,
	modify_samples,
	quit
};

static winampDSPModule *get_module( int which )
{
	if( which )
		return NULL;
	else
		return &mod;
} /* get_module() */

static BOOL CALLBACK ConfigProc( HWND wnd, UINT msg, WPARAM wp, LPARAM lp )
{
	char str[ 1024 ];

	switch( msg )
	{
	case WM_INITDIALOG:
		{
			HWND slider;

			slider = GetDlgItem( wnd, IDC_SLIDER_LEVEL_FEED );
			SendMessage( slider, TBM_SETRANGE, FALSE, MAKELONG( BS2B_MINFEED, BS2B_MAXFEED ) );
			SendMessage( slider, TBM_CLEARTICS, FALSE, 0 );
			SendMessage( slider, TBM_SETPOS, TRUE, _cfg_feed );
			wsprintf( str, "%d.%d dB", _cfg_feed / 10, _cfg_feed % 10 );
			SetDlgItemText( wnd, IDC_STATIC_LEVEL_FEED, str );

			slider = GetDlgItem( wnd, IDC_SLIDER_LEVEL_FCUT );
			SendMessage( slider, TBM_SETRANGE, FALSE, MAKELONG( BS2B_MINFCUT, BS2B_MAXFCUT ) );
			SendMessage( slider, TBM_CLEARTICS, FALSE, 0 );
			SendMessage( slider, TBM_SETPOS, TRUE, _cfg_fcut );
			wsprintf( str, "%d Hz, %d us", _cfg_fcut, bs2b_level_delay( _cfg_fcut ) );
			SetDlgItemText( wnd, IDC_STATIC_LEVEL_FCUT, str );
		}
		break;

	case WM_HSCROLL:
		{
			LRESULT val;

			val = SendDlgItemMessage( wnd, IDC_SLIDER_LEVEL_FEED, TBM_GETPOS, 0, 0 );
			wsprintf( str, "%d.%d dB", val / 10, val % 10 );
			SetDlgItemText( wnd, IDC_STATIC_LEVEL_FEED, str );

			val = SendDlgItemMessage( wnd, IDC_SLIDER_LEVEL_FCUT, TBM_GETPOS, 0, 0 );
			wsprintf( str, "%d Hz, %d us", val, bs2b_level_delay( val ) );
			SetDlgItemText( wnd, IDC_STATIC_LEVEL_FCUT, str );
		}
		break;

	case WM_CLOSE:
		EndDialog( wnd, IDCANCEL );
		break;

	case WM_COMMAND:
		switch( wp )
		{
		case IDC_BUTTON_DEFAULT:
		case IDC_BUTTON_CMOY:
		case IDC_BUTTON_JMEIER:
			{
				HWND slider;
				LRESULT val;
				uint32_t level;

				level = BS2B_DEFAULT_CLEVEL;
				if( wp == IDC_BUTTON_CMOY )   level = BS2B_CMOY_CLEVEL;
				if( wp == IDC_BUTTON_JMEIER ) level = BS2B_JMEIER_CLEVEL;

				val = level >> 16;
				slider = GetDlgItem( wnd, IDC_SLIDER_LEVEL_FEED );
				SendMessage( slider, TBM_SETPOS, TRUE, val );
				wsprintf( str, "%d.%d dB", val / 10, val % 10 );
				SetDlgItemText( wnd, IDC_STATIC_LEVEL_FEED, str );

				val = level & 0xffff;
				slider = GetDlgItem( wnd, IDC_SLIDER_LEVEL_FCUT );
				SendMessage( slider, TBM_SETPOS, TRUE, val );
				wsprintf( str, "%d Hz, %d us", val, bs2b_level_delay( val ) );
				SetDlgItemText( wnd, IDC_STATIC_LEVEL_FCUT, str );
			}
			break;

		case IDC_BUTTON_ABOUT:
			wsprintf( str, "%s Version %s\n\n%s",
				_bs2b_name, _bs2b_version, _bs2b_about );
			MessageBox( GetForegroundWindow(), str, _bs2b_name, MB_OK );
			break;

		case IDOK:
			_dlg_feed = SendDlgItemMessage( wnd, IDC_SLIDER_LEVEL_FEED, TBM_GETPOS, 0, 0 );
			_dlg_fcut = SendDlgItemMessage( wnd, IDC_SLIDER_LEVEL_FCUT, TBM_GETPOS, 0, 0 );
			EndDialog( wnd, IDOK );
			break;

		case IDCANCEL:
			EndDialog( wnd, IDCANCEL );
			break;
		} /* switch( wp ) */
		break;
	} /* switch( msg ) */

	return 0;
} /* ConfigProc() */

static void config( struct winampDSPModule *this_mod )
{
	if( IDOK == DialogBox( this_mod->hDllInstance,
		MAKEINTRESOURCE( IDD_CONFIG_BS2B ),
		GetForegroundWindow(), ConfigProc ) )
	{
		if( ( _cfg_feed != _dlg_feed ) ||
			( _cfg_fcut != _dlg_fcut ) )
		{
			_cfg_feed = _dlg_feed;
			_cfg_fcut = _dlg_fcut;

			bs2b_set_level_feed( _bs2bdp, _cfg_feed );
			bs2b_set_level_fcut( _bs2bdp, _cfg_fcut );

			config_write( this_mod );
		}
	}
} /* config() */

static int init( struct winampDSPModule *this_mod )
{
	_bs2bdp = bs2b_open();

	if( _bs2bdp == NULL ) return 1;

	config_read( this_mod );
	bs2b_set_level_feed( _bs2bdp, _cfg_feed );
	bs2b_set_level_fcut( _bs2bdp, _cfg_fcut );

	return 0;
} /* init() */

static void quit( struct winampDSPModule *this_mod )
{
	bs2b_close( _bs2bdp );
	_bs2bdp = NULL;
}

static int modify_samples( struct winampDSPModule *this_mod,
						  void *samples, int numsamples,
						  int bps, int nch, int srate )
{
	if( nch != 2 )
		return numsamples;

	bs2b_set_srate( _bs2bdp, srate );
	
	switch( bps )
	{
	case 32:
		bs2b_cross_feed_s32( _bs2bdp, samples, numsamples );
		break;

	case 16:
		bs2b_cross_feed_s16( _bs2bdp, samples, numsamples );
		break;
		
	case 8:
		/* Apollo uses signed 8bit */
		/* bs2b_cross_feed_s8( _bs2bdp, samples, numsamples ); */
		bs2b_cross_feed_u8( _bs2bdp, samples, numsamples );
		break;
		
	case 24:
		bs2b_cross_feed_s24( _bs2bdp, samples, numsamples );
		break;
	
	default:
		break;
	} /* switch( bps ) */

	return numsamples;
} /* modify_samples() */

/* makes a .ini file in the winamp directory named "plugin.ini" */
static void config_getinifn( struct winampDSPModule *this_mod, char *ini_file )
{
	char *p;

	GetModuleFileName( this_mod->hDllInstance, ini_file, MAX_PATH );
	p = ini_file + lstrlen( ini_file );
	while( p >= ini_file && *p != '\\' )
		p--;
	if( ++p >= ini_file )
		*p = 0;
	lstrcat( ini_file, "plugin.ini" );
} /* config_getinifn() */

static void config_read( struct winampDSPModule *this_mod )
{
	char ini_file[ MAX_PATH ];

	config_getinifn( this_mod, ini_file );

	_cfg_feed =
		GetPrivateProfileInt( this_mod->description,
		_ininame_bs2b_level_feed, _cfg_feed, ini_file );

	_cfg_fcut =
		GetPrivateProfileInt( this_mod->description,
		_ininame_bs2b_level_fcut, _cfg_fcut, ini_file );
} /* config_read() */

static void config_write( struct winampDSPModule *this_mod )
{
	char string[ 32 ];
	char ini_file[ MAX_PATH ];

	config_getinifn( this_mod, ini_file );

	wsprintf( string, "%d", _cfg_feed );
	WritePrivateProfileString( this_mod->description,
		_ininame_bs2b_level_feed, string, ini_file );

	wsprintf( string, "%d", _cfg_fcut );
	WritePrivateProfileString( this_mod->description,
		_ininame_bs2b_level_fcut, string, ini_file );
} /* config_write() */

#ifdef __cplusplus
extern "C" {
#endif

__declspec( dllexport ) winampDSPHeader *winampDSPGetHeader2()
{
	return &hdr;
}

#ifdef __cplusplus
}
#endif
