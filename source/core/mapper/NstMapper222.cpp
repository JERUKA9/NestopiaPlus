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
#include "NstMapper222.hpp"
	   
namespace Nes
{
	namespace Core
	{
        #ifdef NST_PRAGMA_OPTIMIZE
        #pragma optimize("s", on)
        #endif
	
		void Mapper222::SubReset(bool)
		{
			for (uint i=0x0000U; i < 0x1000U; i += 0x4)
			{
				Map( 0x8000U + i, PRG_SWAP_8K_0 );
				Map( 0xA000U + i, PRG_SWAP_8K_1 );
				Map( 0xB000U + i, CHR_SWAP_1K_0 );
				Map( 0xB002U + i, CHR_SWAP_1K_1 );
				Map( 0xC000U + i, CHR_SWAP_1K_2 );
				Map( 0xC002U + i, CHR_SWAP_1K_3 );
				Map( 0xD000U + i, CHR_SWAP_1K_4 );
				Map( 0xD002U + i, CHR_SWAP_1K_5 );
				Map( 0xE000U + i, CHR_SWAP_1K_6 );
				Map( 0xE002U + i, CHR_SWAP_1K_7 );
			}
		}
	
        #ifdef NST_PRAGMA_OPTIMIZE
        #pragma optimize("", on)
        #endif
	}
}
