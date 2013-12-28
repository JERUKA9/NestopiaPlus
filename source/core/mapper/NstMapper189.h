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

#ifndef NST_MAPPER_189_H
#define NST_MAPPER_189_H

NES_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////
// mapper 188
////////////////////////////////////////////////////////////////////////////////////////

class MAPPER189 : public MAPPER4
{
public:

	MAPPER189(CONTEXT& c)
	: MAPPER4(c) {}

private:

	VOID Reset();
	VOID IrqSync();

	NES_DECL_POKE( 4100 );
	NES_DECL_POKE( 6100 );
	NES_DECL_POKE( 8000	);
	NES_DECL_POKE( 8001	);
};

NES_NAMESPACE_END

#endif