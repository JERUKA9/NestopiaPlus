///////////////////////////////////////////////////////////////////////////////////////
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

#ifndef NST_CONFIGFILE_H
#define NST_CONFIGFILE_H

#include "../paradox/PdxFile.h"
#include "../paradox/PdxMap.h"

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

class CONFIGFILE
{
public:

	CONFIGFILE() {}

	PDXRESULT Load(const PDXSTRING&);
	PDXRESULT Save(const PDXSTRING&);

	PDXSTRING& operator [] (const CHAR* const);
	PDXSTRING& operator [] (const PDXSTRING&);

	static const CHAR* FromGUID(const GUID&);
	static GUID ToGUID(const CHAR* const);

private:

	static VOID WriteGuid(PDXSTRING&,const GUID&);
	static VOID FormatString(PDXSTRING&);

	VOID ParseOptions(PDXSTRING* const);
	VOID WriteCommentHeader();
//	VOID WriteCommentVideo();
//	VOID WriteCommentSound();
//	VOID WriteCommentFiles();
//	VOID WriteCommentInput();
//	VOID WriteCommentPreferences();

private:

	typedef PDXMAP<PDXSTRING,PDXSTRING> TREE;

	PDXFILE file;
	TREE tree;
};

#endif