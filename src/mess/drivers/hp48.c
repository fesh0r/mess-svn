/******************************************************************************
 hp48
 Peter.Trauner@jk.uni-linz.ac.at May 2000
******************************************************************************/

#include "driver.h"
#include "cpu/saturn/saturn.h"

#include "includes/hp48.h"

/* hp28s
 0-0x3ffff rom
 0xc0000- 0xcffff ram also mapped at 0xd0000-0xdffff */

static ADDRESS_MAP_START(hp48_mem, ADDRESS_SPACE_PROGRAM, 8)
	AM_RANGE( 0x00000, 0xfffff) AM_ROM // configured at runtime, complexe mmu
ADDRESS_MAP_END


static INPUT_PORTS_START( hp48s )
#if 1
	PORT_START
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("        A") PORT_CODE(KEYCODE_A)
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("        B") PORT_CODE(KEYCODE_B)
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("        C") PORT_CODE(KEYCODE_C)
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("        D") PORT_CODE(KEYCODE_D)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("        E") PORT_CODE(KEYCODE_E)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("        F") PORT_CODE(KEYCODE_F)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("MTH     G       PRINT") PORT_CODE(KEYCODE_G)
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("PRG     H       I/0") PORT_CODE(KEYCODE_H)
	PORT_START
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("CST     I       MODES") PORT_CODE(KEYCODE_I)
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("VAR     J       MEMORY") PORT_CODE(KEYCODE_J)
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("up      K       LIBRARY") PORT_CODE(KEYCODE_K) PORT_CODE(KEYCODE_UP)
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("NXT     L       PREV") PORT_CODE(KEYCODE_L)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("'       M       UP      HOME") PORT_CODE(KEYCODE_M)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("STO     N       DEF     RCL") PORT_CODE(KEYCODE_N)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("EVAL    O       ->Q     ->NUM") PORT_CODE(KEYCODE_P)
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("left    P       GRAPH") PORT_CODE(KEYCODE_P) PORT_CODE(KEYCODE_LEFT)
	PORT_START
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("down    Q       REVIEW") PORT_CODE(KEYCODE_Q) PORT_CODE(KEYCODE_DOWN)
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("right   R       SWAP") PORT_CODE(KEYCODE_R) PORT_CODE(KEYCODE_RIGHT)
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("SIN     S       ASIN    a") PORT_CODE(KEYCODE_S)
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("COS     T       ACOS    Integral") PORT_CODE(KEYCODE_T)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("TAN     U       ATAN    sum") PORT_CODE(KEYCODE_U)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("sqrt    V       square  root") PORT_CODE(KEYCODE_V)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("power   W       10^x    LOG") PORT_CODE(KEYCODE_W)
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("1/x     X       e^x     LN") PORT_CODE(KEYCODE_X)
	PORT_START
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("ENTER           EQUATION MATRIX") PORT_CODE(KEYCODE_ENTER) PORT_CODE(KEYCODE_ENTER_PAD)
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("+/-     Y       EDIT    VISIT") PORT_CODE(KEYCODE_Y)
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("EEX     Z       2D      3D") PORT_CODE(KEYCODE_Z)
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("DEL             PURGE") PORT_CODE(KEYCODE_DEL)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("<--             DROP    CLR") PORT_CODE(KEYCODE_BACKSPACE)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("alpha           USR     ENTRY") PORT_CODE(KEYCODE_LSHIFT) PORT_CODE(KEYCODE_RSHIFT)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("7               SOLVE") PORT_CODE(KEYCODE_7) PORT_CODE(KEYCODE_7_PAD)
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("8               PLOT") PORT_CODE(KEYCODE_8) PORT_CODE(KEYCODE_8_PAD)
	PORT_START
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("9               ALGEBRA") PORT_CODE(KEYCODE_9) PORT_CODE(KEYCODE_9_PAD)
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("divide          ()      #") PORT_CODE(KEYCODE_SLASH) PORT_CODE(KEYCODE_SLASH_PAD)
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("orange") PORT_CODE(KEYCODE_LCONTROL) PORT_CODE(KEYCODE_RCONTROL)
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("4               TIME") PORT_CODE(KEYCODE_4) PORT_CODE(KEYCODE_4_PAD)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("5               STAT") PORT_CODE(KEYCODE_5) PORT_CODE(KEYCODE_5_PAD)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("6               UNITS") PORT_CODE(KEYCODE_6) PORT_CODE(KEYCODE_6_PAD)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("*               []      _") PORT_CODE(KEYCODE_BACKSPACE) PORT_CODE(KEYCODE_ASTERISK)
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("blue") PORT_CODE(KEYCODE_LALT) PORT_CODE(KEYCODE_RALT)
	PORT_START
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("1               RAD     POLAR") PORT_CODE(KEYCODE_1) PORT_CODE(KEYCODE_1_PAD)
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("2               STACK   ARG") PORT_CODE(KEYCODE_2) PORT_CODE(KEYCODE_2_PAD)
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("3               CMD     MENU") PORT_CODE(KEYCODE_3) PORT_CODE(KEYCODE_3_PAD)
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("-               <<>>    \"\"") PORT_CODE(KEYCODE_MINUS) PORT_CODE(KEYCODE_MINUS_PAD)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("ON              CONT    OFF") PORT_CODE(KEYCODE_F1)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("0               =       ->") PORT_CODE(KEYCODE_0) PORT_CODE(KEYCODE_0_PAD)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(".               ,       enter") PORT_CODE(KEYCODE_STOP) PORT_CODE(KEYCODE_DEL_PAD)
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("SPC             Pi      angle") PORT_CODE(KEYCODE_SPACE)
	PORT_START
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("+               {}      ::") PORT_CODE(KEYCODE_EQUALS) PORT_CODE(KEYCODE_PLUS_PAD)

