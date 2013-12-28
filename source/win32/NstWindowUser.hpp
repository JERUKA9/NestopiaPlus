////////////////////////////////////////////////////////////////////////////////////////
//
// Nestopia - NES / Famicom emulator written in C++
//
// Copyright (C) 2003-2005 Martin Freij
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

#ifndef NST_WINDOW_USER_H
#define NST_WINDOW_USER_H

#pragma once

#include "NstMain.hpp"

namespace Nestopia
{
	namespace String
	{
		class Heap;
	}

	namespace Window
	{
		namespace User
		{
			enum Type
			{
				FAIL,
				WARN,
				INFORM,
				CONFIRM
			};

			void  Fail    (uint,uint=0);
			void  Warn    (uint,uint=0);
			void  Inform  (uint,uint=0);
			ibool Confirm (uint,uint=0);
			ibool Issue   (Type,uint,uint=0);
			ibool Input   (String::Heap&,cstring,cstring=0);
			ibool Input   (String::Heap&,uint,uint=0);
		}
	}
}

#endif