////////////////////////////////////////////////////////////////////////////////////////
//
// Nestopia - NES / Famicom emulator written in C++
//
// Copyright (C) 2003-2006 Martin Freij
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

#if !defined(NDEBUG) && defined(_WIN32)

#include <cstdio>	
#include <cstring>
#include <cstdlib>
#include <new>

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <tchar.h>
#include "NstTypes.hpp"

#ifdef _MSC_VER
#pragma warning( push )
#ifdef __INTEL_COMPILER
#pragma warning( disable : 981 )
#else
#pragma warning( disable : 4996 )
#endif
#endif

namespace Nes
{
	namespace Assertion
	{
		uint NST_CALL Issue
		(
			cstring const expression,
			cstring const msg,
			cstring const file,
			cstring const function,
			const int line
		)
		{
			const ulong length = 
			(
				(msg ?        std::strlen(msg)        :  0) + 
				(expression ? std::strlen(expression) : 16) + 
				(file ?       std::strlen(file)       : 16) + 
				(function ?   std::strlen(function)   : 16) + 
				64 + 1
			);	

			static const TCHAR title[] = __T("Nestopia Debug Assertion!");
			static const char breakpoint[] = "break point";
			static const char unknown[] = "unknown";

			char* const buffer = new (std::nothrow) char [length];

            #ifdef _UNICODE
			wchar_t* const message = new (std::nothrow) wchar_t [length];
            #else
			const char* const message = buffer;
            #endif

			if (!buffer || !message)
			{
				::MessageBox
				(
     				::GetActiveWindow(),
					__T("Out of memory!"),
					title,
					MB_OK|MB_ICONERROR|MB_SETFOREGROUND|MB_TOPMOST
				);

				::FatalExit( EXIT_FAILURE );
			}

			if (msg) 
			{
				std::sprintf
				(
					buffer,
					"%s, Expression: %s\n\n File: %s\n Function: %s\n Line: %i",
					msg,
					expression ? expression : breakpoint,
					file,
					function ? function : unknown,
					line
				);  
			}
			else
			{
				std::sprintf
				(
					buffer,
					"Expression: %s\n\n File: %s\n Function: %s\n Line: %i",
					expression ? expression : breakpoint,
					file,
					function ? function : unknown,
					line
				); 
			}

            #ifdef _UNICODE
			std::mbstowcs( message, buffer, std::strlen(buffer) + 1 );
			delete [] buffer;
            #endif

			int result = ::MessageBox	         						 
			(																 
       			::GetActiveWindow(),														 
				message,                                                       
				title,										 
				MB_ABORTRETRYIGNORE|MB_SETFOREGROUND|MB_TOPMOST
			);			

			delete [] message;

			if (result != IDABORT)
				return result == IDIGNORE ? 1 : 2;
			
			result = ::MessageBox
			(																 
    			::GetActiveWindow(),														 
				__T("break into the debugger?"),
				title, 
				MB_YESNO|MB_SETFOREGROUND|MB_TOPMOST
			);	

			if (result == IDNO)
				::FatalExit( EXIT_FAILURE );

			return 0;
		}
	}
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
