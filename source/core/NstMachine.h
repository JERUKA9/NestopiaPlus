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

#ifndef NST_MACHINE_H
#define NST_MACHINE_H

#include "NstCpu.h"
#include "NstApu.h"
#include "NstPpu.h"
#include "NstPalette.h"
#include "NstRomDatabase.h"

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_NAMESPACE_BEGIN

class FDS;
class CARTRIDGE;
class VSSYSTEM;
class GAMEGENIE;
class NSF;
class MOVIE;

////////////////////////////////////////////////////////////////////////////////////////
// the grey box
////////////////////////////////////////////////////////////////////////////////////////

class MACHINE
{
public:

	MACHINE();
	~MACHINE();

	VOID Power(const BOOL);
	VOID Reset(const BOOL);
	VOID Pause(const BOOL);
	VOID Execute(IO::GFX* const,IO::SFX* const,IO::INPUT* const);

	PDXRESULT LoadRom  (PDXFILE&,const PDXSTRING* const);
	PDXRESULT LoadINES (PDXFILE&,const PDXSTRING* const);
	PDXRESULT LoadUNIF (PDXFILE&,const PDXSTRING* const);
	PDXRESULT LoadFDS  (PDXFILE&);
	PDXRESULT LoadNSF  (PDXFILE&);
	PDXRESULT LoadNST  (PDXFILE&);
	PDXRESULT SaveNST  (PDXFILE&) const;
	
	PDXRESULT LoadMovie    (const PDXSTRING&);
	PDXRESULT CloseMovie   ();
	PDXRESULT PlayMovie    ();
	PDXRESULT RecordMovie  ();
	PDXRESULT StopMovie    ();
	PDXRESULT RewindMovie  ();
	PDXRESULT ForwardMovie ();
	
	BOOL CanPlayMovie()    const;
	BOOL CanStopMovie()    const;
	BOOL CanRecordMovie()  const;
	BOOL CanRewindMovie()  const;
	BOOL CanForwardMovie() const;

	BOOL IsMoviePlaying()   const;
	BOOL IsMovieRecording() const;
	BOOL IsMovieRewinded()  const;

	TSIZE GetAudioLatency() const;

	const PDXSTRING* GetMovieFileName() const;

	VOID Unload();

	BOOL IsPAL       () const;
	BOOL IsOn        () const;
	BOOL IsOff       () const;
	BOOL IsPaused    () const;
	BOOL IsImage     () const;
	BOOL IsCartridge () const;
	BOOL IsFds       () const;
	BOOL IsNsf       () const;
	BOOL IsVs        () const;

	VOID GetGraphicContext (IO::GFX::CONTEXT&) const;
	VOID SetGraphicContext (const IO::GFX::CONTEXT&);
	
	VOID GetAudioContext (IO::SFX::CONTEXT&) const;
	VOID SetAudioContext (const IO::SFX::CONTEXT&);

	PDXRESULT GetFdsContext(IO::FDS::CONTEXT&) const;
	PDXRESULT SetFdsContext(const IO::FDS::CONTEXT&);

	PDXRESULT GetNsfContext(IO::NSF::CONTEXT&) const;
	PDXRESULT SetNsfContext(const IO::NSF::CONTEXT&);

	VOID GetGeneralContext(IO::GENERAL::CONTEXT&) const;
	VOID SetGeneralContext(const IO::GENERAL::CONTEXT&);

	const ROMDATABASE& GetRomDatabase() const;

	enum INTERFACETYPE
	{
		INTERFACE_GAMEGENIE
	};

	VOID* QueryInterface(const INTERFACETYPE);

	const IO::CARTRIDGE::INFO* GetCartridgeInfo() const;

	VOID ResetAudioBuffer();

	UINT GetNumVsSystemDipSwitches() const;
	PDXRESULT GetVsSystemDipSwitch(const UINT,IO::DIPSWITCH::CONTEXT&) const;
	PDXRESULT SetVsSystemDipSwitch(const UINT,const IO::DIPSWITCH::CONTEXT&);

	VOID SetMode(MODE);

	VOID AutoSelectController();
	VOID ConnectController(const UINT,const CONTROLLERTYPE);
	CONTROLLERTYPE ConnectedController(const UINT) const;

	BOOL IsAnyControllerConnected(const CONTROLLERTYPE) const;
	BOOL IsAnyControllerConnected(const CONTROLLERTYPE,const CONTROLLERTYPE) const;
	BOOL IsAnyControllerConnected(const CONTROLLERTYPE,const CONTROLLERTYPE,const CONTROLLERTYPE) const;
	BOOL IsAnyControllerConnected(const CONTROLLERTYPE,const CONTROLLERTYPE,const CONTROLLERTYPE,const CONTROLLERTYPE) const;

	enum
	{
		NUM_CONTROLLER_PORTS = 5
	};

	class CONTROLLER
	{
	public:

		CONTROLLER(const UINT n,PPU* const p=NULL)
		: port(n), ppu(p), strobe(0) { Reset(); }

		virtual ~CONTROLLER() {}

		inline VOID BeginFrame(IO::INPUT* const i,const IO::GFX* const g)
		{
			input = i;
			gfx = g;
		}

		virtual VOID Initialize(const ULONG) {}
		virtual VOID Poll() {}		
		virtual VOID Reset() {}

		virtual UINT Peek_4016() { return 0x00; }
		virtual UINT Peek_4017() { return 0x00; }

		virtual VOID Poke_4016 (const UINT) {}
		virtual VOID Poke_4017 (const UINT) {}

		virtual ULONG GetState() const { return 0; }
		virtual VOID SetState(const ULONG) {};

		virtual CONTROLLERTYPE Type() const
		{ return CONTROLLER_UNCONNECTED; }

	protected:

		inline BOOL SetStrobe(UINT data)
		{
			data &= 0x1;
			const BOOL reset = strobe && !data;
			strobe = data;
			return reset;
		}

		const IO::GFX* gfx;
		PPU* const ppu;

		const UINT port;
		IO::INPUT* input;

		UINT strobe;
	};

private:

	VOID InitializeControllers();

	NES_DECL_POKE( 4016 );
	NES_DECL_PEEK( 4016 );
	NES_DECL_POKE( 4017 );					    
	NES_DECL_PEEK( 4017 );

	CARTRIDGE* cartridge;
	CONTROLLER* controller[4];
	CONTROLLER* expansion;
	VSSYSTEM* VsSystem;
	PALETTE palette;
	GAMEGENIE* const GameGenie;
	FDS* fds;
	NSF* nsf;
	BOOL on;
	BOOL paused;
	MODE mode;
	IO::GENERAL::CONTEXT GeneralContext;
	MOVIE* movie;
	APU& apu;
	CPU cpu;
	PPU ppu;
	const ROMDATABASE RomDatabase;
};

#include "NstMachine.inl"

NES_NAMESPACE_END

#endif
