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

#include "../NstTypes.h"
#include "../NstMachine.h"
#include "NstKeyboard.h"

NES_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

FAMILYBASIC::FAMILYBASIC() 
: 
CONTROLLER (0),
scan       (0),
mode       (0)
{}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID FAMILYBASIC::Reset()
{
	scan = 0;
	mode = 0;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID FAMILYBASIC::Poke_4016(const UINT data)
{
	if (data & COMMAND_KEY)
	{
		const UINT out = data & COMMAND_SCAN;

		if (mode && !out && ++scan > 9)
			scan = 0;

		mode = out ? 1 : 0;

		if (data & COMMAND_RESET)
			scan = 0;
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

UINT FAMILYBASIC::Peek_4017()
{
	if (input)
	{
		input->FamilyKeyboard.Poll( scan, mode );
		return input->FamilyKeyboard.parts[scan] ^ 0x1E;
	}

	return 0xFF;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

ULONG FAMILYBASIC::GetState() const
{
	return 
	(
     	(scan << 0) | 
		(mode << 8)
	);
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID FAMILYBASIC::SetState(const ULONG data)
{
	scan = (data & 0x00FFU) >> 0;
	mode = (data & 0xFF00U) >> 8;
}

NES_NAMESPACE_END
