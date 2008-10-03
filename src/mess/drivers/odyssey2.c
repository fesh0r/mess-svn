/***************************************************************************

  /drivers/odyssey2.c

  Driver file to handle emulation of the Odyssey2.

  Minor update to "the voice" rom names, and add comment about
  the older revision of "the voice" - LN, 10/03/08

***************************************************************************/

#include "driver.h"
#include "cpu/mcs48/mcs48.h"
#include "includes/odyssey2.h"
#include "devices/cartslot.h"
#include "sound/sp0256.h"

static ADDRESS_MAP_START( odyssey2_mem , ADDRESS_SPACE_PROGRAM, 8)
	AM_RANGE( 0x0000, 0x03FF) AM_ROM
	AM_RANGE( 0x0400, 0x0BFF) AM_RAMBANK(1)
	AM_RANGE( 0x0C00, 0x0FFF) AM_RAMBANK(2)
ADDRESS_MAP_END

static ADDRESS_MAP_START( odyssey2_io , ADDRESS_SPACE_IO, 8)
	AM_RANGE( 0x00,		 0xff)		AM_READWRITE( odyssey2_bus_r, odyssey2_bus_w)
	AM_RANGE( MCS48_PORT_P1,	MCS48_PORT_P1)	AM_READWRITE( odyssey2_getp1, odyssey2_putp1 )
	AM_RANGE( MCS48_PORT_P2,	MCS48_PORT_P2)	AM_READWRITE( odyssey2_getp2, odyssey2_putp2 )
	AM_RANGE( MCS48_PORT_BUS,	MCS48_PORT_BUS)	AM_READWRITE( odyssey2_getbus, odyssey2_putbus )
	AM_RANGE( MCS48_PORT_T0,	MCS48_PORT_T0)  AM_READ( odyssey2_t0_r )
	AM_RANGE( MCS48_PORT_T1,	MCS48_PORT_T1)	AM_READ( odyssey2_t1_r )
ADDRESS_MAP_END

static ADDRESS_MAP_START( g7400_io , ADDRESS_SPACE_IO, 8)
	AM_RANGE( 0x00,      0xff)      AM_READWRITE( g7400_bus_r, g7400_bus_w)
	AM_RANGE( MCS48_PORT_P1,	MCS48_PORT_P1)  AM_READWRITE( odyssey2_getp1, odyssey2_putp1 )
	AM_RANGE( MCS48_PORT_P2,	MCS48_PORT_P2)  AM_READWRITE( odyssey2_getp2, odyssey2_putp2 )
	AM_RANGE( MCS48_PORT_BUS,	MCS48_PORT_BUS) AM_READWRITE( odyssey2_getbus, odyssey2_putbus )
	AM_RANGE( MCS48_PORT_T0,	MCS48_PORT_T0)  AM_READ( odyssey2_t0_r )
	AM_RANGE( MCS48_PORT_T1,	MCS48_PORT_T1)  AM_READ( odyssey2_t1_r )
ADDRESS_MAP_END

