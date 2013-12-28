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
#include "NstMapper198.h"
		 
NES_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID MAPPER198::Reset()
{
	cpu.SetPort( 0x4019, 0x5FFF, this, Peek_ExRam, Poke_ExRam );
	cpu.SetPort( 0x6000, 0x7FFF, this, Peek_wRam,  Poke_wRam  );

	for (UINT i=0x8000; i < 0xAFFF; ++i)
	{
		switch (i & 0xE001)
		{
			case 0x8000: cpu.SetPort( i, this, Peek_8000, Poke_8000 ); continue;
			case 0x8001: cpu.SetPort( i, this, Peek_8000, Poke_8001 ); continue;
			case 0xA000: cpu.SetPort( i, this, Peek_A000, Poke_A000 ); continue;
		}
	}

	command = 0x0;

	pBanks[0] = 0x0;
	pBanks[1] = 0x1;
	
	UpdatePRom();
	
	cBanks[0] = 0x0 >> 1;
	cBanks[1] =	0x2 >> 1;
	cBanks[2] =	0x4;
	cBanks[3] =	0x5;
	cBanks[4] = 0x6;
	cBanks[5] = 0x7;

	UpdateCRom();
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_PEEK(MAPPER198,ExRam) { return ExRam[address - 0x4019]; }
NES_POKE(MAPPER198,ExRam) { ExRam[address - 0x4019] = data; }
NES_PEEK(MAPPER198,wRam)  { return wRam[address & 0xFFF];   }
NES_POKE(MAPPER198,wRam)  { wRam[address & 0xFFF] = data;   }

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_POKE(MAPPER198,8000) 
{ 
	command = data;

	UpdatePRom();
	UpdateCRom();
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_POKE(MAPPER198,8001) 
{ 
	switch (command & 0x7)
	{
       	case 0x0:
		case 0x1:

			cBanks[command & 0x7] = data >> 1;
			UpdateCRom();
			return;

		case 0x2:
		case 0x3:
		case 0x4:
		case 0x5:

			cBanks[command & 0x7] = data;
			UpdateCRom();
			return;

		case 0x6:

			pBanks[0] = (data >= 0x50) ? (data & 0x4F) : data;
			UpdatePRom();
			return;

		case 0x7:

			pBanks[1] = data;
			UpdatePRom();
			return;
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_POKE(MAPPER198,A000)
{
	if (mirroring != MIRROR_FOURSCREEN)
		ppu.SetMirroring( (data & 0x1) ? MIRROR_HORIZONTAL : MIRROR_VERTICAL );
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID MAPPER198::UpdatePRom()
{
	apu.Update();

	if (command & 0x40)
	{
		pRom.SwapBanks<n8k,0x0000>( pRom.NumBanks<n8k>() - 2 );
		pRom.SwapBanks<n8k,0x2000>( pBanks[1] );
		pRom.SwapBanks<n8k,0x4000>( pBanks[0] );
		pRom.SwapBanks<n8k,0x6000>( pRom.NumBanks<n8k>() - 1 );
	}
	else
	{
		pRom.SwapBanks<n8k,0x0000>( pBanks[0] );
		pRom.SwapBanks<n8k,0x2000>( pBanks[1] );
		pRom.SwapBanks<n8k,0x4000>( pRom.NumBanks<n8k>() - 2 );
		pRom.SwapBanks<n8k,0x6000>( pRom.NumBanks<n8k>() - 1 );
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID MAPPER198::UpdateCRom()
{
	if (cRom.Size())
	{
		ppu.Update();

		if (command & 0x80)
		{
			cRom.SwapBanks<n1k,0x0000>( cBanks[2] );
			cRom.SwapBanks<n1k,0x0400>( cBanks[3] );
			cRom.SwapBanks<n1k,0x0800>( cBanks[4] );
			cRom.SwapBanks<n1k,0x0C00>( cBanks[5] );
			cRom.SwapBanks<n2k,0x1000>( cBanks[0] );
			cRom.SwapBanks<n2k,0x1800>( cBanks[1] );
		}
		else
		{
			cRom.SwapBanks<n2k,0x0000>( cBanks[0] );
			cRom.SwapBanks<n2k,0x0800>( cBanks[1] );
			cRom.SwapBanks<n1k,0x1000>( cBanks[2] );
			cRom.SwapBanks<n1k,0x1400>( cBanks[3] );
			cRom.SwapBanks<n1k,0x1800>( cBanks[4] );
			cRom.SwapBanks<n1k,0x1C00>( cBanks[5] );
		}
	}
}

NES_NAMESPACE_END
