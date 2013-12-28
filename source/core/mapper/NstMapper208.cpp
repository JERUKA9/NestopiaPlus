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
#include "../board/NstBrdMmc3.hpp"
#include "NstMapper208.hpp"

namespace Nes
{
	namespace Core
	{
        #ifdef NST_PRAGMA_OPTIMIZE
        #pragma optimize("s", on)
        #endif
	 
		Mapper208::Mapper208(Context& c)
		: 
		Mapper (c), 
		irq    (c.cpu,c.ppu) 
		{}
	
		void Mapper208::SubReset(const bool hard)
		{
			if (hard)
			{
				regs.select = 0;
				regs.command = 0;

				for (uint i=0; i < 4; ++i)
					regs.buffer[i] = 0;
			}

			irq.Reset( hard, hard || irq.IsLineEnabled() );

			Map( 0x4800U, 0x4FFFU, &Mapper208::Poke_4800 );
			Map( 0x5000U, 0x57FFU, &Mapper208::Poke_5000 );
			Map( 0x5800U, 0x5FFFU, &Mapper208::Peek_5800, &Mapper208::Poke_5800 );
	
			for (uint i=0x0000U; i < 0x2000U; i += 0x2)
			{
				Map( 0x8000U + i, &Mapper208::Poke_8000 );
				Map( 0x8001U + i, &Mapper208::Poke_8001 );
				Map( 0xC000U + i, &Mapper208::Poke_C000 );
				Map( 0xC001U + i, &Mapper208::Poke_C001 );
				Map( 0xE000U + i, &Mapper208::Poke_E000 );
				Map( 0xE001U + i, &Mapper208::Poke_E001 );
			}
		}
	
		void Mapper208::SubLoad(State::Loader& state)
		{
			while (const dword chunk = state.Begin())
			{
				switch (chunk)
				{
     				case NES_STATE_CHUNK_ID('R','E','G','\0'):
					{
						const State::Loader::Data<6> data( state );

						regs.command = data[0];
						regs.select = data[1];

						for (uint i=0; i < 4; ++i)
							regs.buffer[i] = data[2+i];

						break;
					}

					case NES_STATE_CHUNK_ID('I','R','Q','\0'):
						
						irq.unit.LoadState( state );
						break;
				}

				state.End();
			}
		}

		void Mapper208::SubSave(State::Saver& state) const
		{
			{
				const u8 data[6] =
				{
					regs.command,
					regs.select,
					regs.buffer[0],
					regs.buffer[1],
					regs.buffer[2],
					regs.buffer[3]
				};

				state.Begin('R','E','G','\0').Write( data ).End();
			}

			irq.unit.SaveState( State::Saver::Subset(state,'I','R','Q','\0').Ref() );
		}
	
        #ifdef NST_PRAGMA_OPTIMIZE
        #pragma optimize("", on)
        #endif
	
		NES_POKE(Mapper208,4800)
		{
			prg.SwapBank<SIZE_32K,0x0000U>( (data & 0x1) | ((data >> 3) & 0x2) );
		}
	
		NES_POKE(Mapper208,5000)
		{
			regs.select = data;
		}
	
		NES_POKE(Mapper208,5800)
		{
			static const uchar lut[256] = 
			{
				0x59,0x59,0x59,0x59,0x59,0x59,0x59,0x59,0x59,0x49,0x19,0x09,0x59,0x49,0x19,0x09,
				0x59,0x59,0x59,0x59,0x59,0x59,0x59,0x59,0x51,0x41,0x11,0x01,0x51,0x41,0x11,0x01,
				0x59,0x59,0x59,0x59,0x59,0x59,0x59,0x59,0x59,0x49,0x19,0x09,0x59,0x49,0x19,0x09,
				0x59,0x59,0x59,0x59,0x59,0x59,0x59,0x59,0x51,0x41,0x11,0x01,0x51,0x41,0x11,0x01,
				0x00,0x10,0x40,0x50,0x00,0x10,0x40,0x50,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
				0x08,0x18,0x48,0x58,0x08,0x18,0x48,0x58,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
				0x00,0x10,0x40,0x50,0x00,0x10,0x40,0x50,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
				0x08,0x18,0x48,0x58,0x08,0x18,0x48,0x58,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
				0x59,0x59,0x59,0x59,0x59,0x59,0x59,0x59,0x58,0x48,0x18,0x08,0x58,0x48,0x18,0x08,
				0x59,0x59,0x59,0x59,0x59,0x59,0x59,0x59,0x50,0x40,0x10,0x00,0x50,0x40,0x10,0x00,
				0x59,0x59,0x59,0x59,0x59,0x59,0x59,0x59,0x58,0x48,0x18,0x08,0x58,0x48,0x18,0x08,
				0x59,0x59,0x59,0x59,0x59,0x59,0x59,0x59,0x50,0x40,0x10,0x00,0x50,0x40,0x10,0x00,
				0x01,0x11,0x41,0x51,0x01,0x11,0x41,0x51,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
				0x09,0x19,0x49,0x59,0x09,0x19,0x49,0x59,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
				0x01,0x11,0x41,0x51,0x01,0x11,0x41,0x51,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
				0x09,0x19,0x49,0x59,0x09,0x19,0x49,0x59,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 
			};
	
			regs.buffer[address & 0x3] = data ^ lut[regs.select];
		}
	
		NES_PEEK(Mapper208,5800)
		{
			return regs.buffer[address & 0x3];
		}
	
		NES_POKE(Mapper208,8000) 
		{ 
			regs.command = data;
		}
	
		NES_POKE(Mapper208,8001) 
		{ 
			ppu.Update();
	
			switch (const uint index = (regs.command & 0x7))
			{
				case 0:
				case 1:
			
					chr.SwapBank<SIZE_2K>( index << 11, data >> 1 );
					break;

				case 2:
				case 3:
				case 4:
				case 5:

					chr.SwapBank<SIZE_1K>( (index + 0x2) << 10, data );
					break;
			}
		}
	
		NES_POKE(Mapper208,C000) 
		{
			irq.Update(); 
			irq.unit.SetLatch( data );
		}
	
		NES_POKE(Mapper208,C001)
		{
			irq.Update();
			irq.unit.Reload();
		}
	
		NES_POKE(Mapper208,E000) 
		{
			irq.Update(); 
			irq.unit.Disable( cpu );
		}
	
		NES_POKE(Mapper208,E001)
		{
			irq.Update();
			irq.unit.Enable();
		}

		void Mapper208::VSync()
		{
			irq.VSync();
		}
	}
}
