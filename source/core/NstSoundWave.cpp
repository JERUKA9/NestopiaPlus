////////////////////////////////////////////////////////////////////////////////////////
//
// Nestopia - NES / Famicom emulator written in C++
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

#include "NstCore.hpp"
#include "NstCpu.hpp"
#include "NstSoundWave.hpp"

namespace Nes
{
	namespace Core
	{
        #ifdef NST_PRAGMA_OPTIMIZE
        #pragma optimize("s", on)
        #endif

		SoundWave::SoundWave(Cpu& c)
		: cpu(c)
		{
			cpu.GetApu().HookChannel( this );
		}
		
		SoundWave::~SoundWave()
		{
			cpu.GetApu().ReleaseChannel();
		}

		Result SoundWave::CanDo(const u8* data,dword length,dword rate)
		{
			if (data == NULL || length == 0 || rate == 0)
				return RESULT_ERR_INVALID_PARAM;

			if (rate < 8000U || rate > 96000UL)
				return RESULT_ERR_UNSUPPORTED;

			return RESULT_OK;
		}

		void SoundWave::Reset()
		{
			wave.data = NULL;
		}

		void SoundWave::UpdateContext(uint)
		{
			wave.data = NULL;
		}

        #ifdef NST_PRAGMA_OPTIMIZE
        #pragma optimize("", on)
        #endif

		void SoundWave::Play(const u8* w,dword l,dword r)
		{
			NST_ASSERT( NES_SUCCEEDED(CanDo(w,l,r)) );
			
			wave.data = w;
			wave.length = l;
			wave.rate = r;
			
			pos = 0;
			rate = cpu.GetApu().GetSampleRate();			
		}

		SoundWave::Sample SoundWave::GetSample()
		{
			if (wave.data)
			{
				const uint i = pos / rate;

				if (i < wave.length)
				{
					pos += wave.rate;
    				return Sample((wave.data[i] + (wave.data[i] > 0x80)) << 8) - 32768L;
				}

				wave.data = NULL;
			}

			return 0;
		}
	}
}