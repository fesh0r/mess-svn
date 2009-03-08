/*************************************************************************

    driver/mtx.c

    Memotech MTX 500, MTX 512 and RS 128


    TODO:  - Finish RS232 support
           - Cartridge support
           - Floppy disk emulation
           - Hard disk emulation
           - CBM mode
           - "Silicon" disk emulation
           - Multi Effect Video Wall emulation (maybe)

**************************************************************************/


#include "driver.h"
#include "includes/mtx.h"
#include "cpu/z80/z80.h"
#include "cpu/z80/z80daisy.h"
#include "machine/z80ctc.h"
#include "machine/z80dart.h"
#include "video/tms9928a.h"
#include "sound/sn76496.h"
#include "machine/ctronics.h"
#include "devices/snapquik.h"



/*************************************
 *
 *  Global variables
 *
 *************************************/

UINT8 *mtx_ram;



/*************************************
 *
 *  Memory maps
 *
 *************************************/

static ADDRESS_MAP_START( mtx_mem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x1fff) AM_ROMBANK(1)
	AM_RANGE(0x2000, 0x3fff) AM_ROMBANK(2)
	AM_RANGE(0x4000, 0x7fff) AM_RAMBANK(3)
	AM_RANGE(0x8000, 0xbfff) AM_RAMBANK(4)
	AM_RANGE(0xc000, 0xffff) AM_RAM AM_BASE(&mtx_ram)
ADDRESS_MAP_END


static ADDRESS_MAP_START( mtx_io, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x00, 0x00) AM_DEVREAD("centronics", mtx_strobe_r) AM_WRITE(mtx_bankswitch_w)
	AM_RANGE(0x01, 0x01) AM_READWRITE(TMS9928A_vram_r, TMS9928A_vram_w)
	AM_RANGE(0x02, 0x02) AM_READWRITE(TMS9928A_register_r, TMS9928A_register_w)
	AM_RANGE(0x03, 0x03) AM_READWRITE(mtx_cst_r, mtx_cst_w)
	AM_RANGE(0x04, 0x04) AM_DEVREADWRITE("centronics", mtx_prt_r, centronics_data_w)
	AM_RANGE(0x05, 0x05) AM_READWRITE(mtx_key_lo_r, mtx_sense_w)
	AM_RANGE(0x06, 0x06) AM_READ(mtx_key_hi_r)
	AM_RANGE(0x06, 0x06) AM_DEVWRITE("sn76489a", sn76496_w)
	AM_RANGE(0x08, 0x0b) AM_DEVREADWRITE("z80ctc", z80ctc_r, z80ctc_w)
ADDRESS_MAP_END



/*************************************
 *
 *  Input ports
 *
 *************************************/

