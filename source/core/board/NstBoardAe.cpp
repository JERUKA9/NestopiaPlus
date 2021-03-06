////////////////////////////////////////////////////////////////////////////////////////
//
// Nestopia - NES/Famicom emulator written in C++
//
// Copyright (C) 2003-2008 Martin Freij
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

#include "NstBoard.hpp"
#include "NstBoardAe.hpp"

namespace Nes
{
	namespace Core
	{
		namespace Boards
		{
			namespace Ae
			{
				#ifdef NST_MSVC_OPTIMIZE
				#pragma optimize("s", on)
				#endif

				void Standard::SubReset(const bool hard)
				{
					Map( 0x8000U, 0xFFFFU, &Standard::Poke_8000 );

					if (hard)
						prg.SwapBank<SIZE_32K,0x0000>(0);
				}

				#ifdef NST_MSVC_OPTIMIZE
				#pragma optimize("", on)
				#endif

				NES_POKE_AD(Standard,8000)
				{
					uint bank = (address >> 7 & 0x1F) + (address >> 7 & address >> 8 & 0x10);

					if (address & 0x20)
					{
						bank = (bank << 2) | (address >> 5 & 0x2);
						prg.SwapBanks<SIZE_16K,0x0000>( bank, bank );
					}
					else
					{
						prg.SwapBank<SIZE_32K,0x0000>( bank );
					}

					ppu.SetMirroring( (address & 0x2000) ? Ppu::NMT_H : Ppu::NMT_V );
					chr.SwapBank<SIZE_8K,0x0000>( (address << 2 & 0x3C) | (data & 0x03) );
				}
			}
		}
	}
}
