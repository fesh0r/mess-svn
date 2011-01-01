/***************************************************************************

    drivers/apple3.c

    Apple ///

    Driver is not working yet; seems to get caught in an infinite loop on
    startup.  Special thanks to Chris Smolinski (author of the Sara emulator)
    for his input about this poorly known system.

***************************************************************************/

#include "emu.h"
#include "cpu/m6502/m6502.h"
#include "includes/apple3.h"
#include "includes/apple2.h"
#include "devices/flopdrv.h"
#include "formats/ap2_dsk.h"
#include "machine/6551.h"
#include "machine/6522via.h"
#include "devices/messram.h"
#include "devices/appldriv.h"

static ADDRESS_MAP_START( apple3_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x00FF) AM_READWRITE(apple3_00xx_r, apple3_00xx_w)
	AM_RANGE(0x0100, 0x01FF) AM_RAMBANK("bank2")
	AM_RANGE(0x0200, 0x1FFF) AM_RAMBANK("bank3")
	AM_RANGE(0x2000, 0x9FFF) AM_RAMBANK("bank4")
	AM_RANGE(0xA000, 0xBFFF) AM_RAMBANK("bank5")
ADDRESS_MAP_END


/* the Apple /// does some weird tricks whereby it monitors the SYNC pin
 * on the CPU to check for indexed instructions and directs them to
 * different memory locations */
static const m6502_interface apple3_m6502_interface =
{
	apple3_indexed_read,	/* read_indexed_func */
	apple3_indexed_write,	/* write_indexed_func */
	NULL,					/* port_read_func */
	NULL,					/* port_write_func */
};

#ifdef UNUSED_FUNCTION
static void apple3_floppy_getinfo(const mess_device_class *devclass, UINT32 state, union devinfo *info)
{
    switch(state)
    {
        case MESS_DEVINFO_INT_COUNT:                            info->i = 4; break;

        case MESS_DEVINFO_INT_APPLE525_SPINFRACT_DIVIDEND:  info->i = 1; break;
        case MESS_DEVINFO_INT_APPLE525_SPINFRACT_DIVISOR:   info->i = 4; break;

        case MESS_DEVINFO_STR_NAME+0:                       strcpy(info->s = device_temp_str(), "slot6disk1"); break;
        case MESS_DEVINFO_STR_NAME+1:                       strcpy(info->s = device_temp_str(), "slot6disk2"); break;
        case MESS_DEVINFO_STR_SHORT_NAME+0:                 strcpy(info->s = device_temp_str(), "s6d1"); break;
        case MESS_DEVINFO_STR_SHORT_NAME+1:                 strcpy(info->s = device_temp_str(), "s6d2"); break;
        case MESS_DEVINFO_STR_DESCRIPTION+0:                    strcpy(info->s = device_temp_str(), "Slot 6 Disk #1"); break;
        case MESS_DEVINFO_STR_DESCRIPTION+1:                    strcpy(info->s = device_temp_str(), "Slot 6 Disk #2"); break;

        default:                                        apple525_device_getinfo(devclass, state, info); break;
    }
}
#endif

static const floppy_config apple3_floppy_config =
{
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	FLOPPY_STANDARD_5_25_DSHD,
	FLOPPY_OPTIONS_NAME(apple2),
	NULL
};

static MACHINE_CONFIG_START( apple3, apple3_state )
	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", M6502, 2000000)        /* 2 MHz */
	MCFG_CPU_PROGRAM_MAP(apple3_map)
	MCFG_CPU_CONFIG( apple3_m6502_interface )
	MCFG_CPU_PERIODIC_INT(apple3_interrupt, 192)
	MCFG_QUANTUM_TIME(HZ(60))

	MCFG_MACHINE_RESET( apple3 )

	/* video hardware */
	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(60)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500)) /* not accurate */
	MCFG_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MCFG_SCREEN_SIZE(280*2, 192)
	MCFG_SCREEN_VISIBLE_AREA(0, (280*2)-1,0,192-1)
	MCFG_PALETTE_LENGTH(16)
	MCFG_PALETTE_INIT( apple2 )

	MCFG_VIDEO_START( apple3 )
	MCFG_VIDEO_UPDATE( apple3 )

	/* fdc */
	MCFG_APPLEFDC_ADD("fdc", apple3_fdc_interface)
	MCFG_FLOPPY_APPLE_4_DRIVES_ADD(apple3_floppy_config,1,4)
	/* acia */
	MCFG_ACIA6551_ADD("acia")

	/* via */
	MCFG_VIA6522_ADD("via6522_0", 1000000, apple3_via_0_intf)
	MCFG_VIA6522_ADD("via6522_1", 2000000, apple3_via_1_intf)

	/* internal ram */
	MCFG_RAM_ADD("messram")
	MCFG_RAM_DEFAULT_SIZE("512K")
MACHINE_CONFIG_END