static INPUT_PORTS_START( odyssey2 )
	PORT_START("KEY0")		/* IN0 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_0) PORT_CHAR('0')
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_1) PORT_CHAR('1')
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_2) PORT_CHAR('2')
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_3) PORT_CHAR('3')
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_4) PORT_CHAR('4')
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_5) PORT_CHAR('5')
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_6) PORT_CHAR('6')
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_7) PORT_CHAR('7')

	PORT_START("KEY1")		/* IN1 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_8) PORT_CHAR('8')
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_9) PORT_CHAR('9')
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("?? :") PORT_CODE(KEYCODE_F1) PORT_CHAR(UCHAR_MAMEKEY(F1))
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("?? $") PORT_CODE(KEYCODE_F2) PORT_CHAR(UCHAR_MAMEKEY(F2))
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_SPACE) PORT_CHAR(' ')
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_SLASH) PORT_CHAR('?')
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_L) PORT_CHAR('L')
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_P) PORT_CHAR('P')

	PORT_START("KEY2")		/* IN2 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_PLUS_PAD) PORT_CHAR('+')
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_W) PORT_CHAR('W')
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_E) PORT_CHAR('E')
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_R) PORT_CHAR('R')
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_T) PORT_CHAR('T')
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_U) PORT_CHAR('U')
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_I) PORT_CHAR('I')
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_O) PORT_CHAR('O')

	PORT_START("KEY3")		/* IN3 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_Q) PORT_CHAR('Q')
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_S) PORT_CHAR('S')
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_D) PORT_CHAR('D')
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_F) PORT_CHAR('F')
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_G) PORT_CHAR('G')
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_H) PORT_CHAR('H')
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_J) PORT_CHAR('J')
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_K) PORT_CHAR('K')

	PORT_START("KEY4")		/* IN4 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_A) PORT_CHAR('A')
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_Z) PORT_CHAR('Z')
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_X) PORT_CHAR('X')
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_C) PORT_CHAR('C')
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_V) PORT_CHAR('V')
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_B) PORT_CHAR('B')
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_M) PORT_CHAR('M')
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_STOP) PORT_CHAR('.')

	PORT_START("KEY5")		/* IN5 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_MINUS) PORT_CHAR('-')
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_ASTERISK) PORT_CHAR('*')
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_SLASH_PAD) PORT_CHAR('/')
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_EQUALS) PORT_CHAR('=')
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME(DEF_STR( Yes )) PORT_CODE(KEYCODE_Y) PORT_CHAR('Y')
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME(DEF_STR( No )) PORT_CODE(KEYCODE_N) PORT_CHAR('N')
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("CLR") PORT_CODE(KEYCODE_BACKSPACE) PORT_CHAR(8)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("ENT") PORT_CODE(KEYCODE_ENTER) PORT_CHAR('\r')

	PORT_START("JOY0")		/* IN6 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP)		PORT_PLAYER(1)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT)	PORT_PLAYER(1)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN)	PORT_PLAYER(1)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT)	PORT_PLAYER(1)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1)			PORT_PLAYER(1)
	PORT_BIT( 0xe0, 0xe0,	 IPT_UNUSED )

	PORT_START("JOY1")		/* IN7 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP)		PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT)	PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN)	PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT)	PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1)			PORT_PLAYER(2)
	PORT_BIT( 0xe0, 0xe0,	 IPT_UNUSED )
INPUT_PORTS_END

static const gfx_layout odyssey2_graphicslayout =
{
	8,1,
	256,                                    /* 256 characters */
	1,                      /* 1 bits per pixel */
	{ 0 },                  /* no bitplanes; 1 bit per pixel */
	/* x offsets */
	{
	0,
	1,
	2,
	3,
	4,
	5,
	6,
	7,
	},
	/* y offsets */
	{ 0 },
	1*8
};


static const gfx_layout odyssey2_spritelayout =
{
	8,1,
	256,                                    /* 256 characters */
	1,                      /* 1 bits per pixel */
	{ 0 },                  /* no bitplanes; 1 bit per pixel */
	/* x offsets */
	{
	7,6,5,4,3,2,1,0
	},
	/* y offsets */
	{ 0 },
	1*8
};

static GFXDECODE_START( odyssey2 )
	GFXDECODE_ENTRY( "gfx1", 0x0000, odyssey2_graphicslayout, 0, 2 )
	GFXDECODE_ENTRY( "gfx1", 0x0000, odyssey2_spritelayout, 0, 2 )
GFXDECODE_END

static const sp0256_interface the_voice_sp0256 = {
	the_voice_lrq_callback,
	0
};

