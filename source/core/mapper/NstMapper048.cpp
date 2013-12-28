////////////////////////////////////////////////////////////////////////////////////////
//
// Nestopia - NES / Famicom emulator written in C++
//
// Copyright (C) 2003 Martin Freij
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

#include "NstMappers.h"
#include "NstMapper048.h"

NES_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID MAPPER48::Reset()
{
	EnableIrqSync( IRQSYNC_PPU );

	DoMirroring = TRUE;

	cpu.SetPort( 0x8000, this, Peek_8000, Poke_8000 );
	cpu.SetPort( 0x8001, this, Peek_8000, Poke_8001 );
	cpu.SetPort( 0x8002, this, Peek_8000, Poke_8002 );
	cpu.SetPort( 0x8003, this, Peek_8000, Poke_8003 );
	cpu.SetPort( 0xA000, this, Peek_A000, Poke_A000 );
	cpu.SetPort( 0xA001, this, Peek_A000, Poke_A001 );
	cpu.SetPort( 0xA002, this, Peek_A000, Poke_A002 );
	cpu.SetPort( 0xA003, this, Peek_A000, Poke_A003 );
	cpu.SetPort( 0xC000, this, Peek_C000, Poke_C000 );
	cpu.SetPort( 0xC001, this, Peek_C000, Poke_C001 );
	cpu.SetPort( 0xE000, this, Peek_E000, Poke_E000 );
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_POKE(MAPPER48,8000)	
{
	if (DoMirroring)
		ppu.SetMirroring( (data & 0x40) ? MIRROR_HORIZONTAL : MIRROR_VERTICAL );

	apu.Update();
	pRom.SwapBanks<n8k,0x0000>( data );
}

NES_POKE(MAPPER48,8001)	
{
	apu.Update();
	pRom.SwapBanks<n8k,0x2000>( data );
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_POKE(MAPPER48,8002)	{ ppu.Update(); cRom.SwapBanks<n2k,0x0000>(data); }
NES_POKE(MAPPER48,8003)	{ ppu.Update(); cRom.SwapBanks<n2k,0x0800>(data); }
NES_POKE(MAPPER48,A000)	{ ppu.Update(); cRom.SwapBanks<n1k,0x1000>(data); }
NES_POKE(MAPPER48,A001)	{ ppu.Update(); cRom.SwapBanks<n1k,0x1400>(data); }
NES_POKE(MAPPER48,A002)	{ ppu.Update(); cRom.SwapBanks<n1k,0x1800>(data); }
NES_POKE(MAPPER48,A003)	{ ppu.Update(); cRom.SwapBanks<n1k,0x1C00>(data); }

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_POKE(MAPPER48,C000)	{ IrqCount = data; }
NES_POKE(MAPPER48,C001)	{ SetIrqEnable(data & 0x1); }

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_POKE(MAPPER48,E000)	
{  
	ppu.SetMirroring( (data & 0x40) ? MIRROR_HORIZONTAL : MIRROR_VERTICAL );
	DoMirroring = FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID MAPPER48::IrqSync()
{
	if (IrqCount++ == 0xFF)
	{
		--IrqCount;
		SetIrqEnable(FALSE);
		cpu.TryIRQ();
	}
}

NES_NAMESPACE_END
