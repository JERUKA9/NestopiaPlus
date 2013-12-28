////////////////////////////////////////////////////////////////////////////////////////
//
// Nestopia - NES/Famicom emulator written in C++
//
// Copyright (C) 2003-2006 Martin Freij
//
// This file is part of Nestopia.
// 
// Nestopia is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// Nestopia is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Nestopia; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
////////////////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include "NstApplicationConfiguration.hpp"
#include "NstResourceString.hpp"
#include "NstWindowParam.hpp"
#include "NstManagerPaths.hpp"
#include "NstManagerEmulator.hpp"
#include "NstDialogSound.hpp"
#include "../core/api/NstApiSound.hpp"

namespace Nestopia
{
	using namespace Window;

	const Sound::ChannelLut Sound::channelLut[NUM_CHANNELS] =
	{
		{ "sound master volume",       IDC_SOUND_MASTER_SLIDER,   IDC_SOUND_MASTER_VALUE,   0                            },
		{ "sound apu square 1 volume", IDC_SOUND_SQUARE1_SLIDER,  IDC_SOUND_SQUARE1_VALUE,  Nes::Sound::CHANNEL_SQUARE1  },
		{ "sound apu square 2 volume", IDC_SOUND_SQUARE2_SLIDER,  IDC_SOUND_SQUARE2_VALUE,  Nes::Sound::CHANNEL_SQUARE2  },
		{ "sound apu triangle volume", IDC_SOUND_TRIANGLE_SLIDER, IDC_SOUND_TRIANGLE_VALUE, Nes::Sound::CHANNEL_TRIANGLE },
		{ "sound apu noise volume",	   IDC_SOUND_NOISE_SLIDER,    IDC_SOUND_NOISE_VALUE,    Nes::Sound::CHANNEL_NOISE    },
		{ "sound apu dpcm volume",	   IDC_SOUND_DPCM_SLIDER,     IDC_SOUND_DPCM_VALUE,     Nes::Sound::CHANNEL_DPCM     },
		{ "sound fds volume",	       IDC_SOUND_FDS_SLIDER,      IDC_SOUND_FDS_VALUE,      Nes::Sound::CHANNEL_FDS      },
		{ "sound mmc5 volume",	       IDC_SOUND_MMC5_SLIDER,     IDC_SOUND_MMC5_VALUE,     Nes::Sound::CHANNEL_MMC5     },
		{ "sound vrc6 volume",	       IDC_SOUND_VRC6_SLIDER,     IDC_SOUND_VRC6_VALUE,     Nes::Sound::CHANNEL_VRC6     },
		{ "sound vrc7 volume",	       IDC_SOUND_VRC7_SLIDER,     IDC_SOUND_VRC7_VALUE,     Nes::Sound::CHANNEL_VRC7     },
		{ "sound n106 volume",	       IDC_SOUND_N106_SLIDER,     IDC_SOUND_N106_VALUE,     Nes::Sound::CHANNEL_N106     },
		{ "sound s5b volume",	       IDC_SOUND_S5B_SLIDER,      IDC_SOUND_S5B_VALUE,      Nes::Sound::CHANNEL_S5B      }
	};

	struct Sound::Handlers
	{
		static const MsgHandler::Entry<Sound> messages[];
		static const MsgHandler::Entry<Sound> commands[];
	};

	const MsgHandler::Entry<Sound> Sound::Handlers::messages[] =
	{
		{ WM_INITDIALOG, &Sound::OnInitDialog },
		{ WM_VSCROLL,    &Sound::OnVScroll    },
		{ WM_DESTROY,    &Sound::OnDestroy    }
	};

	const MsgHandler::Entry<Sound> Sound::Handlers::commands[] =
	{
		{ IDC_SOUND_DEVICE,        &Sound::OnCmdDevice       },
		{ IDC_SOUND_DEFAULT,       &Sound::OnCmdDefault      },
		{ IDC_SOUND_8_BIT,         &Sound::OnCmdBits         },
		{ IDC_SOUND_16_BIT,        &Sound::OnCmdBits         },
		{ IDC_SOUND_MONO,          &Sound::OnCmdOutput       },
		{ IDC_SOUND_STEREO,        &Sound::OnCmdOutput       },
		{ IDC_SOUND_RESET_SLIDERS, &Sound::OnCmdResetSliders },
		{ IDC_SOUND_OK,            &Sound::OnCmdOk           }
	};