#else
	PORT_START
	PORT_BIT ( 0x20, 0,     IPT_UNUSED )
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("B") PORT_CODE(KEYCODE_F2)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("C") PORT_CODE(KEYCODE_F3)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("D") PORT_CODE(KEYCODE_F4)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("E") PORT_CODE(KEYCODE_F5)
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("F") PORT_CODE(KEYCODE_F6)
	PORT_START
	PORT_BIT ( 0x20, 0,     IPT_UNUSED )
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("PRG") PORT_CODE(KEYCODE_A)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("CST") PORT_CODE(KEYCODE_B)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("VAR") PORT_CODE(KEYCODE_C)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("up") PORT_CODE(KEYCODE_UP)
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("NXT") PORT_CODE(KEYCODE_D)
	PORT_START
	PORT_BIT ( 0x20, 0,     IPT_UNUSED )
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("STO") PORT_CODE(KEYCODE_E)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("EVL") PORT_CODE(KEYCODE_F)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("left") PORT_CODE(KEYCODE_LEFT)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("down") PORT_CODE(KEYCODE_DOWN)
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("right") PORT_CODE(KEYCODE_RIGHT)
	PORT_START
	PORT_BIT ( 0x20, 0,     IPT_UNUSED )
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("COS")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("TAN") PORT_CODE(KEYCODE_H)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("sqt") PORT_CODE(KEYCODE_LEFT)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("pwr") PORT_CODE(KEYCODE_DOWN)
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("inv") PORT_CODE(KEYCODE_RIGHT)
	PORT_START
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(DEF_STR( On)) PORT_CODE(KEYCODE_NUMLOCK)
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("ENT") PORT_CODE(KEYCODE_I)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("+/-") PORT_CODE(KEYCODE_MINUS)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("EEX") PORT_CODE(KEYCODE_LEFT)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("DEL") PORT_CODE(KEYCODE_BACKSPACE)
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("<==") PORT_CODE(KEYCODE_ENTER_PAD)
	PORT_START
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("alp") PORT_CODE(KEYCODE_LALT)
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("SIN") PORT_CODE(KEYCODE_J)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("7") PORT_CODE(KEYCODE_7_PAD)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("8") PORT_CODE(KEYCODE_8_PAD)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("9") PORT_CODE(KEYCODE_9_PAD)
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("/") PORT_CODE(KEYCODE_SLASH_PAD)
	PORT_START
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("yel") PORT_CODE(KEYCODE_LSHIFT)
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("MTH") PORT_CODE(KEYCODE_K)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("4") PORT_CODE(KEYCODE_4_PAD)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("5") PORT_CODE(KEYCODE_5_PAD)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("6") PORT_CODE(KEYCODE_6_PAD)
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("*") PORT_CODE(KEYCODE_ASTERISK)
	PORT_START
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("blu") PORT_CODE(KEYCODE_LSHIFT)
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("A") PORT_CODE(KEYCODE_F1)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("1") PORT_CODE(KEYCODE_1_PAD)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("2") PORT_CODE(KEYCODE_2_PAD)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("3") PORT_CODE(KEYCODE_3_PAD)
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("-") PORT_CODE(KEYCODE_MINUS_PAD)
	PORT_START
	PORT_BIT ( 0x20, 0,     IPT_UNUSED )
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("'") PORT_CODE(KEYCODE_L)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("0") PORT_CODE(KEYCODE_0_PAD)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(".") PORT_CODE(KEYCODE_DEL_PAD)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("SPC") PORT_CODE(KEYCODE_SPACE)
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("+") PORT_CODE(KEYCODE_PLUS_PAD)
#endif
INPUT_PORTS_END


