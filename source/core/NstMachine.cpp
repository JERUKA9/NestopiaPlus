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

#include "../paradox/PdxFile.h"
#include "NstTypes.h"
#include "NstMap.h"
#include "NstCpu.h"
#include "NstApu.h"
#include "NstPpu.h"
#include "NstFds.h"
#include "NstPalette.h"
#include "mapper/NstMappers.h"
#include "NstCartridge.h"
#include "vssystem/NstVsSystem.h"
#include "NstGameGenie.h"
#include "NstNsf.h"
#include "NstMachine.h"
#include "input/NstPad.h"
#include "input/NstZapper.h"
#include "input/NstPaddle.h"
#include "input/NstPowerPad.h"
#include "input/NstKeyboard.h"
#include "NstIps.h"
#include "NstMovie.h"
  
NES_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

MACHINE::MACHINE()
: 
cpu       (new CPU),
apu       (cpu->GetAPU()),
ppu       (new PPU(cpu)),
fds       (NULL),
palette   (new PALETTE),
cartridge (NULL),
on        (FALSE),
paused    (FALSE),
mode      (MODE_AUTO),
expansion (NULL),
GameGenie (NULL),
VsSystem  (NULL),
nsf       (NULL),
movie     (NULL)
{
	controller[0] = new PAD(0,0);
	controller[1] = new PAD(1,1);
	controller[2] = new CONTROLLER(2);
	controller[3] = new CONTROLLER(3);
}
										  
////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