	Sound::Sound(Managers::Emulator& e,const Adapters& a,const Managers::Paths& paths,const Configuration& cfg)
	: 
	adapters ( a ),
	nes      ( e ),
	dialog   ( IDD_SOUND, this, Handlers::messages, Handlers::commands ),
	recorder ( paths )
	{
		settings.adapter = UINT_MAX;

		if (adapters.size())
		{
			const GenericString device( cfg["sound device"] );

			if (device != _T("none"))
			{
				const Adapters::const_iterator it( std::find( adapters.begin(), adapters.end(), System::Guid(device) ));
			
				if (it != adapters.end())
					settings.adapter = it - adapters.begin();
				else
					settings.adapter = GetDefaultAdapter();
			}
		}

		switch (uint rate = cfg["sound sample rate"])
		{
			case 11025U:
			case 22050U:
			case 44100U:
			case 48000U:
			case 88200U:
			case 96000U:
	
				nes.SetSampleRate( rate );
		    	break;
		}
  
		switch (uint bits = cfg["sound sample bits"])
		{
			case 8:
			case 16:
	
				nes.SetSampleBits( bits );
				break;
		}
  
		nes.SetAutoTranspose( cfg["sound adjust pitch"] == Configuration::YES );
		nes.SetSpeaker( GenericString(cfg["sound output"]) == _T("stereo") ? Nes::Sound::SPEAKER_STEREO : Nes::Sound::SPEAKER_MONO );

		for (uint i=0; i < NUM_CHANNELS; ++i)
		{
			uint volume = cfg[channelLut[i].cfg].Default( (uint) DEFAULT_VOLUME );

			if (volume > VOLUME_MAX)
				volume = DEFAULT_VOLUME;

			settings.volumes[i] = volume;

			if (channelLut[i].channel)
				nes.SetVolume( channelLut[i].channel, GetVolume( channelLut[i].channel ) );
		}

		settings.latency = cfg[ "sound buffers" ].Default( (uint) DEFAULT_LATENCY );

		if (settings.latency > LATENCY_MAX)
			settings.latency = DEFAULT_LATENCY;
	}

	Sound::~Sound()
	{
	}

	void Sound::Save(Configuration& cfg) const
	{
		if (settings.adapter < adapters.size())
			cfg[ "sound device" ].Quote() = adapters[settings.adapter].guid.GetString();
		else
			cfg[ "sound device" ] = _T("none");

		cfg[ "sound sample rate"  ] = nes.GetSampleRate();
		cfg[ "sound sample bits"  ] = nes.GetSampleBits();
		cfg[ "sound buffers"      ] = settings.latency;
		cfg[ "sound output"       ] = (nes.GetSpeaker() == Nes::Sound::SPEAKER_STEREO ? _T("stereo") : _T("mono"));
		cfg[ "sound adjust pitch" ].YesNo() = nes.IsAutoTransposing();

		for (uint i=0; i < NUM_CHANNELS; ++i)
			cfg[channelLut[i].cfg] = (uint) settings.volumes[i];
	}

	uint Sound::GetVolume(const uint channel) const
	{
		if (settings.adapter != UINT_MAX)
		{
			for (uint i=1; i < NUM_CHANNELS; ++i)
			{
				if (channelLut[i].channel == channel)
				{
					const uint volume = (settings.volumes[i] * settings.volumes[0] + DEFAULT_VOLUME/2) / DEFAULT_VOLUME;
					return NST_MIN(volume,VOLUME_MAX);
				}
			}
		}

		return 0;
	}

	uint Sound::GetDefaultAdapter() const
	{
		const Adapters::const_iterator it( std::find( adapters.begin(), adapters.end(), System::Guid() ) );
		return it != adapters.end() ? it - adapters.begin() : 0;
	}

	ibool Sound::OnInitDialog(Param&)
	{
		if (adapters.size())
		{
			{
				const Control::ComboBox comboBox( dialog.ComboBox(IDC_SOUND_DEVICE) );

				comboBox.Add( Resource::String(IDS_NONE) );

				for (Adapters::const_iterator it(adapters.begin()); it != adapters.end(); ++it)
					comboBox.Add( it->name.Ptr() );

				comboBox[settings.adapter+1U].Select();
			}

			{
				static const tchar rates[][6] =
				{
					_T("11025"), 
					_T("22050"), 
					_T("44100"), 
					_T("48000"),
					_T("88200"),
					_T("96000")
				};

				uint index;

				switch (nes.GetSampleRate())
				{
					case 11025U: index = 0; break;
					case 22050U: index = 1; break;
					case 48000U: index = 3; break;
					case 88200U: index = 4; break;
					case 96000U: index = 5; break;
					default:	 index = 2; break;
				}

				const Control::ComboBox comboBox( dialog.ComboBox(IDC_SOUND_SAMPLE_RATE) );

				comboBox.Add( rates, NST_COUNT(rates) );
				comboBox[index].Select();
			}

			dialog.RadioButton( (nes.GetSampleBits() == 8 ? IDC_SOUND_8_BIT : IDC_SOUND_16_BIT) ).Check();
			dialog.RadioButton( (nes.GetSpeaker() == Nes::Sound::SPEAKER_STEREO ? IDC_SOUND_STEREO : IDC_SOUND_MONO) ).Check();

			for (uint i=0; i < NUM_CHANNELS; ++i)
			{
				const Control::Slider control( dialog.Slider(channelLut[i].ctrlSlider) );
				control.SetRange( 0, VOLUME_MAX );
				control.Position() = VOLUME_MAX - settings.volumes[i];
				dialog.Edit( channelLut[i].ctrlValue ) << (uint) settings.volumes[i];
			}

			{
				const Control::Slider control( dialog.Slider(IDC_SOUND_LATENCY) );
				control.SetRange( 1, LATENCY_MAX );
				control.Position() = settings.latency;
			}

			dialog.CheckBox( IDC_SOUND_ADJUST_PITCH ).Check( nes.IsAutoTransposing() );

			if (settings.adapter == UINT_MAX)
				Enable( FALSE );
		}
		else
		{
			dialog.Control( IDC_SOUND_DEVICE ).Disable();
			dialog.Control( IDC_SOUND_DEFAULT ).Disable();

			Enable( FALSE );
		}

		return TRUE;
	}

