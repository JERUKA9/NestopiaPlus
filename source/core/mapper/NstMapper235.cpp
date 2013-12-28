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
#include "NstMapper235.h"
		
NES_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID MAPPER235::Reset()
{
	cpu.SetPort( 0x8000, 0xFFFF, this, Peek_pRom, Poke_pRom );

	pRom.SwapBanks<n32k,0x0000>(0);

	switch (pRom.Size())
	{
     	case 0x00100000UL: SelectCartridge = Select128k; break;
		case 0x00200000UL: SelectCartridge = Select256k; break;
		case 0x00300000UL: SelectCartridge = Select384k; break;
		default:           SelectCartridge = NULL;       break;
	}

	dummy = 0;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID MAPPER235::Select128k(const UINT address)
{
	dummy = address & 0x0300;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID MAPPER235::Select256k(const UINT address)
{
	switch (address & 0x0300)
	{
		case 0x0000: dummy = 0; return;
		case 0x0100: dummy = 1; return;
		case 0x0200: dummy = 0; bank = (bank & 0x1F) | 0x20; return;
		case 0x0300: dummy = 1; return;
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID MAPPER235::Select384k(const UINT address)
{
	switch (address & 0x0300)
	{
     	case 0x0000: dummy = 0;	return;
     	case 0x0100: dummy = 1; return;
     	case 0x0200: dummy = 0; bank = (bank & 0x1F) | 0x20; return;
     	case 0x0300: dummy = 0; bank = (bank & 0x1F) | 0x40; return;
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_POKE(MAPPER235,pRom) 
{ 
	ppu.SetMirroring
	( 
		(address & 0x0400) ? MIRROR_ZERO : 
       	(address & 0x2000) ? MIRROR_HORIZONTAL : 
		MIRROR_VERTICAL 
	);

	bank = ((address & 0x0300) >> 3) | (address & 0x001F);

	if (SelectCartridge)
		(*this.*SelectCartridge)(address);

	if (address & 0x0800)
	{
		if (address & 0x1000)
		{
			pRom.SwapBanks<n16k,0x0000>(bank << 1);
			pRom.SwapBanks<n16k,0x4000>(bank << 1);
		}
		else
		{
			pRom.SwapBanks<n16k,0x0000>(bank << 0);
			pRom.SwapBanks<n16k,0x4000>(bank << 0);
		}
	}
	else
	{
		pRom.SwapBanks<n32k,0x0000>(bank);
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_PEEK(MAPPER235,pRom)
{
	return dummy ? cpu.GetCache() : pRom[address - 0x8000];
}

NES_NAMESPACE_END