static const gfx_layout hp48_charlayout =
{
	2,16,
	256,                                    /* 256 characters */
	1,                      /* 1 bits per pixel */
	{ 0 },                  /* no bitplanes; 1 bit per pixel */
	/* x offsets */
	{ 0,0 },
	/* y offsets */
	{
		7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0, 0
	},
	1*8
};

static GFXDECODE_START( hp48 )
	GFXDECODE_ENTRY( REGION_GFX1, 0x0000, hp48_charlayout, 0, 8 )
GFXDECODE_END

static const SATURN_CONFIG config={
	hp48_out, hp48_in,
	hp48_mem_reset, hp48_mem_config, hp48_mem_unconfig, hp48_mem_id,
	hp48_crc
};


static MACHINE_DRIVER_START( hp48s )
	/* basic machine hardware */
	MDRV_CPU_ADD_TAG("main", SATURN, 4000000)		/* 2 MHz */
	MDRV_CPU_PROGRAM_MAP(hp48_mem, 0)
	MDRV_CPU_CONFIG(config)
	MDRV_SCREEN_REFRESH_RATE(64)
	MDRV_SCREEN_VBLANK_TIME(DEFAULT_REAL_60HZ_VBLANK_DURATION)
	MDRV_INTERLEAVE(1)

	MDRV_MACHINE_RESET( hp48 )

	/* video hardware (well, actually there was no video ;) */
	/* scanned with 300 dpi, scaled x 55%, y 55% for perfect display 2x2 pixels */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(339, 775)
	MDRV_SCREEN_VISIBLE_AREA(0, 339-1, 0, 775-1)
	MDRV_GFXDECODE( hp48 )
	MDRV_PALETTE_LENGTH( 248 )
	MDRV_COLORTABLE_LENGTH( sizeof (hp48_colortable) / sizeof(hp48_colortable[0][0]) )
	MDRV_PALETTE_INIT( hp48 )

	MDRV_VIDEO_START( hp48 )
	MDRV_VIDEO_UPDATE( hp48 )
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( hp48g )
	MDRV_IMPORT_FROM( hp48s )
	MDRV_CPU_REPLACE( "main", SATURN, 8000000 )		/* 4 MHz */
	MDRV_CPU_PROGRAM_MAP(hp48_mem, 0)
	MDRV_CPU_CONFIG(config)
MACHINE_DRIVER_END

