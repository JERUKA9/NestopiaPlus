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

#include "../NstMapper.hpp"
#include "NstMapper185.hpp"
	  
namespace Nes
{
	namespace Core
	{
        #ifdef NST_PRAGMA_OPTIMIZE
        #pragma optimize("s", on)
        #endif
	
		Mapper185::Mapper185(Context& c)
		: 
		Mapper  (c,CRAM_8K), 
		compare (c.pRomCrc == 0xB36457C7UL ? 0x3 : c.pRomCrc == 0xA03C98A7UL ? 0x1 : 0x0) // Spy vs Spy, Seicross
		{}
	
		void Mapper185::SubReset(const bool hard)
		{
			if (hard)
			{
				chr.Source(1).Fill(0xFF);
				chr.Source(1).SwapBank<SIZE_8K,0x0000U>(0);
			}

			Map( 0x8000U, 0xFFFFU, &Mapper185::Poke_Prg );
		}
	
        #ifdef NST_PRAGMA_OPTIMIZE
        #pragma optimize("", on)
        #endif
	
		NES_POKE(Mapper185,Prg)
		{
			// Bird Week  : $C020:F0 $C021:0F
			// B-Wings    : $B69F:00 $B69E:33
			// Bomb Jack  : $8071:00 $8072:11
			// Sansuu 2.. : $805C:20 $805D:22
			// Sanssu 2.. : $803F:20 $8040:22
			// Sanssu 3.. : $805C:00 $805D:FF
			// Seicross   : $80DF:21 $80DE:20
			// Spy vs Spy : $8090:13 $8091:21

			ppu.Update();	
			chr.Source( (data & 0x3) == compare ).SwapBank<SIZE_8K,0x0000U>(0);
		}
	}
}