static INPUT_PORTS_START( mtx512 )
	PORT_START("keyboard_low_0")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_1)     PORT_CHAR('1')  PORT_CHAR('!')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_3)     PORT_CHAR('3')  PORT_CHAR('#')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_5)     PORT_CHAR('5')  PORT_CHAR('%')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_7)     PORT_CHAR('7')  PORT_CHAR('\'')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_9)     PORT_CHAR('9')  PORT_CHAR(')')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_MINUS) PORT_CHAR('-')  PORT_CHAR('=')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_TILDE) PORT_CHAR('\\') PORT_CHAR('|')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Keypad 7 Page") PORT_CODE(KEYCODE_7_PAD) PORT_CHAR(UCHAR_MAMEKEY(PGDN)) PORT_CHAR(UCHAR_MAMEKEY(7_PAD))

	PORT_START("keyboard_low_1")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_ESC)    PORT_CHAR(UCHAR_MAMEKEY(ESC))
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_2)      PORT_CHAR('2') PORT_CHAR('"')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_4)      PORT_CHAR('4') PORT_CHAR('$')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_6)      PORT_CHAR('6') PORT_CHAR('&')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_8)      PORT_CHAR('8') PORT_CHAR('(')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_0)      PORT_CHAR('0')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_EQUALS) PORT_CHAR('^') PORT_CHAR('~')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Keypad 8 EOL") PORT_CODE(KEYCODE_8_PAD) PORT_CHAR(UCHAR_MAMEKEY(END)) PORT_CHAR(UCHAR_MAMEKEY(8_PAD))

	PORT_START("keyboard_low_2")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_LCONTROL)   PORT_CHAR(UCHAR_SHIFT_2)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_W)          PORT_CHAR('w') PORT_CHAR('W')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_R)          PORT_CHAR('r') PORT_CHAR('R')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_Y)          PORT_CHAR('y') PORT_CHAR('Y')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_I)          PORT_CHAR('i') PORT_CHAR('I')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_P)          PORT_CHAR('p') PORT_CHAR('P')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_CLOSEBRACE) PORT_CHAR('[') PORT_CHAR('{')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_5_PAD)      PORT_CHAR(UCHAR_MAMEKEY(UP)) PORT_CHAR(UCHAR_MAMEKEY(5_PAD))

	PORT_START("keyboard_low_3")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_Q)         PORT_CHAR('q') PORT_CHAR('Q')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_E)         PORT_CHAR('e') PORT_CHAR('E')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_T)         PORT_CHAR('t') PORT_CHAR('T')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_U)         PORT_CHAR('u') PORT_CHAR('U')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_O)         PORT_CHAR('o') PORT_CHAR('O')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_OPENBRACE) PORT_CHAR('@') PORT_CHAR('`')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Linefeed") PORT_CODE(KEYCODE_PRTSCR)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_1_PAD)     PORT_CHAR(UCHAR_MAMEKEY(LEFT)) PORT_CHAR(UCHAR_MAMEKEY(1_PAD))

	PORT_START("keyboard_low_4")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("AlphaLock") PORT_CODE(KEYCODE_CAPSLOCK) PORT_CHAR(UCHAR_MAMEKEY(CAPSLOCK))
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_S)          PORT_CHAR('s') PORT_CHAR('S')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_F)          PORT_CHAR('f') PORT_CHAR('F')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_H)          PORT_CHAR('h') PORT_CHAR('H')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_K)          PORT_CHAR('k') PORT_CHAR('K')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_COLON)      PORT_CHAR(';') PORT_CHAR('+')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_BACKSLASH2) PORT_CHAR(']') PORT_CHAR('}')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_3_PAD)      PORT_CHAR(UCHAR_MAMEKEY(RIGHT)) PORT_CHAR(UCHAR_MAMEKEY(3_PAD))

	PORT_START("keyboard_low_5")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_A)     PORT_CHAR('a') PORT_CHAR('A')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_D)     PORT_CHAR('d') PORT_CHAR('D')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_G)     PORT_CHAR('g') PORT_CHAR('G')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_J)     PORT_CHAR('j') PORT_CHAR('J')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_L)     PORT_CHAR('l') PORT_CHAR('L')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_QUOTE) PORT_CHAR(':') PORT_CHAR('*')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_ENTER) PORT_CHAR(13)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_2_PAD) PORT_CHAR(UCHAR_MAMEKEY(HOME)) PORT_CHAR(UCHAR_MAMEKEY(2_PAD))

	PORT_START("keyboard_low_6")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_LSHIFT)  PORT_CHAR(UCHAR_SHIFT_1)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_X)       PORT_CHAR('x') PORT_CHAR('X')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_V)       PORT_CHAR('v') PORT_CHAR('V')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_N)       PORT_CHAR('n') PORT_CHAR('N')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_COMMA)   PORT_CHAR(',') PORT_CHAR('<')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_SLASH)   PORT_CHAR('/') PORT_CHAR('?')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_RSHIFT)  PORT_CHAR(UCHAR_SHIFT_1)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_DEL_PAD) PORT_CHAR(UCHAR_MAMEKEY(DOWN)) PORT_CHAR(UCHAR_MAMEKEY(DEL_PAD))

	PORT_START("keyboard_low_7")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_Z)      PORT_CHAR('z') PORT_CHAR('Z')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_C)      PORT_CHAR('c') PORT_CHAR('C')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_B)      PORT_CHAR('b') PORT_CHAR('B')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_M)      PORT_CHAR('m') PORT_CHAR('M')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_STOP)   PORT_CHAR('.') PORT_CHAR('>')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_EQUALS) PORT_CHAR('_')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_0_PAD)  PORT_CHAR(UCHAR_MAMEKEY(INSERT)) PORT_CHAR(UCHAR_MAMEKEY(0_PAD))
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Keypad Enter CLS") PORT_CODE(KEYCODE_ENTER_PAD) PORT_CHAR(UCHAR_MAMEKEY(ENTER_PAD))

	PORT_START("keyboard_high_0")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_9_PAD) PORT_CHAR(UCHAR_MAMEKEY(CANCEL)) PORT_CHAR(UCHAR_MAMEKEY(9_PAD))
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_F1)    PORT_CHAR(UCHAR_MAMEKEY(F1))
	PORT_BIT(0xfc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("keyboard_high_1")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_BACKSPACE) PORT_CHAR(8)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_F5)        PORT_CHAR(UCHAR_MAMEKEY(F5))
	PORT_BIT(0xfc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("keyboard_high_2")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_4_PAD) PORT_CHAR('\t') PORT_CHAR(UCHAR_MAMEKEY(4_PAD))
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_F2)    PORT_CHAR(UCHAR_MAMEKEY(F2))
	PORT_BIT(0xfc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("keyboard_high_3")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_6_PAD) PORT_CHAR(UCHAR_MAMEKEY(DEL)) PORT_CHAR(UCHAR_MAMEKEY(6_PAD))
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_F6)    PORT_CHAR(UCHAR_MAMEKEY(F6))
	PORT_BIT(0xfc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("keyboard_high_4")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_F7) PORT_CHAR(UCHAR_MAMEKEY(F7))
	PORT_BIT(0xfc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("keyboard_high_5")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_F3) PORT_CHAR(UCHAR_MAMEKEY(F3))
	PORT_BIT(0xfc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("keyboard_high_6")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_F8) PORT_CHAR(UCHAR_MAMEKEY(F8))
	PORT_BIT(0xfc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("keyboard_high_7")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_SPACE) PORT_CHAR(' ')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_F4)    PORT_CHAR(UCHAR_MAMEKEY(F4))
	PORT_BIT(0xfc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("country_code")
	PORT_DIPNAME(0x04, 0x00, "Country Code Switch 1")
	PORT_DIPSETTING(0x04, DEF_STR(Off) )
	PORT_DIPSETTING(0x00, DEF_STR(On) )
	PORT_DIPNAME(0x08, 0x00, "Country Code Switch 0")
	PORT_DIPSETTING(0x08, DEF_STR(Off) )
	PORT_DIPSETTING(0x00, DEF_STR(On) )
	PORT_BIT(0xf3, IP_ACTIVE_LOW, IPT_UNUSED)
INPUT_PORTS_END



/*************************************
 *
 *  Z80 daisy chain
 *
 *************************************/

static const z80_daisy_chain mtx_daisy_chain[] =
{
	{ "z80ctc" },
	{ NULL }
};

static TIMER_DEVICE_CALLBACK( ctc_c0_tick )
{
	const device_config *z80ctc = devtag_get_device(timer->machine, "z80ctc");

	z80ctc_trg_w(z80ctc, 0, 1);
	z80ctc_trg_w(z80ctc, 0, 0);
}

static TIMER_DEVICE_CALLBACK( ctc_c1_c2_tick )
{
	const device_config *z80ctc = devtag_get_device(timer->machine, "z80ctc");

	z80ctc_trg_w(z80ctc, 1, 1);
	z80ctc_trg_w(z80ctc, 1, 0);
	z80ctc_trg_w(z80ctc, 2, 1);
	z80ctc_trg_w(z80ctc, 2, 0);
}

static void mtx_ctc_interrupt(const device_config *device, int state)
{
	cpu_set_input_line(device->machine->cpu[0], 0, state);
}



/*************************************
 *
 *  Machine drivers
 *
 *************************************/

static const z80ctc_interface mtx_ctc_intf =
{
	0,
	mtx_ctc_interrupt,
	0,
	0,
	0
};

static Z80DART_INTERFACE( mtx_dart_intf )
{
	0,
	0,
	0,

	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,

	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,

	DEVCB_NULL,
};

static MACHINE_DRIVER_START( mtx512 )
	/* basic machine hardware */
	MDRV_CPU_ADD("maincpu", Z80, MTX_SYSTEM_CLOCK)
	MDRV_CPU_PROGRAM_MAP(mtx_mem, 0)
	MDRV_CPU_IO_MAP(mtx_io, 0)
	MDRV_CPU_VBLANK_INT("screen", mtx_interrupt)
	MDRV_CPU_CONFIG(mtx_daisy_chain)

	/* video hardware */
	MDRV_IMPORT_FROM(tms9928a)
	MDRV_SCREEN_MODIFY("screen")
	MDRV_SCREEN_REFRESH_RATE(50)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500)) /* not accurate */

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")
	MDRV_SOUND_ADD("sn76489a", SN76489A, MTX_SYSTEM_CLOCK)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.00)

	MDRV_Z80CTC_ADD( "z80ctc", MTX_SYSTEM_CLOCK, mtx_ctc_intf )
	MDRV_TIMER_ADD_PERIODIC("z80ctc_c0", ctc_c0_tick, HZ(50))
	MDRV_TIMER_ADD_PERIODIC("z80ctc_c1c2", ctc_c1_c2_tick, HZ(MTX_SYSTEM_CLOCK/13))

	/* printer */
	MDRV_CENTRONICS_ADD("centronics", standard_centronics)

	/* snapshot */
	MDRV_SNAPSHOT_ADD("snapshot", mtx, "mtb", 0.5)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( rs128 )
	MDRV_IMPORT_FROM(mtx512)

	MDRV_Z80DART_ADD("z80dart", MTX_SYSTEM_CLOCK, mtx_dart_intf)