static INPUT_PORTS_START( apple3 )
    PORT_START("keyb_0")
    PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Delete")	PORT_CODE(KEYCODE_BACKSPACE)PORT_CHAR(8)
    PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(UTF8_LEFT)		PORT_CODE(KEYCODE_LEFT)		PORT_CHAR(UCHAR_MAMEKEY(LEFT))
    PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Tab")		PORT_CODE(KEYCODE_TAB)		PORT_CHAR(9)
    PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(UTF8_DOWN)		PORT_CODE(KEYCODE_DOWN)		PORT_CHAR(UCHAR_MAMEKEY(DOWN))
    PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(UTF8_UP)		PORT_CODE(KEYCODE_UP)		PORT_CHAR(UCHAR_MAMEKEY(UP))
    PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Return")	PORT_CODE(KEYCODE_ENTER)	PORT_CHAR(13)
    PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(UTF8_RIGHT)		PORT_CODE(KEYCODE_RIGHT)	PORT_CHAR(UCHAR_MAMEKEY(RIGHT))
    PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Esc")		PORT_CODE(KEYCODE_ESC)		PORT_CHAR(27)

    PORT_START("keyb_1")
    PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_SPACE)	PORT_CHAR(' ')
    PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_QUOTE)	PORT_CHAR('\'') PORT_CHAR('\"')
    PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_COMMA)	PORT_CHAR(',') PORT_CHAR('<')
    PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_MINUS)	PORT_CHAR('-') PORT_CHAR('_')
    PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_STOP)	PORT_CHAR('.') PORT_CHAR('>')
    PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_SLASH)	PORT_CHAR('/') PORT_CHAR('?')
    PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_0)		PORT_CHAR('0') PORT_CHAR(')')
    PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_1)		PORT_CHAR('1') PORT_CHAR('!')

    PORT_START("keyb_2")
    PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_2)	PORT_CHAR('2') PORT_CHAR('@')
    PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_3)	PORT_CHAR('3') PORT_CHAR('#')
    PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_4)	PORT_CHAR('4') PORT_CHAR('$')
    PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_5)	PORT_CHAR('5') PORT_CHAR('%')
    PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_6)	PORT_CHAR('6') PORT_CHAR('^')
    PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_7)	PORT_CHAR('7') PORT_CHAR('&')
    PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_8)	PORT_CHAR('8') PORT_CHAR('*')
    PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_9)	PORT_CHAR('9') PORT_CHAR('(')

    PORT_START("keyb_3")
    PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_COLON)		PORT_CHAR(';') PORT_CHAR(':')
    PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_EQUALS)		PORT_CHAR('=') PORT_CHAR('+')
    PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_OPENBRACE)	PORT_CHAR('[') PORT_CHAR('{')
    PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_BACKSLASH)	PORT_CHAR('\\') PORT_CHAR('|')
    PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_CLOSEBRACE)	PORT_CHAR(']') PORT_CHAR('}')
    PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_TILDE)		PORT_CHAR('`') PORT_CHAR('~')
    PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_A)			PORT_CHAR('A') PORT_CHAR('a')
    PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_B)			PORT_CHAR('B') PORT_CHAR('b')

    PORT_START("keyb_4")
    PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_C)	PORT_CHAR('C') PORT_CHAR('c')
    PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_D)	PORT_CHAR('D') PORT_CHAR('d')
    PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_E)	PORT_CHAR('E') PORT_CHAR('e')
    PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_F)	PORT_CHAR('F') PORT_CHAR('f')
    PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_G)	PORT_CHAR('G') PORT_CHAR('g')
    PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_H)	PORT_CHAR('H') PORT_CHAR('h')
    PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_I)	PORT_CHAR('I') PORT_CHAR('i')
    PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_J)	PORT_CHAR('J') PORT_CHAR('j')

    PORT_START("keyb_5")
    PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_K)	PORT_CHAR('K') PORT_CHAR('k')
    PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_L)	PORT_CHAR('L') PORT_CHAR('l')
    PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_M)	PORT_CHAR('M') PORT_CHAR('m')
    PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_N)	PORT_CHAR('N') PORT_CHAR('n')
    PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_O)	PORT_CHAR('O') PORT_CHAR('o')
    PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_P)	PORT_CHAR('P') PORT_CHAR('p')
    PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_Q)	PORT_CHAR('Q') PORT_CHAR('q')
    PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_R)	PORT_CHAR('R') PORT_CHAR('r')

    PORT_START("keyb_6")
    PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_S)	PORT_CHAR('S') PORT_CHAR('s')
    PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_T)	PORT_CHAR('T') PORT_CHAR('t')
    PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_U)	PORT_CHAR('U') PORT_CHAR('u')
    PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_V)	PORT_CHAR('V') PORT_CHAR('v')
    PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_W)	PORT_CHAR('W') PORT_CHAR('w')
    PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_X)	PORT_CHAR('X') PORT_CHAR('x')
    PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_Y)	PORT_CHAR('Y') PORT_CHAR('y')
    PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_Z)	PORT_CHAR('Z') PORT_CHAR('z')

    PORT_START("keyb_special")
    PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Left Shift")	PORT_CODE(KEYCODE_LSHIFT)	PORT_CHAR(UCHAR_SHIFT_1)
    PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Right Shift")	PORT_CODE(KEYCODE_RSHIFT)	PORT_CHAR(UCHAR_SHIFT_1)
    PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Control")		PORT_CODE(KEYCODE_LCONTROL)	PORT_CHAR(UCHAR_SHIFT_2)
    PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED)
    PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
    PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("RESET")		PORT_CODE(KEYCODE_F12)
INPUT_PORTS_END

ROM_START(apple3)
    ROM_REGION(0x1000,"maincpu",0)
	ROM_LOAD( "apple3.rom", 0x0000, 0x1000, CRC(55e8eec9) SHA1(579ee4cd2b208d62915a0aa482ddc2744ff5e967))
ROM_END

/*     YEAR     NAME        PARENT  COMPAT  MACHINE    INPUT    INIT    COMPANY             FULLNAME */
COMP( 1980,	apple3,		0,		0,		apple3,    apple3,	apple3,		"Apple Computer",	"Apple ///", GAME_NOT_WORKING | GAME_NO_SOUND )

