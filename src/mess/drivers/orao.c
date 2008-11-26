/***************************************************************************

        Orao driver by Miodrag Milanovic

        01/03/2008 Updated to work with latest SVN code
        23/02/2008 Sound support added.
        22/02/2008 Preliminary driver.

        Driver is based on work of Josip Perusanec

****************************************************************************/

#include "driver.h"
#include "cpu/m6502/m6502.h"
#include "sound/dac.h"
#include "includes/orao.h"
#include "devices/cassette.h"
#include "formats/orao_cas.h"

/* Address maps */
static ADDRESS_MAP_START(orao_mem, ADDRESS_SPACE_PROGRAM, 8)
    AM_RANGE( 0x0000, 0x5fff ) AM_RAM AM_BASE(&orao_memory)
    AM_RANGE( 0x6000, 0x7fff ) AM_RAM AM_BASE(&orao_video_ram) // video ram
    AM_RANGE( 0x8000, 0x9fff ) AM_READWRITE( orao_io_r, orao_io_w )
    AM_RANGE( 0xa000, 0xafff ) AM_RAM  // extension
    AM_RANGE( 0xb000, 0xbfff ) AM_RAM  // DOS
    AM_RANGE( 0xc000, 0xdfff ) AM_ROM
    AM_RANGE( 0xe000, 0xffff ) AM_ROM
ADDRESS_MAP_END