MACHINE_DRIVER_END



/*************************************
 *
 *  ROM definitions
 *
 *************************************/

ROM_START( mtx512 )
	ROM_REGION(0x02000, "user1", 0)
	ROM_LOAD("osrom",    0x0000, 0x2000, CRC(9ca858cc) SHA1(3804503a58f0bcdea96bb6488833782ebd03976d))
	ROM_REGION(0x10000, "user2", 0)
	ROM_LOAD("basicrom", 0x0000, 0x2000, CRC(87b4e59c) SHA1(c49782a82a7f068c1195cd967882ba9edd546eaf))
	ROM_LOAD("assemrom", 0x2000, 0x2000, CRC(9d7538c3) SHA1(d1882c4ea61a68b1715bd634ded5603e18a99c5f))
	ROM_FILL(            0x4000, 0xc000, 0xff)
ROM_END


#define rom_mtx500  rom_mtx512
#define rom_rs128   rom_mtx512



/*************************************
 *
 *  System configs
 *
 *************************************/

static SYSTEM_CONFIG_START( mtx_common )
	CONFIG_RAM(160 * 1024)
	CONFIG_RAM(192 * 1024)
	CONFIG_RAM(224 * 1024)
	CONFIG_RAM(256 * 1024)
	CONFIG_RAM(288 * 1024)
	CONFIG_RAM(320 * 1024)
	CONFIG_RAM(352 * 1024)
	CONFIG_RAM(384 * 1024)
	CONFIG_RAM(416 * 1024)
	CONFIG_RAM(448 * 1024)
	CONFIG_RAM(480 * 1024)
	CONFIG_RAM(512 * 1024)
