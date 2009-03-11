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

/* foobar2000 SDK copyright by Peter Pawlowski.
 * http://www.foobar2000.org/
 */

#include <windows.h>

#include <foobar2000.h>
#include <helpers.h>
#include <bs2bclass.h>

#include "resource.h"

static char _bs2b_version[]   = "2.2.0";
static char _bs2b_name[]      = "bs2b";
static char _bs2b_full_name[] = "Bauer stereophonic-to-binaural DSP";
static char _bs2b_about[]     =
"The Bauer stereophonic-to-binaural DSP (bs2b) is designed\n"
"to improve headphone listening of stereo audio records.\n"
"Project source code and description is available at\n"
"http://bs2b.sourceforge.net/\n";

DECLARE_COMPONENT_VERSION( _bs2b_name, _bs2b_version, _bs2b_about );

struct t_dsp_bs2b_params
{
	t_int32 m_level;

	t_dsp_bs2b_params( t_int32 p_level = BS2B_DEFAULT_CLEVEL )
		: m_level( p_level ) {}

	static const GUID &g_get_guid()
	{
		// {0E2473DF-1170-471b-A609-C5D4A6D35AD4}
		static const GUID guid =
		{ 0xe2473df, 0x1170, 0x471b, { 0xa6, 0x9, 0xc5, 0xd4, 0xa6, 0xd3, 0x5a, 0xd4 } };
		return guid;
	}

	bool set_data( const dsp_preset &p_data )
	{
		if( p_data.get_owner() != g_get_guid() ) return false;
		if( p_data.get_data_size() != sizeof( t_int32 ) ) return false;
		t_int32 temp = *( t_int32 * )p_data.get_data();
		byte_order::order_le_to_native_t( temp );
		m_level = temp;
		return true;
	}

	void get_data( dsp_preset &p_data )
	{
		p_data.set_owner( g_get_guid() );
		t_int32 temp = m_level;
		byte_order::order_native_to_le_t( temp );
		p_data.set_data( &temp, sizeof( temp ) );
	}
};

class dialog_dsp_bs2b : public dialog_helper::dialog_modal
{
private:
	bool m_dirty;
	const dsp_preset &m_old_data;
	t_dsp_bs2b_params m_params;
	dsp_preset_edit_callback &m_callback;

	void update_display()
	{
		HWND wnd = get_wnd();
		LRESULT level = SendDlgItemMessage( wnd,
			IDC_SLIDER_LEVEL, TBM_GETPOS, 0, 0 );
		if( BST_CHECKED ==
			SendDlgItemMessage( wnd, IDC_CHECK_EASY, BM_GETCHECK, 0, 0 ) )
		{
			level += BS2B_CLEVELS;
		}

		m_params.m_level = level;
		if( m_dirty )
		{
			dsp_preset_impl data;
			m_params.get_data( data );
			m_callback.on_preset_changed( data );
			m_dirty = false;
		}
	} // update_display()

public:
	dialog_dsp_bs2b( const dsp_preset &p_data, dsp_preset_edit_callback &p_callback )
		: m_old_data( p_data ), m_callback( p_callback ), m_dirty( false )
	{
		m_params.set_data( m_old_data );
	}

	virtual BOOL on_message( UINT msg, WPARAM wp, LPARAM lp )
	{
		switch( msg )
		{
		case WM_INITDIALOG:
			{
				HWND wnd = get_wnd();
				HWND slider = GetDlgItem( wnd, IDC_SLIDER_LEVEL );
				SendMessage( slider, TBM_SETRANGE, TRUE,
					MAKELONG( 1, BS2B_CLEVELS ) );
				SendMessage( slider, TBM_SETPOS, TRUE,
					m_params.m_level > BS2B_CLEVELS ?
					m_params.m_level - BS2B_CLEVELS : m_params.m_level );
				SendDlgItemMessage( wnd, IDC_CHECK_EASY, BM_SETCHECK,
					m_params.m_level > BS2B_CLEVELS ?
					BST_CHECKED : BST_UNCHECKED, 0 );
				update_display();
			}
			break;

		case WM_HSCROLL:
			{
				m_dirty = true;
				update_display();
			}
			break;

		case WM_COMMAND:
			switch( wp )
			{
			case IDC_CHECK_EASY:
				{
					m_dirty = true;
					update_display();
				}
				break;

			case IDC_BUTTON_ABOUT:
				{
					uMessageBox( GetForegroundWindow(),
						pfc::string8() << _bs2b_name << " Version " <<
						_bs2b_version << "\n\n" << _bs2b_about,
						_bs2b_name, MB_OK );
				}
				break;

			case IDOK:
				{
					end_dialog( 1 );
				}
				break;

			case IDCANCEL:
				{
					m_callback.on_preset_changed( m_old_data );
					end_dialog( 0 );
				}
				break;
			} // switch
			break;
		} // switch
		
		return 0;
	} // on_message()
}; // class dialog_dsp_bs2b

class dsp_bs2b : public dsp_impl_base
{
private:
	bs2b_base bs2b;

public:
	dsp_bs2b( const dsp_preset &p_data)
	{
		set_data( p_data );
	}

	static GUID g_get_guid()
	{
		return t_dsp_bs2b_params::g_get_guid();
	}

	static void g_get_name( pfc::string_base &p_out )
	{
		p_out = _bs2b_full_name;
	}

	static bool g_have_config_popup()
	{
		return true;
	}

	static void g_show_config_popup( const dsp_preset &p_data, HWND p_parent,
		dsp_preset_edit_callback &p_callback )
	{
		t_dsp_bs2b_params params;
		if( params.set_data( p_data ) )
		{
			dialog_dsp_bs2b dlg( p_data, p_callback );
			dlg.run( IDD_CONFIG_BS2B, p_parent );
		}
	} // g_show_config_popup()

	static bool g_get_default_preset( dsp_preset &p_out )
	{
		t_dsp_bs2b_params().get_data( p_out );
		return true;
	} // g_get_default_preset()

	bool set_data( const dsp_preset &p_data )
	{
		t_dsp_bs2b_params params;
		if( ! params.set_data( p_data ) ) return false;
		bs2b.set_level( params.m_level );
		return true;
	}

	virtual void on_endoftrack( abort_callback &p_abort )
	{
		flush();
	}

	virtual void on_endofplayback( abort_callback &p_abort )
	{
		flush();
	}

	virtual bool on_chunk( audio_chunk *chunk, abort_callback &p_abort )
	{
		if( 2 == chunk->get_channels() ) // Do it only if stereo
		{
			int sample_count = chunk->get_sample_count();
			audio_sample *sample = chunk->get_data();
			bs2b.set_srate( chunk->get_srate() );
			bs2b.cross_feed( sample, sample_count );
		}

		return true;
	} // on_chunk()

	virtual void flush() {}

	virtual double get_latency()
	{
		return .0;
	}

	virtual bool need_track_change_mark()
	{
		return false;
	}
}; // class dsp_bs2b

static dsp_factory_t<dsp_bs2b> foo_dsp_bs2b;