	void Sound::ResetVolumeSliders()
	{
		for (uint i=0; i < NUM_CHANNELS; ++i)
		{
			settings.volumes[i] = DEFAULT_VOLUME;
			dialog.Slider( channelLut[i].ctrlSlider ).Position() = VOLUME_MAX - DEFAULT_VOLUME;
			dialog.Edit( channelLut[i].ctrlValue ) << (uint) DEFAULT_VOLUME;
		}
	}

	ibool Sound::OnVScroll(Param& param)
	{
		for (uint i=0; i < NUM_CHANNELS; ++i)
		{
			if (param.Slider().IsControl( channelLut[i].ctrlSlider ))
			{
				const uchar volume = VOLUME_MAX - param.Slider().Scroll();

				if (settings.volumes[i] != volume)
				{
					settings.volumes[i] = volume;
					dialog.Edit( channelLut[i].ctrlValue ) << volume;
				}

				break;
			}
		}

		return TRUE;
	}

	ibool Sound::OnDestroy(Param&)
	{	
		if (adapters.size())
		{
			settings.adapter = dialog.ComboBox( IDC_SOUND_DEVICE ).Selection().GetIndex() - 1U;
			settings.latency = dialog.Slider( IDC_SOUND_LATENCY ).Position();	

			static const uint rates[] = {11025,22050,44100,48000,88200,96000};

			nes.SetSampleRate( rates[dialog.ComboBox( IDC_SOUND_SAMPLE_RATE ).Selection().GetIndex()] );
			nes.SetSampleBits( dialog.RadioButton( IDC_SOUND_8_BIT ).IsChecked() ? 8 : 16 );
			nes.SetSpeaker( dialog.RadioButton( IDC_SOUND_STEREO ).IsChecked() ? Nes::Sound::SPEAKER_STEREO : Nes::Sound::SPEAKER_MONO );
			nes.SetAutoTranspose( dialog.CheckBox( IDC_SOUND_ADJUST_PITCH ).IsChecked() );

			for (uint i=1; i < NUM_CHANNELS; ++i)
				nes.SetVolume( channelLut[i].channel, GetVolume( channelLut[i].channel ) );
		}

		return TRUE;
	}

	ibool Sound::OnCmdDevice(Param&)
	{
		Enable( dialog.ComboBox( IDC_SOUND_DEVICE ).Selection().GetIndex() != 0 );
		return TRUE;
	}

	ibool Sound::OnCmdBits(Param& param)
	{
		if (param.Button().IsClicked())
		{
			const uint cmd = param.Button().GetId();
			dialog.RadioButton( IDC_SOUND_16_BIT ).Check( cmd == IDC_SOUND_16_BIT );
			dialog.RadioButton( IDC_SOUND_8_BIT ).Check( cmd == IDC_SOUND_8_BIT );
		}

		return TRUE;
	}

	ibool Sound::OnCmdOutput(Param& param)
	{
		if (param.Button().IsClicked())
		{
			const uint cmd = param.Button().GetId();
			dialog.RadioButton( IDC_SOUND_MONO ).Check( cmd == IDC_SOUND_MONO );
			dialog.RadioButton( IDC_SOUND_STEREO ).Check( cmd == IDC_SOUND_STEREO );
		}

		return TRUE;
	}

	ibool Sound::OnCmdResetSliders(Param& param)
	{
		if (param.Button().IsClicked())
			ResetVolumeSliders();

		return TRUE;
	}

