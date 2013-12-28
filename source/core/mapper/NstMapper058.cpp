////////////////////////////////////////////////////////////////////////////////////////
//
// Nestopia - NES / Famicom emulator written in C++
//
// Copyright (C) 2003-2005 Martin Freij
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

#include "../NstMapper.hpp"
#include "NstMapper058.hpp"

namespace Nes
{
	namespace Core
	{
        #ifdef NST_PRAGMA_OPTIMIZE
        #pragma optimize("s", on)
        #endif
	
		Mapper58::Mapper58(Context& c)
		: 
		Mapper    (c), 
		studyGame ((c.pRomCrc == 0xABB2F974UL) || (c.pRomCrc == 0x6A24FA34UL)) // Study and Game 32-in-1
		{}
	
		void Mapper58::SubReset(const bool hard)
		{
			if (hard)
				prg.SwapBank<NES_16K,0x4000U>( studyGame );

			Map( 0x8000U, 0xFFFFU, &Mapper58::Poke_Prg );
		}
	
		#ifdef NST_PRAGMA_OPTIMIZE
        #pragma optimize("", on)
        #endif
	
		NES_POKE(Mapper58,Prg) 
		{
			if (studyGame)
			{
				// Study and Game 32-in-1
				prg.SwapBank<NES_32K,0x0000U>( data & 0x1F );
			}
			else
			{
				ppu.SetMirroring( (data & 0x2) ? Ppu::NMT_VERTICAL : Ppu::NMT_HORIZONTAL );
	
				if (address & 0x40)
					prg.SwapBanks<NES_16K,0x0000U>( address & 0x7, address & 0x7 );
				else
					prg.SwapBank<NES_32K,0x0000U>( (address & 0x6) >> 1 );
	
				chr.SwapBank<NES_8K,0x0000U>( (address & 0x38) >> 3 );
			}
		}
	}
}