ROM_START(hp48s)
	ROM_REGION(0x1c0000,REGION_CPU1, 0)
	/* version at 0x7fff0 little endian 6 characters */
	/* 0x3fff8 in byte wide rom */
	ROM_SYSTEM_BIOS( 0, "sxrom-a",  "sxrom-a" )
	ROMX_LOAD("sxrom-a", 0x00000, 0x40000, CRC(a87696c7) SHA1(3271b103ad99254d069e20171beb418ace72cc90), ROM_BIOS(1) )
	ROM_SYSTEM_BIOS( 1, "sxrom-b",  "sxrom-b" )
	ROMX_LOAD("sxrom-b", 0x00000, 0x40000, CRC(034f6ce4) SHA1(acd256f2efee868ce402008f4131d94b312e60bc), ROM_BIOS(2) )
	ROM_SYSTEM_BIOS( 2, "sxrom-c",  "sxrom-c" )
	ROMX_LOAD("sxrom-c", 0x00000, 0x40000, CRC(a9a0279d) SHA1(fee852d43ae6941d07a9d0d31f37e68e4f9051b1), ROM_BIOS(3) )
	ROM_SYSTEM_BIOS( 3, "sxrom-d",  "sxrom-d" )
	ROMX_LOAD("sxrom-d", 0x00000, 0x40000, CRC(6e71244e) SHA1(893f29abd17434db7da986b27dc10c48f458d3aa), ROM_BIOS(4) )
	ROM_SYSTEM_BIOS( 4, "sxrom-e",  "sxrom-e" )
	ROMX_LOAD("sxrom-e", 0x00000, 0x40000, CRC(704ffa08) SHA1(0d498d135bf729c1d775cce522528837729e2e94), ROM_BIOS(5) )
	ROM_SYSTEM_BIOS( 5, "sxrom-ea",  "sxrom-e(Alternate)")
	ROMX_LOAD("sxrom-ea", 0x00000, 0x40000, CRC(d4f1390b) SHA1(f6be01a05aedefa8a285d480e33458447cf2bdaa), ROM_BIOS(6) )// differences only in the hardware window
	ROM_SYSTEM_BIOS( 6, "sxrom-f", 	"sxrom-f")
	ROMX_LOAD("sxrom-f", 0x00000, 0x40000, NO_DUMP, ROM_BIOS(7) )
	ROM_SYSTEM_BIOS( 7, "sxrom-j",  "sxrom-j" )
	ROMX_LOAD("sxrom-j", 0x00000, 0x40000, CRC(1a6378ef) SHA1(5235f5379f1fd7edfe9bb6bf466b60d279163e73), ROM_BIOS(8) )
//  ROM_SYSTEM_BIOS( 8, "rom.sx",  "bad version e dump" )
//  ROMX_LOAD("rom.sx", 0x00000, 0x40000, CRC(5619ccaf), ROM_BIOS(9) ) //revision E bad dump
	ROM_REGION(0x100,REGION_GFX1,0)
ROM_END

ROM_START(hp48g)
	ROM_REGION(0x580000,REGION_CPU1, 0)
	/* version at 0x7ffbf little endian 6 characters */
	ROM_SYSTEM_BIOS( 0, "gxrom-k",  "gxrom-k" )
	ROMX_LOAD("gxrom-k", 0x00000, 0x80000, CRC(bdd5d2ee) SHA1(afa1498238e991b1e3d07fb8b4c227b115f7bcc1), ROM_BIOS(1) )
	ROM_SYSTEM_BIOS( 1, "gxrom-l",  "gxrom-l" )
	ROMX_LOAD("gxrom-l", 0x00000, 0x80000, CRC(70958e6b) SHA1(8eebac69ff804086247b989bf320e57a2d8a59a7), ROM_BIOS(2) )
	ROM_SYSTEM_BIOS( 2, "gxrom-m",  "gxrom-m" )
	ROMX_LOAD("gxrom-m", 0x00000, 0x80000, CRC(e21a09e4) SHA1(09932d543594e459eeb94a79654168cd15e79a87), ROM_BIOS(3) )
	ROM_SYSTEM_BIOS( 3, "gxrom-p",  "gxrom-p" )
	ROMX_LOAD("gxrom-p", 0x00000, 0x80000, CRC(27f90428) SHA1(b98312716ff81b9b439002c2ec7a07b9808440fb), ROM_BIOS(4) )
	ROM_SYSTEM_BIOS( 4, "gxrom-r",  "gxrom-r" )
	ROMX_LOAD("gxrom-r", 0x00000, 0x80000, CRC(00ee1a62) SHA1(5705fc9ea791916c4456ac35e22275862411db9b), ROM_BIOS(5) )
	ROM_REGION(0x100,REGION_GFX1,0)
ROM_END

/*    YEAR  NAME    PARENT  COMPAT  MACHINE INPUT   INIT    CONFIG  COMPANY             FULLNAME    FLAGS */
COMP( 1989, hp48s,  0,      0,      hp48s,  hp48s,  hp48s,  NULL,   "Hewlett Packard",  "HP48S/SX", GAME_NOT_WORKING )
COMP( 1993, hp48g,  0,      0,      hp48g,  hp48s,  hp48g,  NULL,   "Hewlett Packard",  "HP48G/GX", GAME_NOT_WORKING )