static MACHINE_DRIVER_START( odyssey2 )
	/* basic machine hardware */
	MDRV_CPU_ADD("main", I8048, ( ( XTAL_7_15909MHz * 3 ) / 4 ) )
	MDRV_CPU_PROGRAM_MAP(odyssey2_mem, 0)
	MDRV_CPU_IO_MAP(odyssey2_io, 0)
	MDRV_INTERLEAVE(1)

	MDRV_MACHINE_RESET( odyssey2 )

    /* video hardware */
	MDRV_SCREEN_ADD("main", RASTER)
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_RAW_PARAMS( XTAL_7_15909MHz/2, I824X_LINE_CLOCKS, I824X_START_ACTIVE_SCAN, I824X_END_ACTIVE_SCAN, 262, I824X_START_Y, I824X_START_Y + I824X_SCREEN_HEIGHT )

	MDRV_GFXDECODE( odyssey2 )
	MDRV_PALETTE_LENGTH(24)
	MDRV_PALETTE_INIT( odyssey2 )

	MDRV_VIDEO_START( odyssey2 )
	MDRV_VIDEO_UPDATE( odyssey2 )

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")
	MDRV_SOUND_ADD("custom", CUSTOM, XTAL_7_15909MHz/2)
	MDRV_SOUND_CONFIG(odyssey2_sound_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.40)

	MDRV_SOUND_ADD("sp0256_speech", SP0256, 3120000)
	MDRV_SOUND_CONFIG(the_voice_sp0256)
	/* The Voice uses a speaker with its own volume control so the relative volumes to use are subjective, these sound good */
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.00)
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( videopac )
	/* basic machine hardware */
	MDRV_CPU_ADD("main", I8048, ( XTAL_17_73447MHz / 3 ) )
	MDRV_CPU_PROGRAM_MAP(odyssey2_mem, 0)
	MDRV_CPU_IO_MAP(odyssey2_io, 0)
	MDRV_INTERLEAVE(1)

	MDRV_MACHINE_RESET( odyssey2 )

	/* video hardware */
	MDRV_SCREEN_ADD("main", RASTER)
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_RAW_PARAMS( XTAL_17_73447MHz/5, I824X_LINE_CLOCKS, I824X_START_ACTIVE_SCAN, I824X_END_ACTIVE_SCAN, 312, I824X_START_Y, I824X_START_Y + I824X_SCREEN_HEIGHT )

	MDRV_GFXDECODE( odyssey2 )
	MDRV_PALETTE_LENGTH(24)
	MDRV_PALETTE_INIT( odyssey2 )

	MDRV_VIDEO_START( odyssey2 )
	MDRV_VIDEO_UPDATE( odyssey2 )

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")
	MDRV_SOUND_ADD("custom", CUSTOM, XTAL_17_73447MHz/5)
	MDRV_SOUND_CONFIG(odyssey2_sound_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.40)

	MDRV_SOUND_ADD("sp0256_speech", SP0256, 3120000)
	MDRV_SOUND_CONFIG(the_voice_sp0256)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.00)
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( g7400 )
	/* basic machine hardware */
	MDRV_CPU_ADD("main", I8048, 5911000 )
	MDRV_CPU_PROGRAM_MAP(odyssey2_mem, 0)
	MDRV_CPU_IO_MAP(g7400_io, 0)
	MDRV_INTERLEAVE(1)

	MDRV_MACHINE_RESET( odyssey2 )

	/* video hardware */
	MDRV_SCREEN_ADD("main", RASTER)
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_RAW_PARAMS( 3547000*2, 448, 96, 416, 312, 39, 289 )	/* EF9340 doubles the input clock into dot clocks internally */

	MDRV_GFXDECODE( odyssey2 )
	MDRV_PALETTE_LENGTH(24)
	MDRV_PALETTE_INIT( odyssey2 )

	MDRV_VIDEO_START( odyssey2 )
//	MDRV_VIDEO_UPDATE( odyssey2 )

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")
	MDRV_SOUND_ADD("custom", CUSTOM, 3547000)
	MDRV_SOUND_CONFIG(odyssey2_sound_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.40)
MACHINE_DRIVER_END

ROM_START (odyssey2)
    ROM_REGION(0x10000,"main",0)    /* safer for the memory handler/bankswitching??? */
    ROM_LOAD ("o2bios.rom", 0x0000, 0x0400, CRC(8016a315) SHA1(b2e1955d957a475de2411770452eff4ea19f4cee))
    ROM_REGION(0x100, "gfx1", ROMREGION_ERASEFF)

    ROM_REGION(0x4000, "user1", 0)
	ROM_CART_LOAD(0, "bin,rom", 0x0000, 0x4000, ROM_MIRROR)

	ROM_REGION( 0x10000, "sp0256_speech", 0 )
	/* SP0256B-019 Speech chip w/2KiB mask rom */
	ROM_LOAD( "sp0256b-019.bin",   0x1000, 0x0800, CRC(19355075) SHA1(13bc08f08d161c30ff386d1f0d15676d82afde63) )

	/* A note about "The Voice": Two versions of "The Voice" exist:
	   * An earlier version with eight 2KiB speech roms, spr016-??? thru spr016-??? on a small daughterboard
	   <note to self: fill in numbers later>
	   * A later version with one 16KiB speech rom, spr128-003, mounted directly on the mainboard
	   The rom contents of these two versions are EXACTLY the same.
	   Both versions have an sp0256b-019 speech chip, which has 2KiB of its own internal speech data
	   Thanks to kevtris for this info. - LN
	*/
	/* External 16KiB speech ROM (spr128-003) from "The Voice" */
	ROM_LOAD( "spr128-003.bin",   0x4000, 0x4000, CRC(66041b03) SHA1(31acbaf1ae92b3efbb5093d63b0472170699da85) )
	/* Additional External 16KiB ROM (spr128-004) from S.I.D. the Spellbinder */
	ROM_LOAD( "spr128-004.bin",   0x8000, 0x4000, CRC(6780c7d3) SHA1(2e44233f25d07e35500ef79c9c542e974c94390a) )