SYSTEM_CONFIG_END


static SYSTEM_CONFIG_START( mtx512 )
	CONFIG_IMPORT_FROM(mtx_common)
	CONFIG_RAM_DEFAULT(64 * 1024)
	CONFIG_RAM(96 * 1024)
	CONFIG_RAM(128 * 1024)
SYSTEM_CONFIG_END


static SYSTEM_CONFIG_START( mtx500 )
	CONFIG_IMPORT_FROM(mtx_common)
	CONFIG_RAM_DEFAULT(32 * 1024)
	CONFIG_RAM(64 * 1024)
	CONFIG_RAM(96 * 1024)
	CONFIG_RAM(128 * 1024)
SYSTEM_CONFIG_END

static SYSTEM_CONFIG_START( rs128 )
	CONFIG_IMPORT_FROM(mtx_common)
	CONFIG_RAM_DEFAULT(128 * 1024)
SYSTEM_CONFIG_END



/*************************************
 *
 *  Driver definitions
 *
 *************************************/

/*    YEAR  NAME      PARENT    COMPAT  MACHINE   INPUT     INIT     CONFIG,  COMPANY          FULLNAME   FLAGS */
COMP( 1983, mtx512,   0,		0,		mtx512,   mtx512,   mtx512,  mtx512,  "Memotech Ltd.", "MTX 512", 0 )
COMP( 1983, mtx500,   mtx512,   0,      mtx512,   mtx512,   mtx512,  mtx500,  "Memotech Ltd.", "MTX 500", 0 )
COMP( 1984, rs128,    mtx512,   0,      rs128,    mtx512,   rs128,   rs128,   "Memotech Ltd.", "RS 128",  0 )
