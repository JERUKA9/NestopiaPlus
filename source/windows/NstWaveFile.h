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

#ifndef NST_WAVEFILE_H
#define NST_WAVEFILE_H

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

class WAVEFILE
{
public:

	WAVEFILE();
	~WAVEFILE();

	PDXRESULT Open  (const CHAR* const,const WAVEFORMATEX&);
	PDXRESULT Write (const VOID*,const DWORD);
	PDXRESULT Close ();

private:

	HMMIO mmio;
	MMCKINFO mmck;
	MMIOINFO mmckInfo;
	MMCKINFO mmckRiff;
};

#endif