ROM_END

ROM_START (videopac)
	ROM_REGION(0x10000,"main",0)    /* safer for the memory handler/bankswitching??? */
	ROM_SYSTEM_BIOS( 0, "g7000", "g7000" )
	ROMX_LOAD ("o2bios.rom", 0x0000, 0x0400, CRC(8016a315) SHA1(b2e1955d957a475de2411770452eff4ea19f4cee), ROM_BIOS(1))
	ROM_SYSTEM_BIOS( 1, "c52", "c52" )
	ROMX_LOAD ("c52.bin", 0x0000, 0x0400, CRC(a318e8d6) SHA1(a6120aed50831c9c0d95dbdf707820f601d9452e), ROM_BIOS(2))
	ROM_REGION(0x100, "gfx1", ROMREGION_ERASEFF)

	ROM_REGION(0x4000, "user1", 0)
	ROM_CART_LOAD(0, "bin,rom", 0x0000, 0x4000, ROM_MIRROR)

	ROM_REGION( 0x10000, "sp0256_speech", 0 )
	/* SP0256B-019 Speech chip w/2KiB mask rom */
	ROM_LOAD( "sp0256b-019.bin",   0x1000, 0x0800, CRC(19355075) SHA1(13bc08f08d161c30ff386d1f0d15676d82afde63) )
	/* External 16KiB speech ROM (spr128-003) from "The Voice" */
	ROM_LOAD( "spr128-003.bin",   0x4000, 0x4000, CRC(66041b03) SHA1(31acbaf1ae92b3efbb5093d63b0472170699da85) )
	/* Additional External 16KiB speech ROM (spr128-004) from S.I.D. the Spellbinder */
	ROM_LOAD( "spr128-004.bin",   0x8000, 0x4000, CRC(6780c7d3) SHA1(2e44233f25d07e35500ef79c9c542e974c94390a) )
ROM_END

ROM_START (g7400)
	ROM_REGION(0x10000,"main",0)    /* safer for the memory handler/bankswitching??? */
	ROM_LOAD ("g7400.bin", 0x0000, 0x0400, CRC(e20a9f41) SHA1(5130243429b40b01a14e1304d0394b8459a6fbae))
	ROM_REGION(0x100, "gfx1", ROMREGION_ERASEFF)

	ROM_REGION(0x4000, "user1", 0)
	ROM_CART_LOAD(0, "bin,rom", 0x0000, 0x4000, ROM_MIRROR)
ROM_END

static void odyssey2_cartslot_device_getinfo(const mess_device_class *devclass, UINT32 state, union devinfo *info)
{
	/* cartslot */
	switch(state)
	{
		/* --- the following bits of info are returned as 64-bit signed integers --- */
		case MESS_DEVINFO_INT_COUNT:							info->i = 1; break;
		case MESS_DEVINFO_INT_MUST_BE_LOADED:				info->i = 1; break;

		/* --- the following bits of info are returned as pointers to data or functions --- */
		case MESS_DEVINFO_PTR_VERIFY:						info->imgverify = odyssey2_cart_verify; break;

		default:										cartslot_device_getinfo(devclass, state, info); break;
	}
}

static SYSTEM_CONFIG_START(odyssey2)
	CONFIG_DEVICE(odyssey2_cartslot_device_getinfo)
SYSTEM_CONFIG_END

/*     YEAR  NAME      PARENT   COMPAT  MACHINE   INPUT     INIT      CONFIG    COMPANY     FULLNAME     FLAGS */
COMP( 1978, odyssey2, 0,		0,		odyssey2, odyssey2, odyssey2, odyssey2, "Magnavox", "Odyssey 2", GAME_IMPERFECT_SOUND )
COMP( 1979, videopac, odyssey2,	0,		videopac, odyssey2, odyssey2, odyssey2, "Philips", "Videopac G7000/C52", GAME_IMPERFECT_SOUND )
COMP( 1983, g7400, odyssey2, 0,			g7400,    odyssey2, odyssey2, odyssey2, "Philips", "Videopac Plus G7400", GAME_NOT_WORKING )


