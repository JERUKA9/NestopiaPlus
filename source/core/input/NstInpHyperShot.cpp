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

#include "NstInpDevice.hpp"
#include "NstInpHyperShot.hpp"

namespace Nes
{
	namespace Core
	{
		namespace Input
		{
            #ifdef NST_PRAGMA_OPTIMIZE
            #pragma optimize("s", on)
            #endif
	
			HyperShot::HyperShot()
			: Device(Api::Input::HYPERSHOT)
			{
				Reset();
			}
	
			void HyperShot::Reset()
			{
				strobe = 0;
				stream = 0;
			}
	
			void HyperShot::SaveState(State::Saver& state,const uchar id) const
			{
				state.Begin('H','S',id,'\0').Write8( strobe ).End();
			}

			void HyperShot::LoadState(State::Loader& state,const dword id)
			{
				if (id == NES_STATE_CHUNK_ID('H','S','\0','\0'))
					strobe = state.Read8() & 0x1;
			}

            #ifdef NST_PRAGMA_OPTIMIZE
            #pragma optimize("", on)
            #endif
	
			void HyperShot::BeginFrame(Controllers* i)
			{
				input = i;
				state = 0;
			}

			void HyperShot::Poke(const uint data)
			{
				const uint prev = strobe;
				strobe = data & 0x1;

				if (prev > strobe)
				{
					if (input)
					{
						Controllers::HyperShot& hyperShot = input->hyperShot;
						input = NULL;

						if (Controllers::HyperShot::callback( hyperShot ))
							state = hyperShot.buttons & b00011110;
					}

					stream = state;
				}
			}
	
			uint HyperShot::Peek(const uint port)
			{
				return port ? stream : 0;
			}
		}
	}
}
