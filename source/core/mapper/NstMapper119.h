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

#pragma once

#ifndef NST_MAPPER_119_H
#define NST_MAPPER_119_H

NES_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////
// mapper 119
////////////////////////////////////////////////////////////////////////////////////////

class MAPPER119 : public MAPPER4
{
public:

	MAPPER119(CONTEXT& c)
	: MAPPER4(c,SelectCRam,SelectCRam+6), cRam(n8k) {}

	PDXRESULT LoadState(PDXFILE&);
	PDXRESULT SaveState(PDXFILE&) const;

protected:

	VOID Reset();
	VOID UpdateCRom();

	enum
	{
		SELECT_CRAM = 0x40
	};

	NES_DECL_PEEK( 0000 );
	NES_DECL_POKE( 0000 );
	NES_DECL_PEEK( 0400 );
	NES_DECL_POKE( 0400 );
	NES_DECL_POKE( 0800 );
	NES_DECL_PEEK( 0800 );
	NES_DECL_POKE( 0C00 );
	NES_DECL_PEEK( 0C00 );
	NES_DECL_POKE( 1000 );
	NES_DECL_PEEK( 1000 );
	NES_DECL_POKE( 1400 );
	NES_DECL_PEEK( 1400 );
	NES_DECL_POKE( 1800 );
	NES_DECL_PEEK( 1800 );
	NES_DECL_PEEK( 1C00 );
	NES_DECL_POKE( 1C00 );

	CRAM cRam;
	UINT SelectCRam[6];
};

NES_NAMESPACE_END

#endif