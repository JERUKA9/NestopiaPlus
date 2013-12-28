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

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#include <Windows.h>
#include "../paradox/PdxString.h"
#include "NstUtilities.h"
#include "NstMenu.h"

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NSTMENU::NSTMENU(HMENU h)
: hMenu(h) {}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NSTMENU::NSTMENU(const UINT id)
: hMenu(::LoadMenu( UTILITIES::GetInstance(), MAKEINTRESOURCE(id) )) {}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID NSTMENU::Load(const UINT id)
{
	hMenu = ::LoadMenu( UTILITIES::GetInstance(), MAKEINTRESOURCE(id) );
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID NSTMENU::Destroy()
{
	if (hMenu)
	{
		::DestroyMenu( hMenu );
		hMenu = NULL;
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

BOOL NSTMENU::Enable(const UINT i,const BOOL enabled,const FLAG flag)
{
	PDX_ASSERT( hMenu && (flag == CMD || flag == POS) );
	const DWORD result = ::EnableMenuItem( hMenu, i, (enabled ? MF_ENABLED : MF_GRAYED) | flag );
	PDX_ASSERT( result != DWORD(-1) );
	return result == MF_ENABLED;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

BOOL NSTMENU::IsEnabled(const UINT i,const FLAG flag) const
{
	PDX_ASSERT( hMenu && (flag == CMD || flag == POS) );
	const UINT state = ::GetMenuState( hMenu, i, flag );
	PDX_ASSERT( state != UINT(-1) );
	return !(state & (MF_DISABLED|MF_GRAYED));
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

BOOL NSTMENU::Check(const UINT i,const BOOL checked,const FLAG flag)
{
	PDX_ASSERT( hMenu && (flag == CMD || flag == POS) );
	const DWORD result = ::CheckMenuItem( hMenu, i, (checked ? MF_CHECKED : MF_UNCHECKED) | flag );
	PDX_ASSERT( result != DWORD(-1) );
	return result == MF_CHECKED;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

BOOL NSTMENU::IsChecked(const UINT i,const FLAG flag) const
{
	PDX_ASSERT( hMenu && (flag == CMD || flag == POS) );
	const UINT state = ::GetMenuState( hMenu, i, flag );	
	PDX_ASSERT( state != UINT(-1) );
	return (state & MF_CHECKED);
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

INT NSTMENU::GetHeight(HWND hWnd) const
{
	PDX_ASSERT( hMenu && hWnd );

	if (hMenu == ::GetMenu( hWnd ))
	{
		MENUBARINFO mbi;
		mbi.cbSize = sizeof(mbi);

		if (::GetMenuBarInfo( hWnd, OBJID_MENU, 0, &mbi ))
			return (mbi.rcBar.bottom - mbi.rcBar.top);
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID NSTMENU::Insert(const UINT uiPos,const CHAR* const text)
{
	PDX_ASSERT( hMenu && text );
	const BOOL success = ::AppendMenu( hMenu, MF_BYPOSITION, uiPos, text );
	PDX_ASSERT( success );
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID NSTMENU::InsertSub(NSTMENU sub,const CHAR* const text)
{
	PDX_ASSERT( hMenu && text );
	const BOOL success = ::AppendMenu( hMenu, MF_POPUP, PDX_CAST(UINT_PTR,sub.hMenu), text );
	PDX_ASSERT( success );
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID NSTMENU::Remove(const UINT i,const FLAG flag)
{
	PDX_ASSERT( hMenu && (flag == CMD || flag == POS) );
	const BOOL success = ::RemoveMenu( hMenu, i, flag );
	PDX_ASSERT( success );
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID NSTMENU::SetText(const UINT i,const CHAR* const string,const FLAG flag)
{
	PDX_ASSERT( hMenu && string && (flag == CMD || flag == POS) );

	PDXSTRING tmp( string );

	MENUITEMINFO info;

	info.cbSize     = sizeof(info);
	info.fMask      = MIIM_STRING;
	info.dwTypeData	= tmp.Begin();

	const BOOL success = ::SetMenuItemInfo( hMenu, i, flag == POS, &info );
	PDX_ASSERT( success );
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

CHAR* NSTMENU::GetText(const UINT i,PDXSTRING& string,const FLAG flag) const
{
	MENUITEMINFO info;

	info.cbSize     = sizeof(info);
	info.fMask      = MIIM_STRING;
	info.dwTypeData	= NULL;

	if (!::GetMenuItemInfo( hMenu, i, flag == POS, &info ))
	{
		PDX_ASSERT(0);
		return NULL;
	}

	string.Resize( ++info.cch );
	info.dwTypeData	= string.Begin();

	if (!::GetMenuItemInfo( hMenu, i, flag == POS, &info ))
	{
		string.Clear();
		PDX_ASSERT(0);
		return NULL;
	}

	string.Validate();

	return string.Begin();
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NSTMENU NSTMENU::GetSub(const UINT uiPos)
{
	PDX_ASSERT( hMenu );
	NSTMENU sub( ::GetSubMenu( hMenu, uiPos ) );
	PDX_ASSERT( sub.hMenu );
	return sub;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID NSTMENU::Clear()
{
	PDX_ASSERT( hMenu );

	for (INT i=::GetMenuItemCount( hMenu ); i > 0; --i)
	{
		const BOOL success = ::DeleteMenu( hMenu, 0, MF_BYPOSITION );
		PDX_ASSERT( success );
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID NSTMENU::Attach(HWND hWnd)
{
	PDX_ASSERT( hWnd && hMenu );
	::SetMenu( hWnd, hMenu );
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID NSTMENU::Detach(HWND hWnd)
{
	PDX_ASSERT( hWnd && hMenu );

	if (::GetMenu( hWnd ) == hMenu)
		::SetMenu( hWnd, NULL );
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID NSTMENU::MakeModeless()
{
	PDX_ASSERT( hMenu );

	MENUINFO MenuInfo;
		
	MenuInfo.cbSize = sizeof(MENUINFO);
	MenuInfo.fMask = MIM_STYLE;
	MenuInfo.dwStyle = MNS_MODELESS;

	const BOOL success = ::SetMenuInfo( hMenu, &MenuInfo );
	PDX_ASSERT( success );
}
