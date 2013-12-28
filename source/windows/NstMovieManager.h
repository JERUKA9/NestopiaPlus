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

#ifndef NST_MOVIEMANAGER_H
#define NST_MOVIEMANAGER_H

#include "../paradox/PdxString.h"
#include "NstManager.h"

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

class MOVIEMANAGER : public MANAGER
{
public:

	MOVIEMANAGER() 
	: MANAGER(IDD_MOVIE) {}

	VOID Load(PDXSTRING&);

	VOID Play    ();
	VOID Record  ();
	VOID Stop    ();
	VOID Rewind  ();
	VOID Forward ();
	
	VOID SetFile(const PDXSTRING& name)
	{ file = name; }

	inline BOOL CanStop()    const { return nes.CanStopMovie();    }
	inline BOOL CanPlay()    const { return nes.CanPlayMovie();    }
	inline BOOL CanRecord()  const { return nes.CanRecordMovie();  }
	inline BOOL CanRewind()  const { return nes.CanRewindMovie();  }
	inline BOOL CanForward() const { return nes.CanForwardMovie(); }

private:

	VOID UpdateDialog   (HWND);
	VOID UpdateSettings (HWND);
	VOID OnBrowse       (HWND);
	VOID OnClear        (HWND);

	BOOL DialogProc (HWND,UINT,WPARAM,LPARAM);

	PDXSTRING file;
};

#endif
