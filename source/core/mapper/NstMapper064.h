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

#ifndef NST_MAPPER_64_H
#define NST_MAPPER_64_H

NES_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////
// mapper 64
////////////////////////////////////////////////////////////////////////////////////////

class MAPPER64 : public MAPPER
{
public:

	MAPPER64(CONTEXT& c)
	: MAPPER(c,regs,regs+3) {}

private:

	VOID Reset();
	VOID IrqSync();

	NES_DECL_POKE( 8000 );
	NES_DECL_POKE( 8001 );
	NES_DECL_POKE( A000 );
	NES_DECL_POKE( C000 );
	NES_DECL_POKE( C001 );
	NES_DECL_POKE( E000 );
	NES_DECL_POKE( E001 );

	UINT regs[3];
};

NES_NAMESPACE_END

#endif