/* Input ports */
static INPUT_PORTS_START( orao )
	PORT_START("LINE0")
		PORT_BIT(0x0F, IP_ACTIVE_HIGH, IPT_UNUSED)
		PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Left") PORT_CODE(KEYCODE_LEFT)
		PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Up") PORT_CODE(KEYCODE_UP)
		PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Down") PORT_CODE(KEYCODE_DOWN)
		PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Righ") PORT_CODE(KEYCODE_RIGHT)
	PORT_START("LINE1")
		PORT_BIT(0x0F, IP_ACTIVE_HIGH, IPT_UNUSED)
		PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Enter") PORT_CODE(KEYCODE_ENTER)
		PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Control") PORT_CODE(KEYCODE_LCONTROL) PORT_CODE(KEYCODE_RCONTROL)
		PORT_BIT(0xC0, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_START("LINE2")
		PORT_BIT(0x0F, IP_ACTIVE_HIGH, IPT_UNUSED)
	  	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("F1") PORT_CODE(KEYCODE_F1)
    	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("F2") PORT_CODE(KEYCODE_F2)
    	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("F3") PORT_CODE(KEYCODE_F3)
    	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("F4") PORT_CODE(KEYCODE_F4)
	PORT_START("LINE3")
		PORT_BIT(0x0F, IP_ACTIVE_HIGH, IPT_UNUSED)
		PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Space") PORT_CODE(KEYCODE_SPACE)
		PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Shift") PORT_CODE(KEYCODE_LSHIFT) PORT_CODE(KEYCODE_RSHIFT)
		PORT_BIT(0xC0, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_START("LINE4")
		PORT_BIT(0x0F, IP_ACTIVE_HIGH, IPT_UNUSED)
	  	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("R") PORT_CODE(KEYCODE_R)
    	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Z") PORT_CODE(KEYCODE_Z)
    	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("T") PORT_CODE(KEYCODE_T)
    	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("6") PORT_CODE(KEYCODE_6)
	PORT_START("LINE5")
		PORT_BIT(0x0F, IP_ACTIVE_HIGH, IPT_UNUSED)
		PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("5") PORT_CODE(KEYCODE_5)
		PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("4") PORT_CODE(KEYCODE_4)
		PORT_BIT(0xC0, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_START("LINE6")
		PORT_BIT(0x0F, IP_ACTIVE_HIGH, IPT_UNUSED)
	  	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("O") PORT_CODE(KEYCODE_O)
    	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("I") PORT_CODE(KEYCODE_I)
    	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("U") PORT_CODE(KEYCODE_U)
    	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("7") PORT_CODE(KEYCODE_7)
	PORT_START("LINE7")
		PORT_BIT(0x0F, IP_ACTIVE_HIGH, IPT_UNUSED)
		PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("8") PORT_CODE(KEYCODE_8)
		PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("9") PORT_CODE(KEYCODE_9)
		PORT_BIT(0xC0, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_START("LINE8")
		PORT_BIT(0x0F, IP_ACTIVE_HIGH, IPT_UNUSED)
	  	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("E") PORT_CODE(KEYCODE_E)
    	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Q") PORT_CODE(KEYCODE_Q)
    	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("W") PORT_CODE(KEYCODE_W)
    	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("1") PORT_CODE(KEYCODE_1)
	PORT_START("LINE9")
		PORT_BIT(0x0F, IP_ACTIVE_HIGH, IPT_UNUSED)
		PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("2") PORT_CODE(KEYCODE_2)
		PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("3") PORT_CODE(KEYCODE_3)
		PORT_BIT(0xC0, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_START("LINE10")
		PORT_BIT(0x0F, IP_ACTIVE_HIGH, IPT_UNUSED)
	  	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("L") PORT_CODE(KEYCODE_L)
    	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("J") PORT_CODE(KEYCODE_J)
    	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("K") PORT_CODE(KEYCODE_K)
    	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("M") PORT_CODE(KEYCODE_M)
	PORT_START("LINE11")
		PORT_BIT(0x0F, IP_ACTIVE_HIGH, IPT_UNUSED)
		PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME(",") PORT_CODE(KEYCODE_COMMA)
		PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME(".") PORT_CODE(KEYCODE_STOP)
		PORT_BIT(0xC0, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_START("LINE12")
		PORT_BIT(0x0F, IP_ACTIVE_HIGH, IPT_UNUSED)
	  	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("D") PORT_CODE(KEYCODE_D)
    	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("A") PORT_CODE(KEYCODE_A)
    	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("S") PORT_CODE(KEYCODE_S)
    	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Y") PORT_CODE(KEYCODE_Y)
  	PORT_START("LINE13")
		PORT_BIT(0x0F, IP_ACTIVE_HIGH, IPT_UNUSED)
		PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("X") PORT_CODE(KEYCODE_X)
		PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("C") PORT_CODE(KEYCODE_C)
		PORT_BIT(0xC0, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_START("LINE14")
		PORT_BIT(0x0F, IP_ACTIVE_HIGH, IPT_UNUSED)
	  	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("F") PORT_CODE(KEYCODE_F)
    	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("H") PORT_CODE(KEYCODE_H)
    	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("G") PORT_CODE(KEYCODE_G)
    	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("N") PORT_CODE(KEYCODE_N)
	PORT_START("LINE15")
		PORT_BIT(0x0F, IP_ACTIVE_HIGH, IPT_UNUSED)
		PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("B") PORT_CODE(KEYCODE_B)
		PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("V") PORT_CODE(KEYCODE_V)
		PORT_BIT(0xC0, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_START("LINE16")
	  	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Ch") PORT_CODE(KEYCODE_COLON)
    	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Cj") PORT_CODE(KEYCODE_QUOTE)
    	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Zh") PORT_CODE(KEYCODE_SLASH)
    	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME(":") PORT_CODE(KEYCODE_TILDE)
	PORT_START("LINE17")
		PORT_BIT(0x0F, IP_ACTIVE_HIGH, IPT_UNUSED)
		PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Del") PORT_CODE(KEYCODE_BACKSPACE)
		PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("^") PORT_CODE(KEYCODE_BACKSLASH)
		PORT_BIT(0xC0, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_START("LINE18")
	  	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("P") PORT_CODE(KEYCODE_P)
    	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Dj") PORT_CODE(KEYCODE_CLOSEBRACE)
    	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Sh") PORT_CODE(KEYCODE_OPENBRACE)
    	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME(";") PORT_CODE(KEYCODE_EQUALS)
	PORT_START("LINE19")
		PORT_BIT(0x0F, IP_ACTIVE_HIGH, IPT_UNUSED)
		PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("-") PORT_CODE(KEYCODE_MINUS)
		PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("0") PORT_CODE(KEYCODE_0)
		PORT_BIT(0xC0, IP_ACTIVE_LOW, IPT_UNUSED)
INPUT_PORTS_END

/* Machine driver */
static const cassette_config orao_cassette_config =
{
	orao_cassette_formats,
	NULL,
	CASSETTE_STOPPED | CASSETTE_SPEAKER_ENABLED | CASSETTE_MOTOR_ENABLED
};


static MACHINE_DRIVER_START( orao )
    /* basic machine hardware */
    MDRV_CPU_ADD("main", M6502, 1000000)
    MDRV_CPU_PROGRAM_MAP(orao_mem, 0)
    MDRV_MACHINE_RESET( orao )

    /* video hardware */
	MDRV_SCREEN_ADD("main", RASTER)
	MDRV_SCREEN_REFRESH_RATE(50)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500)) /* not accurate */
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(256, 256)
	MDRV_SCREEN_VISIBLE_AREA(0, 256-1, 0, 256-1)
	MDRV_PALETTE_LENGTH(2)
	MDRV_PALETTE_INIT(black_and_white)

    MDRV_VIDEO_START(orao)
    MDRV_VIDEO_UPDATE(orao)

    /* audio hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")
	MDRV_SOUND_ADD("dac", DAC, 0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 8.00)
	MDRV_SOUND_ADD("cassette", WAVE, 0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.00)

	MDRV_CASSETTE_ADD( "cassette", orao_cassette_config )
MACHINE_DRIVER_END

/* ROM definition */
ROM_START( orao )
    ROM_REGION( 0x10000, "main", ROMREGION_ERASEFF )
    ROM_LOAD( "bas12.rom", 0xc000, 0x2000, CRC(42ae6f69) SHA1(b9d4a544fae13a9c492af027545178addd557111) )
    ROM_LOAD( "crt12.rom", 0xe000, 0x2000, CRC(94ebdc94) SHA1(3959d717f96558823ccc806c842d2fb5ab0c3890) )
ROM_END

ROM_START( orao103 )
    ROM_REGION( 0x10000, "main", ROMREGION_ERASEFF )
    ROM_LOAD( "bas13.rom", 0xc000, 0x2000, CRC(35daf5da) SHA1(499c5a4bd930c26ec6226623c2793b4c7f771658) )
    ROM_LOAD( "crt13.rom", 0xe000, 0x2000, CRC(e7076014) SHA1(0e213287b0b520440af6a2a6297788a9356818c2) )
ROM_END

/* Driver */

/*    YEAR  NAME   PARENT  COMPAT  MACHINE  INPUT   INIT     CONFIG         COMPANY          FULLNAME       FLAGS */
COMP( 1984, orao,     0,      0, 		orao, 	orao, 	orao, 	 0,  "PEL Varazdin", "Orao 102",		 0)
COMP( 1985, orao103,  orao,   0, 		orao, 	orao, 	orao103, 0,  "PEL Varazdin", "Orao 103",		 0)