	ibool Sound::OnCmdDefault(Param& param)
	{
		NST_VERIFY( adapters.size() );

		if (param.Button().IsClicked())
		{
			Enable( TRUE );

			dialog.ComboBox( IDC_SOUND_DEVICE )[GetDefaultAdapter()+1].Select();
			dialog.ComboBox( IDC_SOUND_SAMPLE_RATE )[2].Select();	

			dialog.RadioButton( IDC_SOUND_16_BIT ).Check();
			dialog.RadioButton( IDC_SOUND_8_BIT ).Uncheck();

			dialog.RadioButton( IDC_SOUND_MONO ).Check();
			dialog.RadioButton( IDC_SOUND_STEREO ).Uncheck();

			dialog.CheckBox( IDC_SOUND_ADJUST_PITCH ).Uncheck();
			dialog.Slider( IDC_SOUND_LATENCY ).Position() = DEFAULT_LATENCY;

			ResetVolumeSliders();
		}

		return TRUE;
	}

	ibool Sound::OnCmdOk(Param& param)
	{
		if (param.Button().IsClicked())
			dialog.Close();

		return TRUE;
	}

	void Sound::Enable(const ibool state)
	{
		dialog.Control( IDC_SOUND_SAMPLE_RATE   ).Enable( state );
		dialog.Control( IDC_SOUND_8_BIT         ).Enable( state );
		dialog.Control( IDC_SOUND_16_BIT        ).Enable( state );
		dialog.Control( IDC_SOUND_LATENCY       ).Enable( state );
		dialog.Control( IDC_SOUND_MONO          ).Enable( state );
		dialog.Control( IDC_SOUND_STEREO        ).Enable( state );
		dialog.Control( IDC_SOUND_ADJUST_PITCH  ).Enable( state );
		dialog.Control( IDC_SOUND_RESET_SLIDERS ).Enable( state );

		for (uint i=0; i < NUM_CHANNELS; ++i)
		{
			dialog.Control( channelLut[i].ctrlSlider ).Enable( state );
			dialog.Control( channelLut[i].ctrlValue ).Enable( state );
		}
	}

	struct Sound::Recorder::Handlers
	{
		static const MsgHandler::Entry<Recorder> messages[];
		static const MsgHandler::Entry<Recorder> commands[];
	};

	const MsgHandler::Entry<Sound::Recorder> Sound::Recorder::Handlers::messages[] =
	{
		{ WM_INITDIALOG, &Recorder::OnInitDialog }
	};

	const MsgHandler::Entry<Sound::Recorder> Sound::Recorder::Handlers::commands[] =
	{
		{ IDC_SOUND_CAPTURE_CLEAR,  &Recorder::OnCmdClear  },
		{ IDC_SOUND_CAPTURE_BROWSE, &Recorder::OnCmdBrowse },
		{ IDC_SOUND_CAPTURE_OK,     &Recorder::OnCmdOk     },
		{ IDC_SOUND_CAPTURE_CANCEL, &Recorder::OnCmdCancel }
	};

	Sound::Recorder::Recorder(const Managers::Paths& p)
	: dialog(IDD_SOUND_RECORDER,this,Handlers::messages,Handlers::commands), paths(p) {}

	ibool Sound::Recorder::OnInitDialog(Param&)
	{
		dialog.Edit( IDC_SOUND_CAPTURE_FILE ) << waveFile.Ptr();
		dialog.Edit( IDC_SOUND_CAPTURE_FILE ).Limit( _MAX_PATH );

		return TRUE;
	}

	ibool Sound::Recorder::OnCmdClear(Param& param)
	{
		if (param.Button().IsClicked())
			dialog.Edit(IDC_SOUND_CAPTURE_FILE).Clear();

		return TRUE;
	}

	ibool Sound::Recorder::OnCmdBrowse(Param& param)
	{
		if (param.Button().IsClicked())	
		{
			Path tmp;
			dialog.Edit(IDC_SOUND_CAPTURE_FILE) >> tmp;
			dialog.Edit(IDC_SOUND_CAPTURE_FILE).Try() << paths.BrowseSave( Managers::Paths::File::WAVE, Managers::Paths::SUGGEST, tmp ).Ptr();
		}

		return TRUE;
	}

	ibool Sound::Recorder::OnCmdOk(Param& param)
	{
		if (param.Button().IsClicked())
		{
			dialog.Edit(IDC_SOUND_CAPTURE_FILE) >> waveFile;
			paths.FixFile( Managers::Paths::File::WAVE, waveFile );
			dialog.Close();
		}

		return TRUE;
	}

	ibool Sound::Recorder::OnCmdCancel(Param& param)
	{
		if (param.Button().IsClicked())
			dialog.Close();

		return TRUE;
	}
}
