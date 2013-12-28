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
#include "NstMapper004.h"
#include "NstMapper074.h"

NES_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID MAPPER74::Reset()
{
	EnableCartridgeCRam();
	MAPPER4::Reset();
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID MAPPER74::UpdatePRom()
{
	apu->Update(); 

	if (command & SWAP_PROM_BANKS)
	{
		pRom.SwapBanks<n8k,0x0000>( pRomBanks[2] & 0x3F );
		pRom.SwapBanks<n8k,0x2000>( pRomBanks[1] & 0x3F );
		pRom.SwapBanks<n8k,0x4000>( pRomBanks[0] & 0x3F );
	}
	else
	{
		pRom.SwapBanks<n8k,0x0000>( pRomBanks[0] & 0x3F );
		pRom.SwapBanks<n8k,0x2000>( pRomBanks[1] & 0x3F );
		pRom.SwapBanks<n8k,0x4000>( pRomBanks[2] & 0x3F );
	}

	pRom.SwapBanks<n8k,0x6000>( pRomBanks[3] & 0x3F );
}

NES_NAMESPACE_END