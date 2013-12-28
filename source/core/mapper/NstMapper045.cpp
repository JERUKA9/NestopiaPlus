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
#include "../board/NstBrdMmc3.hpp"
#include "NstMapper045.hpp"
	   
namespace Nes
{
	namespace Core
	{
        #ifdef NST_PRAGMA_OPTIMIZE
        #pragma optimize("s", on)
        #endif

		void Mapper45::SubReset(const bool hard)
		{
			if (hard)
			{
				for (uint i=0; i < 5; ++i)
					exRegs[i] = 0;
			}
	
			hack = true;

			Mmc3::SubReset( hard );
	
			Map( 0x6000U, 0x7FFFU, &Mapper45::Poke_6000 );
		}
	
		void Mapper45::SubLoad(State::Loader& state)
		{
			while (const dword chunk = state.Begin())
			{
				if (chunk == NES_STATE_CHUNK_ID('R','E','G','\0'))
				{
					const State::Loader::Data<5> data( state );
	
					for (uint i=0; i < 5; ++i)
						exRegs[i] = data[i];
				}
	
				state.End();
			}
		}
	
		void Mapper45::SubSave(State::Saver& state) const
		{
			const u8 data[5] = 
			{
				exRegs[0],
				exRegs[1],
				exRegs[2],
				exRegs[3],
				exRegs[4]
			};

			state.Begin('R','E','G','\0').Write( data ).End();
		}
	
        #ifdef NST_PRAGMA_OPTIMIZE
        #pragma optimize("", on)
        #endif

		NES_POKE(Mapper45,6000)
		{
			if (hack)
			{
				hack = false;

				// King of Fighters will not work unless APU 
				// frame IRQ's are disabled on power-on

				cpu.Poke( 0x4017, 0x40 );
			}

			if (!(exRegs[3] & 0x40))
			{
				exRegs[exRegs[4]] = data;
				exRegs[4] = (exRegs[4] + 1) & 0x3;
	
				Mapper45::UpdatePrg();
				Mapper45::UpdateChr();
			}
		}
	
		void Mapper45::UpdatePrg()
		{
			const uint r = (exRegs[3] & 0x3F) ^ 0x3F;
			const uint i = (regs.ctrl0 & Regs::CTRL0_XOR_PRG) >> 5;
	
			prg.SwapBanks<NES_8K,0x0000U>
			( 
		    	(banks.prg[i]   & r) | exRegs[1],
				(banks.prg[1]   & r) | exRegs[1],
				(banks.prg[i^2] & r) | exRegs[1],
				(banks.prg[3]   & r) | exRegs[1]
			);
		}
	
		void Mapper45::UpdateChr() const
		{
			ppu.Update();
	
			static const uchar masks[16] =
			{
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
				0x01,0x03,0x07,0x0F,0x1F,0x3F,0x7F,0xFF
			};
	
			const uint r[2] =
			{
				masks[exRegs[2] & 0x0F],
				exRegs[0] | ((exRegs[2] & 0xF0) << 4)
			};
	
			const uint swap = (regs.ctrl0 & Regs::CTRL0_XOR_CHR) << 5;
	
			chr.SwapBanks<NES_1K>
			( 
		     	0x0000U ^ swap, 
				(((banks.chr[0] << 1) | 0) & r[0]) | r[1],
				(((banks.chr[0] << 1) | 1) & r[0]) | r[1],
				(((banks.chr[1] << 1) | 0) & r[0]) | r[1],
				(((banks.chr[1] << 1) | 1) & r[0]) | r[1]
			); 
	  
			chr.SwapBanks<NES_1K>
			( 
		     	0x1000U ^ swap, 
				(banks.chr[2] & r[0]) | r[1],
				(banks.chr[3] & r[0]) | r[1],
				(banks.chr[4] & r[0]) | r[1],
				(banks.chr[5] & r[0]) | r[1]
			);
  	  	}
	}
}
