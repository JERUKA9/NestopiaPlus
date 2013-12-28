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

#ifndef NST_DIALOG_VIDEO_H
#define NST_DIALOG_VIDEO_H

#pragma once

#include "NstWindowRect.hpp"
#include "NstWindowDialog.hpp"
#include "NstDirect2D.hpp"
#include "../core/api/NstApiVideo.hpp"

namespace Nestopia
{
	namespace Window
	{
		class Video
		{
		public:

			typedef DirectX::Direct2D::Adapter Adapter;
			typedef DirectX::Direct2D::Adapters Adapters;
			typedef DirectX::Direct2D::Mode Mode;
			typedef Adapter::Modes Modes;

			enum
			{
				NES_WIDTH      = Nes::Video::Output::WIDTH,
				NES_HEIGHT     = Nes::Video::Output::HEIGHT,
				DEFAULT_WIDTH  = 640,
				DEFAULT_HEIGHT = 480,
				DEFAULT_BPP    = 16
			};

			enum Filter
			{
				FILTER_NONE,
				FILTER_BILINEAR,
				FILTER_SCANLINES_BRIGHT,
				FILTER_SCANLINES_DARK,
				FILTER_TV_SOFT,
				FILTER_TV_HARSH,
				FILTER_2XSAI,
				FILTER_SUPER_2XSAI,
				FILTER_SUPER_EAGLE,
				FILTER_SCALE2X,
				FILTER_SCALE3X,
				FILTER_HQ2X,
				FILTER_HQ3X
			};

			Video(Managers::Emulator&,const Adapters&,const Managers::Paths&,const Configuration&);
			~Video();

			void Save(Configuration&) const;

			ibool PutTextureInVideoMemory() const;
			Modes::const_iterator GetDialogMode() const;

		private:

			struct Handlers;

			struct Settings
			{
				Settings();

				struct Rects
				{
					Rect ntsc;
					Rect pal;
				};

				enum TexMem
				{
					TEXMEM_AUTO,
					TEXMEM_VIDMEM,
					TEXMEM_SYSMEM
				};

				Adapters::const_iterator adapter;
				Filter filter;
				TexMem texMem;
				Modes::const_iterator mode;
				Rects rects;
				String::Path<false> palette;
				ibool autoHz;
			};

			ibool OnInitDialog     (Param&);
			ibool OnDestroy        (Param&);
			ibool OnHScroll        (Param&);
			ibool OnCmdDevice      (Param&);
			ibool OnCmdMode        (Param&);
			ibool OnCmdTexMem      (Param&);
			ibool OnCmdBitDepth    (Param&);
			ibool OnCmdRam         (Param&);
			ibool OnCmdColorsReset (Param&);
			ibool OnCmdPalType     (Param&);
			ibool OnCmdPalBrowse   (Param&);
			ibool OnCmdPalClear    (Param&);
			ibool OnCmdAutoHz      (Param&);
			ibool OnCmdDefault     (Param&);
			ibool OnCmdOk          (Param&);

			void UpdateColors() const;
			void UpdateDevice();
			void UpdateFilters() const;
			void UpdateBitDepth(uint);
			void UpdateResolution() const;
			void UpdateTexMem() const;
			void UpdateTexMemEnable() const;
			void UpdateRects() const;
			void UpdatePalette() const;
			void ImportPalette(Managers::Paths::Alert);
			void ValidateRects();

			void ResetDevice();
			void ResetRects();
			void ResetColors();

			Modes::const_iterator GetDefaultMode() const;

			static void UpdateScreen(HWND);

			Settings settings;
			const Adapters& adapters;
			Nes::Video nes;
			Dialog dialog;
			const Managers::Paths& paths;

		public:

			void Open()
			{
				dialog.Open();
			}

			const Rect& GetNtscRect() const
			{
				return settings.rects.ntsc;
			}

			const Rect& GetPalRect() const
			{
				return settings.rects.pal;
			}

			Filter GetFilter() const
			{
				return settings.filter;
			}

			Modes::const_iterator GetMode() const
			{
				return settings.mode;
			}

			Adapters::const_iterator GetAdapter() const
			{
				return settings.adapter;
			}

			ibool UseAutoFrequency() const
			{
				return settings.autoHz;
			}
		};
	}
}

#endif