MACHINE::~MACHINE()
{
	delete movie;
	delete expansion;
	delete controller[0];
	delete controller[1];
	delete controller[2];
	delete controller[3];
	delete nsf;
	delete GameGenie;
	delete palette;
	delete VsSystem;
	delete cartridge;
	delete fds;
	delete ppu;
	delete cpu;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_POKE(MACHINE,4016)
{
	if (expansion)
	{
		expansion->Poke_4016( data );
	}
	else
	{
		controller[0]->Poke_4016( data );
		controller[1]->Poke_4016( data );
		controller[2]->Poke_4016( data );
		controller[3]->Poke_4016( data );
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_POKE(MACHINE,4017)
{
	if (expansion)
	{
		expansion->Poke_4017( data );
	}
	else
	{
		controller[0]->Poke_4017( data );
		controller[1]->Poke_4017( data );
		controller[2]->Poke_4017( data );
		controller[3]->Poke_4017( data );
	}

	cpu->Poke_4017( data );
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_PEEK(MACHINE,4016)
{
	if (expansion)
	{
		return 
		(
    		expansion->Peek_4016() |
			(cpu->GetCache() & 0xC0)
		);
	}
	else
	{
		return 
		(
        	controller[0]->Peek_4016() | 
			controller[1]->Peek_4016() |
			controller[2]->Peek_4016() |
			controller[3]->Peek_4016() |
			(cpu->GetCache() & 0xC0)
		);
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

NES_PEEK(MACHINE,4017)
{
	if (expansion)
	{
		return 
		(
			expansion->Peek_4017() |
			(cpu->GetCache() & 0xC0)
		);
	}
	else
	{
		return
		(
         	controller[0]->Peek_4017() | 
			controller[1]->Peek_4017() | 
			controller[2]->Peek_4017() | 
			controller[3]->Peek_4017() |
			cpu->Peek_4017()
		);
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID MACHINE::GetGraphicContext(IO::GFX::CONTEXT& context) const
{ 
	ppu->GetContext(context); 
}

VOID MACHINE::SetGraphicContext(const IO::GFX::CONTEXT& context)
{
	PALETTE::TYPE type;

	switch (context.PaletteMode)
	{
    	case IO::GFX::PALETTE_EMULATED: type = PALETTE::EMULATE;  break;
		case IO::GFX::PALETTE_CUSTOM:   type = PALETTE::CUSTOM;   if (context.palette) break;
		case IO::GFX::PALETTE_INTERNAL: type = PALETTE::INTERNAL; break;
	}

	palette->SetHue        ( context.hue        );
	palette->SetBrightness ( context.brightness );
	palette->SetSaturation ( context.saturation );
	palette->SetType       ( type               );
	palette->Import        ( context.palette    );

	ppu->SetContext( context ); 
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID MACHINE::SetAudioContext(const IO::SFX::CONTEXT& context)
{ 
	apu->SetContext(context); 
}

VOID MACHINE::GetAudioContext(IO::SFX::CONTEXT& context) const
{ 
	apu->GetContext(context); 
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

PDXRESULT MACHINE::SetGameGenieContext(const IO::GAMEGENIE::CONTEXT& context)
{
	if (!GameGenie)
		GameGenie = new GAMEGENIE(cpu);

	return GameGenie->SetContext( context );
}

PDXRESULT MACHINE::GetGameGenieContext(IO::GAMEGENIE::CONTEXT& context)
{
	if (!GameGenie)
		GameGenie = new GAMEGENIE(cpu);

	return GameGenie->GetContext( context );
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

PDXRESULT MACHINE::GetFdsContext(IO::FDS::CONTEXT& context) const
{
	if (fds)
	{
		return fds->GetContext( context );
	}
	else
	{
		context.Reset();
		context.bios = NULL;
		context.WriteProtected = FDS::IsWriteProtected();
	}

	return PDX_OK;
}

PDXRESULT MACHINE::SetFdsContext(const IO::FDS::CONTEXT& context)
{
	if (fds)
	{
		return fds->SetContext( context );
	}
	else
	{
		if (context.bios)
			FDS::SetBIOS( *context.bios );

		FDS::SetWriteProtection( context.WriteProtected );
	}

	return PDX_OK;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

PDXRESULT MACHINE::GetNsfContext(IO::NSF::CONTEXT& context) const
{
	return nsf ? nsf->GetContext(context) : PDX_FAILURE;
}

PDXRESULT MACHINE::SetNsfContext(const IO::NSF::CONTEXT& context)
{
	return nsf && PDX_SUCCEEDED(nsf->SetContext(context)) ? PDX_OK : PDX_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID MACHINE::SetGeneralContext(const IO::GENERAL::CONTEXT& context)
{
	GeneralContext = context;
}

VOID MACHINE::GetGeneralContext(IO::GENERAL::CONTEXT& context) const
{
	context = GeneralContext;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

const IO::CARTRIDGE::INFO* MACHINE::GetCartridgeInfo() const
{
	return cartridge ? &cartridge->GetInfo() : NULL;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

UINT MACHINE::GetNumVsSystemDipSwitches() const
{ 
	return 
	(
      	VsSystem ? VsSystem->NumDipSwitches() : 
     	(cartridge ? cartridge->Mapper()->NumDipSwitches() : 0)
	); 
}

PDXRESULT MACHINE::GetVsSystemDipSwitch(const UINT index,IO::DIPSWITCH::CONTEXT& context) const
{
	if (VsSystem)
	{
		VsSystem->GetDipSwitch( index, context );
		return PDX_OK;
	}

	if (cartridge && cartridge->Mapper()->NumDipSwitches())
	{
		cartridge->Mapper()->GetDipSwitch( index, context );
		return PDX_OK;
	}

	return PDX_FAILURE;
}

PDXRESULT MACHINE::SetVsSystemDipSwitch(const UINT index,const IO::DIPSWITCH::CONTEXT& context)
{
	if (VsSystem)
	{
		VsSystem->SetDipSwitch( index, context );
		return PDX_OK;
	}

	if (cartridge && cartridge->Mapper()->NumDipSwitches())
	{
		cartridge->Mapper()->SetDipSwitch( index, context );
		return PDX_OK;
	}

	return PDX_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

BOOL MACHINE::IsPAL() const
{
	return 
	(
     	(mode == MODE_PAL) || 
    	(mode == MODE_AUTO && cartridge && cartridge->GetInfo().system == SYSTEM_PAL) ||
		(mode == MODE_AUTO && nsf && nsf->IsPAL())
	);
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

PDXRESULT MACHINE::Load(PDXFILE& file)
{
	if (file.Readable(sizeof(U32)))
	{
		switch (file.Peek<U32>())
		{
     		case 0x1A53454EUL: return LoadINES  ( file );
			case 0x46494E55UL: return LoadUNIF  ( file );
			case 0x1A534446UL: return LoadFDS   ( file );
    		case 0x4D53454EUL: return LoadNSF   ( file );
			case 0x43544150UL: return LoadIPS   ( file );
			case 0x1A54534EUL: return LoadNST   ( file );
			case 0x1A564D4EUL: return LoadMovie ( file.Name() );
		}
	}

	return MsgWarning("Unsupported file format");
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID MACHINE::Unload()
{
	if (on)
	{
		on = FALSE;
		LogOutput("MACHINE: power off");
	}

	if (cartridge || fds || nsf)
		LogOutput("MACHINE: file unloaded");

	paused = FALSE;

	delete fds;
	fds = NULL;

	delete nsf;      
	nsf = NULL;

	delete VsSystem;  
	VsSystem = NULL;

	delete cartridge;
	cartridge = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

PDXRESULT MACHINE::LoadINES(PDXFILE& file)
{
	Unload();

	RecentImage = file.Name();

	{
		PDXSTRING log;
		
		log  = "MACHINE: Loading NES file \"";
		log	+= file.Name();
		log += "\"";
		
		LogOutput( log );
	}

	cartridge = new CARTRIDGE;

	if (GeneralContext.ApplyIPS)
		ApplyIPS( file );

	if (PDX_FAILED(cartridge->Load( file, cpu, ppu, GeneralContext )))
	{
		Unload();
		return PDX_FAILURE;
	}

	if (cartridge->GetInfo().system == SYSTEM_VS)
	{
		if (!(VsSystem = VSSYSTEM::New( cpu, ppu, cartridge->GetInfo().pRomCrc )))
		{
			Unload();
			return PDX_FAILURE;
		}
	}

	return PDX_OK;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

PDXRESULT MACHINE::LoadUNIF(PDXFILE& file)
{
	Unload();

	RecentImage = file.Name();

	{
		PDXSTRING log;

		log  = "MACHINE: Loading UNIF file \"";
		log	+= file.Name();
		log += "\"";

		LogOutput( log );
	}

	cartridge = new CARTRIDGE;

 #ifdef NES_AUTO_IPS_ON_UNIF // IPS patching is not that useful for UNIF files so default is off

	if (GeneralContext.ApplyIPS)
		ApplyIPS( file );

 #endif

	if (PDX_FAILED(cartridge->Load( file, cpu, ppu, GeneralContext )))
	{
		Unload();
		return PDX_FAILURE;
	}

	if (cartridge->GetInfo().system == SYSTEM_VS)
	{
		if (!(VsSystem = VSSYSTEM::New( cpu, ppu, cartridge->GetInfo().pRomCrc )))
		{
			Unload();
			return PDX_FAILURE;
		}
	}

	return PDX_OK;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

PDXRESULT MACHINE::LoadFDS(PDXFILE& file)
{
	Unload();

	RecentImage = file.Name();

	{
		PDXSTRING log;

		log  = "MACHINE: Loading FDS file \"";
		log	+= file.Name();
		log += "\"";

		LogOutput( log );
	}

	fds = new FDS(cpu,ppu);

	if (GeneralContext.ApplyIPS)
		ApplyIPS( file );

	if (PDX_FAILED(fds->Load( file )))
	{
		Unload();
		return PDX_FAILURE;
	}

	return PDX_OK;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

PDXRESULT MACHINE::LoadNSF(PDXFILE& file)
{
	Unload();

	{
		PDXSTRING log;

		log  = "MACHINE: Loading NSF file \"";
		log	+= file.Name();
		log += "\"";

		LogOutput( log );
	}

	nsf = new NSF(cpu);

	if (PDX_FAILED(nsf->Load( file )))
	{
		Unload();
		return PDX_FAILURE;
	}

	return PDX_OK;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

PDXRESULT MACHINE::LoadIPS(PDXFILE& IpsFile)
{
	PDX_ASSERT( IpsFile.IsOpen() );

	const BOOL loaded = cartridge || fds;

	Unload();

	PDXFILE ImageFile( PDXFILE::INPUT );

	const PDXSTRING backup( RecentImage );

	RecentImage = IpsFile.Name();
	RecentImage.ReplaceFileExtension( "nes" );

	if (PDX_FAILED(ImageFile.Open( RecentImage )))
	{
		RecentImage.ReplaceFileExtension( "fds" );

		if (PDX_FAILED(ImageFile.Open( RecentImage )))
		{
			if (GeneralContext.RomPath.IsEmpty())
				return MsgWarning("Couldn't find any matching image file!");

			RecentImage.ReplaceFilePath( GeneralContext.RomPath );

			if (PDX_FAILED(ImageFile.Open( RecentImage )))
			{
				RecentImage.ReplaceFileExtension( "nes" );

				if (PDX_FAILED(ImageFile.Open( RecentImage )))
					RecentImage = backup;
			}
		}
	}

	if (RecentImage.IsEmpty() || (!ImageFile.IsOpen() && PDX_FAILED(ImageFile.Open( RecentImage ))))
		return MsgWarning( loaded ? "Couldn't open image file!" : "Couldn't find any matching image file!");

	PDX_TRY(ApplyIPS( ImageFile, IpsFile ));

	const BOOL ApplyIPS = GeneralContext.ApplyIPS;
	GeneralContext.ApplyIPS = FALSE;

	const PDXRESULT result = Load( ImageFile );
	
	GeneralContext.ApplyIPS = ApplyIPS;

	return result;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID MACHINE::ApplyIPS(PDXFILE& ImageFile)
{
	PDXSTRING IpsName( ImageFile.Name() );
	IpsName.ReplaceFileExtension( "ips" );

	PDXFILE IpsFile( PDXFILE::INPUT );

	BOOL ok = FALSE;

	if ((GeneralContext.LookInRomPathForIps || GeneralContext.IpsPath.IsEmpty()) && PDX_SUCCEEDED(IpsFile.Open( IpsName )))
	{
		ok = TRUE;
	}
	else if (GeneralContext.IpsPath.Size())
	{
		IpsName.ReplaceFilePath( GeneralContext.IpsPath );
		ok = PDX_SUCCEEDED(IpsFile.Open( IpsName ));
	}

	if (ok)
	{
		ApplyIPS( ImageFile, IpsFile );
	}
	else
	{
		LogOutput("MACHINE: found no matching IPS file");
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

PDXRESULT MACHINE::ApplyIPS(PDXFILE& ImageFile,PDXFILE& IpsFile)
{
	PDXSTRING log;

	if (PDX_FAILED(IPS::Load( ImageFile, IpsFile )))
	{
		log  = "MACHINE: warning, IPS file \"";
		log += IpsFile.Name();
		log += "\" is either invalid or corrupt";
	
		LogOutput( log );

		if (!GeneralContext.DisableWarnings)
			MsgWarning("Invalid or corrupt IPS file..");

		return PDX_FAILURE;
	}

	log  = "MACHINE: file \"";
	log += ImageFile.Name();
	log += "\" was succesfully patched by IPS file \"";
	log += IpsFile.Name();
	log += "\"";

	LogOutput( log );

	return PDX_OK;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

PDXRESULT MACHINE::LoadNST(PDXFILE& file)
{
	if (!on || (!cartridge && !fds))
		return PDX_FAILURE;

	if (!file.Readable(sizeof(U32) * 2) || file.Read<U32>() != 0x1A54534EUL)
		return MsgWarning( "invalid save state file!" );

	if (file.Read<U32>() != (cartridge ? cartridge->GetInfo().crc : 0))
	{
		static const CHAR wrn[] = 
		(
     		"Save State Loading!"
		);

		static const CHAR msg[] = 
		(
     		"The current rom image file checksum doesn't match the state file's saved value. "
			"Do you still want to load it?"
		);

		if (!MsgQuestion( wrn, msg ))
			return PDX_FAILURE;

		if (MsgQuestion( wrn, "Do you want to repair it?" ))
		{
			const TSIZE pos = file.Position() - sizeof(U32);
			PDXFILE FixFile;

			if (PDX_SUCCEEDED(FixFile.Open( file.Name(), PDXFILE::APPEND )))
			{
				FixFile.Seek( PDXFILE::BEGIN, pos );
				FixFile << U32(cartridge->GetInfo().crc);
			}
			else
			{
				MsgWarning( "Couldn't repair the file!" );
			}
		}
	}

	PDX_TRY( cpu->LoadState( file ));
	PDX_TRY( apu->LoadState( file ));
	PDX_TRY( ppu->LoadState( file ));

	if (!file.Readable(sizeof(U8)))
		return MsgWarning("Corrupt data!");

	if (cartridge)
	{
		if (file.Read<U8>() != 0x88 || PDX_FAILED(cartridge->Mapper()->LoadState(file)))
			return MsgWarning("Corrupt data!");
	}
	else if (fds)
	{
		if (file.Read<U8>() != 0x99 || PDX_FAILED(fds->LoadState(file)))
			return MsgWarning("Corrupt data!");
	}

	return PDX_OK;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

PDXRESULT MACHINE::SaveNST(PDXFILE& file) const
{
	if (!on || (!cartridge && !fds))
		return PDX_FAILURE;

	file << U32(0x1A54534EUL);
	file << U32(cartridge ? cartridge->GetInfo().crc : 0);

	PDX_TRY( cpu->SaveState( file ));
	PDX_TRY( apu->SaveState( file ));
	PDX_TRY( ppu->SaveState( file ));

	if (cartridge)
	{
		file << U8(0x88);
		PDX_TRY(cartridge->Mapper()->SaveState(file));
	}
	else if (fds)
	{
		file << U8(0x99);
		PDX_TRY(fds->SaveState(file));
	}

	return PDX_OK;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

PDXRESULT MACHINE::SaveMovie(const CHAR* const FileName)
{
	if (!on || (!cartridge && !fds))
		return PDX_FAILURE;

	if (!movie)
		movie = new MOVIE(this);

	if (PDX_FAILED(movie->Save( FileName )))
	{
		CloseMovie();
		return PDX_FAILURE;
	}
	
	InitializeControllers();

	return PDX_OK;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

PDXRESULT MACHINE::LoadMovie(const CHAR* const FileName)
{
	if (!on || (!cartridge && !fds))
		return PDX_FAILURE;

	if (!movie)
		movie = new MOVIE(this);
	
	if (PDX_FAILED(movie->Load( FileName )))
	{
		CloseMovie();
		return PDX_FAILURE;
	}
  
	InitializeControllers();
	
	return PDX_OK;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

const PDXSTRING* MACHINE::GetMovieFileName() const
{
	return movie && movie->FileName().Length() ? &movie->FileName() : NULL;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

PDXRESULT MACHINE::StartMovie()
{
	if (movie)
	{
		movie->Start();
		return PDX_OK;
	}
  
	return PDX_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

PDXRESULT MACHINE::StopMovie()
{
	if (movie)
	{
		movie->Stop();
		return PDX_OK;
	}
  
	return PDX_OK;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

PDXRESULT MACHINE::RewindMovie()
{
	if (movie)
	{
		movie->Rewind();
		return PDX_OK;
	}

	return PDX_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

PDXRESULT MACHINE::CloseMovie()
{
	delete movie;
	movie = NULL;
	return PDX_OK;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

BOOL MACHINE::IsMoviePlaying()   const { return on && (cartridge || fds) && movie && movie->IsPlaying();   }
BOOL MACHINE::IsMovieRecording() const { return on && (cartridge || fds) && movie && movie->IsRecording(); }

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

PDXRESULT MACHINE::Pause(const BOOL state)
{
	paused = state;
	return PDX_OK;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

PDXRESULT MACHINE::Power(const BOOL state)
{
	if (state)
	{
		if (!on)
		{
			LogOutput("MACHINE: power on");
			on = TRUE;
			Reset( TRUE );	
		}
	}
	else
	{
		if (on)
		{
			LogOutput("MACHINE: power off");
			on = FALSE;

		}
	}

	return PDX_OK;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

PDXRESULT MACHINE::Execute(IO::GFX* const gfx,IO::SFX* const sfx,IO::INPUT* const input)
{
	if (on && !paused)
	{
		if (nsf)
		{
			apu->BeginFrame( sfx );
			nsf->Execute();
		}
		else
		{
			if (gfx)
			{
				gfx->PaletteChanged = palette->Update();
				gfx->palette = palette->GetData();
			}

			if (VsSystem)
				VsSystem->SetContext( input );

			for (UINT i=0; i < 4; ++i)
			{
				controller[i]->SetContext( input, gfx ); 
				controller[i]->Poll();
			}

			if (expansion)
			{
				expansion->SetContext( input, gfx );
				expansion->Poll();
			}

			if (movie)
			{
				if (PDX_FAILED(movie->ExecuteFrame()))
					CloseMovie();
			}

			ppu->BeginFrame( gfx );
			apu->BeginFrame( sfx );
			cpu->Execute();
			ppu->EndFrame();

			if (cartridge)
			{
				cartridge->Mapper()->VSync();
			}
			else if (fds)
			{
				fds->VSync();
			}
		}

		return PDX_OK;
	}

	return PDX_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID MACHINE::SetMode(MODE m)
{
	if (mode != m)
	{
		mode = m;

		if (m == MODE_AUTO)
			m = (cartridge && cartridge->GetInfo().system == SYSTEM_PAL) || (nsf && nsf->IsPAL()) ? MODE_PAL : MODE_NTSC;

		cpu->SetMode( m );
		ppu->SetMode( m );

		if (nsf)
			nsf->SetMode( m );
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID MACHINE::AutoSelectController()
{
	if (cartridge)
	{
		ConnectController( 0, cartridge->GetInfo().controllers[0] );
		ConnectController( 1, cartridge->GetInfo().controllers[1] );
		ConnectController( 2, cartridge->GetInfo().controllers[2] );
		ConnectController( 3, cartridge->GetInfo().controllers[3] );
		ConnectController( 4, cartridge->GetInfo().controllers[4] );
	}
	else
	{
		ConnectController( 0, CONTROLLER_PAD1        );
		ConnectController( 1, CONTROLLER_PAD2        );
		ConnectController( 2, CONTROLLER_UNCONNECTED );
		ConnectController( 3, CONTROLLER_UNCONNECTED );
		ConnectController( 4, CONTROLLER_UNCONNECTED );
	}

	InitializeControllers();
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID MACHINE::ConnectController(const UINT port,const CONTROLLERTYPE type)
{
	PDX_ASSERT(port < 5);

	if (port < 4)
	{
		if (controller[port]->Type() == type)
			return;

		delete controller[port];
		controller[port] = NULL;
	}
	else
	{
		if (bool(expansion) == bool(type == CONTROLLER_KEYBOARD))
			return;

		delete expansion;
		expansion = NULL;
	}

	switch (port)
	{
     	case 0:
		case 1:

			switch (type)
			{
     			case CONTROLLER_ZAPPER:   controller[port] = new ZAPPER(port,ppu); break;
     			case CONTROLLER_PADDLE:   controller[port] = new PADDLE(port);     break;
     			case CONTROLLER_POWERPAD: controller[port] = new POWERPAD(port);   break;
			}

		case 2:
		case 3:

			switch (type)
			{
       			case CONTROLLER_UNCONNECTED: controller[port] = new CONTROLLER(port); break;
     			case CONTROLLER_PAD1:        controller[port] = new PAD(port,0);      break;
     			case CONTROLLER_PAD2:        controller[port] = new PAD(port,1);      break;
     			case CONTROLLER_PAD3:        controller[port] = new PAD(port,2);      break;
     			case CONTROLLER_PAD4:        controller[port] = new PAD(port,3);      break;
			}
			break;

    	case 4:

			switch (type)
			{
     			case CONTROLLER_KEYBOARD: expansion = new FAMILYBASIC();
       			case CONTROLLER_UNCONNECTED: break;
			}
			break;
	}

	PDX_ASSERT(controller[0] && controller[1] && controller[2] && controller[3]);

	InitializeControllers();
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

VOID MACHINE::InitializeControllers()
{
	if (cartridge)
	{
		const ULONG crc = cartridge->GetInfo().pRomCrc;

		controller[0]->Initialize( crc );
		controller[1]->Initialize( crc );
		controller[2]->Initialize( crc );
		controller[3]->Initialize( crc );

		if (expansion)
			expansion->Initialize( crc );
	}

	if (movie)
	{
		movie->RemoveInputDevices();
		movie->AddInputDevice( controller[0] );
		movie->AddInputDevice( controller[1] );
		movie->AddInputDevice( controller[2] );
		movie->AddInputDevice( controller[3] );

		if (expansion)
			movie->AddInputDevice( expansion );
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////

PDXRESULT MACHINE::Reset(const BOOL hard)
{
	if (on)
	{
		if (nsf)
		{
			cpu->ResetPorts();
			cpu->Reset( TRUE );
			nsf->Reset();
		}
		else
		{
			if (movie)
			{
				StopMovie();
				CloseMovie();
			}

			InitializeControllers();

			cpu->SetPort( 0x4016, this, Peek_4016, Poke_4016 );
			cpu->SetPort( 0x4017, this, Peek_4017, Poke_4017 );

			for (UINT i=0; i < 4; ++i)
				controller[i]->Reset();

			ppu->Reset( hard );

			if (cartridge)
			{
				cartridge->Reset( hard );
			}
			else if (fds) 
			{
				fds->Reset( hard );
			}
			else
			{
				cpu->ResetPorts();
			}

			cpu->Reset( hard );

			if (VsSystem)
				VsSystem->Reset( hard );

			if (GameGenie)
				GameGenie->Reset();

			if (cartridge && cartridge->GetInfo().pRomCrc == 0x885ACC2BUL)
			{
				// bad dump of SMB1, but I'm willing to help..
				cpu->ClearRAM();
			}
		}

		if (mode == MODE_AUTO)
		{
			const MODE m = IsPAL() ? MODE_PAL : MODE_NTSC;

			cpu->SetMode( m );
			ppu->SetMode( m );

			if (nsf)
				nsf->SetMode( m );

			PDXSTRING log;
			
			log  = "MACHINE: auto-changed to ";
			log += (m == MODE_NTSC ? "NTSC" : "PAL");
			
			LogOutput( log );
		}
	}

	return PDX_OK;
}

NES_NAMESPACE_END
