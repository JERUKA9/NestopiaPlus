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

#ifndef NST_VIDEO_RENDERER_H
#define NST_VIDEO_RENDERER_H

#ifdef NST_PRAGMA_ONCE_SUPPORT
#pragma once
#endif

namespace Nes
{
	namespace Core
	{
		namespace Video
		{
			class Renderer
			{
				typedef Api::Video::RenderState RenderState;
				typedef Api::Video::Decoder Decoder;

			public:
		
				Renderer();
				~Renderer();

				enum PaletteType
				{
					PALETTE_YUV,
					PALETTE_RGB,
					PALETTE_CUSTOM
				};

				enum
				{
					WIDTH = 256,
					HEIGHT = 240,
					PIXELS = dword(WIDTH) * HEIGHT,
					PALETTE = 64 * 8,
					DEFAULT_HUE = 128,
					DEFAULT_BRIGHTNESS = 128,
					DEFAULT_SATURATION = 128,
					DEFAULT_CONTRAST = 128,
					DEFAULT_SHARPNESS = 128,
					DEFAULT_PALETTE = PALETTE_YUV
				};

				Result SetState(const RenderState&);
				Result GetState(RenderState&) const;
				void Blit(Output&,uint=1);

				void SetMode(Mode);
				Result SetDecoder(const Decoder&);

				Result SetPaletteType(PaletteType);
				Result LoadCustomPalette(const u8 (*)[3]);
				void   ResetCustomPalette();

				void EnableFieldMerging(bool);

				typedef u8 PaletteEntries[PALETTE][3];
				typedef u16 Screen[PIXELS];

				const PaletteEntries& GetPalette();

			private:

				void UpdateFilter();
				Result SetLevel(u8&,u8);

				class Palette
				{
				public:

					Palette();
					~Palette();

					Result SetType(PaletteType);
					Result LoadCustom(const u8 (*)[3]);
					bool   ResetCustom();
					void   Update(uint,uint,uint);
					Result SetDecoder(const Decoder&);

					inline const PaletteEntries& Get() const;

				private:

					enum
					{
						HUE_OFFSET = 33
					};

					struct Custom
					{
						u8 palette[64][3];
					};

					void Generate(int,int,int);
					void Build(int,int,int);

					static void ToPAL(const double (&)[3],u8 (&)[3]);
					static void ToHSV(double,double,double,double&,double&,double&);
					static void ToRGB(double,double,double,double&,double&,double&);

					PaletteType type;						
					Custom* custom;
					Decoder decoder;
					u8 palette[64*8][3];

					static const u8 rgbPalette[64][3];

				public:

					PaletteType GetType() const
					{
						return type;
					}

					const Decoder& GetDecoder() const
					{
						return decoder;
					}
				};

				struct Input
				{
					u16 screen[PIXELS];
					u32 palette[PALETTE];
				};

				class FilterNone;
				class FilterScanlines;

                #ifndef NST_NO_2XSAI
				class Filter2xSaI;
                #endif

                #ifndef NST_NO_SCALE2X
				class FilterScaleX;
                #endif

                #ifndef NST_NO_HQ2X
				class FilterHqX;
                #endif

                #ifndef NST_NO_NTSCVIDEO
				template<uint BITS> class FilterNtsc;
                #endif

				class NST_NO_VTABLE Filter
				{
					struct Format
					{
						Format(const RenderState::Bits::Mask&);

						dword left[3];
						dword right[3];
					};

				protected:

					template<uint BITS>
					struct OutPixel
					{
						typedef u16 Type;
					};

				public:

					Filter(const RenderState&);
					virtual ~Filter() {}

					virtual void Blit(const Input&,const Output&,uint) = 0;
					virtual void Transform(const u8 (&)[PALETTE][3],u32 (&)[PALETTE]) const;
					virtual bool CanTransform() const { return true; }

					const uint bpp;
					const Format format;
				};

				struct State
				{
					State();

					enum
					{
						UPDATE_PALETTE = 0x1,
						UPDATE_FILTER = 0x2,
						FIELD_MERGING_USER = 0x1,
						FIELD_MERGING_PAL = 0x2
					};

					RenderState::Filter filter;
					u16 width;
					u16 height;
					u8 update;
					u8 brightness;
					u8 saturation;
					u8 hue;
					u8 contrast;
					u8 sharpness;
					u8 scanlines;
					u8 fieldMerging;
					RenderState::Bits::Mask mask;
				};

				Filter* filter;
				State state;
				Input input;
				Palette palette;
				Decoder decoder;

			public:

				Result SetBrightness(u8 brightness)
				{
					return SetLevel( state.brightness, brightness );
				}

				Result SetSaturation(u8 saturation)
				{
					return SetLevel( state.saturation, saturation );
				}

				Result SetHue(u8 hue)
				{
					return SetLevel( state.hue, hue );
				}

				Result SetContrast(u8 contrast)
				{
					return SetLevel( state.contrast, contrast );
				}

				Result SetSharpness(u8 sharpness)
				{
					return SetLevel( state.sharpness, sharpness );
				}

				uint GetBrightness() const
				{
					return state.brightness;
				}

				uint GetSaturation() const
				{
					return state.saturation;
				}

				uint GetHue() const
				{
					return state.hue;
				}

				uint GetContrast() const
				{
					return state.contrast;
				}

				uint GetSharpness() const
				{
					return state.sharpness;
				}

				bool IsFieldMergingEnabled() const
				{
					return state.fieldMerging & State::FIELD_MERGING_USER;
				}

				PaletteType GetPaletteType() const
				{
					return palette.GetType();
				}

				const Decoder& GetDecoder() const
				{
					return palette.GetDecoder();
				}
  
				Screen& GetScreen()
				{
					return input.screen;
				}

				bool IsReady() const
				{
					return filter != NULL;
				}
			};

			template<>
			struct Renderer::Filter::OutPixel<32U>
			{
				typedef u32 Type;
			};
		}
	}
}

#endif
