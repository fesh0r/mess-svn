// dgPIX 'VRender 2 Beta Rev4' hardware
// MEDIAGX CPU + 3dFX VooDoo chipset

/***************************************************************************

Funky Ball
dgPIX, 1998

PCB Layout
----------

VRender 2Beta Rev4
  |--------------------------------------------------------------------|
  |TDA2005  14.31818MHz       |---------|   KM416C254 KM416C254        |
  |VOL KDA0340D    |-------|  |3DFX     |                              |
  |4558            |GENDAC |  |500-0004-02                             |
  |                |ICS5342|  |F004221.1|                              |
  |                |       |  |TMU      |                    KM416S1020|
  |                |-------|  |---------|   KM416C254 KM416C254        |
  |SERVICE_SW |---SUB---|                                              |
  |           |         |                                              |
  |           |FLASH.U3 |     |-----------|                            |
|-|           |         |     |3DFX       |                  KM416S1020|
|             |         |     |500-0003-03| KM416C254 KM416C254        |
|             |         |     |F006531.1  |                            |
|             |         |     |FBI        |                            |
|J            |         |     |           | KM416C254 KM416C254        |
|A            |         |     |-----------|                            |
|M            |         |RESET_SW                                      |
|M            |         |                                              |
|A            |---------|                     |-------------|          |
|      512K-EPR.U62       14.31818MHz         |Cyrix        |KM416S1020|
|                            |---------|      |GX MEDIA     |          |
|    |-------|  |------|     |LSI      |      |GXm-233GP    |          |
|    |XILINX |  |KS0164|     |L2A0788  |      |             |          |
|-|  |XCS05  |  |      |     |Cyrix    |      |             |          |
  |  |       |  |------|     |CX5520   |      |             |          |
  |  |-------| 16.9344MHz    |---------|      |-------------|KM416S1020|
  | LED               DIP20                                            |
  | |--------------FLASH-DAUGHTERBOARD----------------|                |
  | |                                                 |                |
  | |           FLASH.U30 FLASH.U29              DIP20|                |
  | |-------------------------------------------------|                |
  |--------------------------------------------------------------------|
Notes:
      Cyrix GXm233 - Main CPU; Cyrix GX Media running at 233MHz. Clock is generated by the Cyrix CX5520
                     and a 14.31818MHz xtal. That gives a 66.6MHz bus clock with a 3.5X multiplier for 233MHz
      Cyrix CX5220 - CPU-support chipset (BGA IC)
      FLASH.U29/30 - Intel Strata-Flash DA28F320J5 SSOP56 contained on a plug-in daughterboard; graphics ROMs
      FLASH.U3     - Intel Strata-Flash DA28F320J5 SSOP56 contained on a plug-in daughterboard; main program
      KS0164       - Samsung Electronics KS0164 General Midi compliant 32-voice Wavetable Synthesizer chip
                     with built-in 16bit CPU and MPU-401 compatibility (QFP100)
      512K-EPR     - 512k EPROM, boot-loader program. EPROM is tied to the KS0164 and the XCS05
      DIP20        - not-populated sockets
      KDA0340D     - Samsung KDA0340D CMOS low-power two-channel digital-to-analog converter (SOP28)
      KM416S1020   - Samsung 1M x16 SDRAM (x4, TSSOP50)
      KM416C254    - Samsung 256k x16 DRAM (x8, SOJ40)
      ICS5342      - combination dual programmable clock generator, 256bytes x18-bit RAM and a triple 8-bit video DAC (PLCC68)
      XCS05        - Xilinx Spartan XCS05 FPGA (PLCC84)

***************************************************************************/

#include "emu.h"
#include "cpu/i386/i386.h"
#include "memconv.h"
#include "devconv.h"


class funkball_state : public driver_device
{
public:
	funkball_state(running_machine &machine, const driver_device_config_base &config)
		: driver_device(machine, config) { }

};


static VIDEO_START(funkball)
{
}

static SCREEN_UPDATE(funkball)
{
	return 0;
}

static ADDRESS_MAP_START(funkball_map, AS_PROGRAM, 32)
	AM_RANGE(0xffff0000, 0xffffffff) AM_ROM AM_REGION("user1", 0)	/* System BIOS */
ADDRESS_MAP_END

static ADDRESS_MAP_START(funkball_io, AS_IO, 32)
ADDRESS_MAP_END

static INPUT_PORTS_START( funkball )
INPUT_PORTS_END

static MACHINE_CONFIG_START( funkball, funkball_state )
	MCFG_CPU_ADD("maincpu", MEDIAGX, 66666666*3.5)
	MCFG_CPU_PROGRAM_MAP(funkball_map)
	MCFG_CPU_IO_MAP(funkball_io)

	/* video hardware */
	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(60)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MCFG_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MCFG_SCREEN_SIZE(640, 480)
	MCFG_SCREEN_VISIBLE_AREA(0, 639, 0, 199)
	MCFG_SCREEN_UPDATE(funkball)
	MCFG_PALETTE_LENGTH(16)

	MCFG_VIDEO_START(funkball)
MACHINE_CONFIG_END

ROM_START( funkball )
	ROM_REGION32_LE(0x10000, "user1", 0)
	ROM_LOAD( "512k-epr.u62", 0x000000, 0x010000, CRC(cced894a) SHA1(298c81716e375da4b7215f3e588a45ca3ea7e35c) )

	ROM_REGION(0x400000, "user2", 0)
	ROM_LOAD( "flash.u3", 0x000000, 0x400000, CRC(fb376abc) SHA1(ea4c48bb6cd2055431a33f5c426e52c7af6997eb) )

	ROM_REGION(0x800000, "user3", 0)
	ROM_LOAD( "flash.u29", 0x000000, 0x400000, CRC(7cf6ff4b) SHA1(4ccdd4864ad92cc218998f3923997119a1a9dd1d) )
	ROM_LOAD( "flash.u30", 0x400000, 0x400000, CRC(1d46717a) SHA1(acfbd0a2ccf4d717779733c4a9c639296c3bbe0e) )
ROM_END


GAME(1998, funkball, 0, funkball, funkball, 0, ROT0, "dgPIX Entertainment Inc.", "Funky Ball", GAME_NO_SOUND|GAME_NOT_WORKING)
