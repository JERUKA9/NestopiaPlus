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

#ifndef NST_MANAGER_SAVESTATES_H
#define NST_MANAGER_SAVESTATES_H

#pragma once

namespace Nestopia
{
	namespace Window
	{
		class AutoSaver;
		class Main;
	}

	namespace Managers
	{
		class SaveStates
		{
		public:

			SaveStates(Emulator&,const Configuration&,Window::Menu&,const Paths&,const Window::Main&);
			~SaveStates();

			void Load(Collection::Buffer&,String::Generic=String::Generic()) const;

		private:

			typedef Collection::Buffer Slot;

			enum				
			{
				SLOT_1,
				SLOT_2,
				SLOT_3,
				SLOT_4,
				SLOT_5,
				SLOT_6,
				SLOT_7,
				SLOT_8,
				SLOT_9,
				SLOT_NONE = SLOT_9,
				NUM_SLOTS
			};

			void ImportSlots();
			void ExportSlot(uint);

			void SaveToSlot(uint,ibool=TRUE);
			void LoadFromSlot(uint,ibool=TRUE);

			void ToggleAutoSaver(ibool);

			void OnEmuEvent(Emulator::Event);			
			ibool OnTimerAutoSave();

			void OnCmdStateLoad        (uint);
			void OnCmdStateSave        (uint);
			void OnCmdSlotSave         (uint);
			void OnCmdNextSlotSave     (uint);
			void OnCmdSlotLoad         (uint);
			void OnCmdLastSlotLoad     (uint);
			void OnCmdAutoSaverOptions (uint);
			void OnCmdAutoSaverStart   (uint);

			Emulator& emulator;
			Window::Menu& menu;
			const Window::Main& window;
			const Paths& paths;
			uint lastSaveSlot;
			ibool autoSaveEnabled;
			Object::Heap<Window::AutoSaver> autoSaver;
			Slot slots[NUM_SLOTS];
		};
	}
}

#endif
