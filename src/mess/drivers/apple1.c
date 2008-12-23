/**********************************************************************

Apple I

CPU:        6502 @ 1.023 MHz
            (Effective speed with RAM refresh waits is 0.960 MHz.)

RAM:        4-8 KB on main board (4 KB standard)

            Additional memory could be added via the expansion
            connector, but the user was responsible for making sure
            the extra memory was properly interfaced.

            Some users replaced the onboard 4-kilobit RAM chips with
            16-kilobit RAM chips, increasing on-board memory to 32 KB,
            but this required modifying the RAM interface circuitry.

ROM:        256 bytes for Monitor program

            Optional cassette interface included 256 bytes for
            cassette routines.

Interrupts: None.
            (The system board had jumpers to allow interrupts, but
            these were not connected in a standard system.)

Video:      Dumb terminal, based on 7 1K-bit shift registers

Sound:      None

Hardware:   Motorola 6820 PIA for keyboard and display interface

Memory map:

$0000-$1FFF:    RAM address space
    $0000-$00FF:    6502 zero page
        $0024-$002B:    Zero page locations used by the Monitor
    $0100-$01FF:    6502 processor stack
    $0200-$027F:    Keyboard input buffer storage used by the Monitor
    $0280-$0FFF:    RAM space available for a program in a 4 KB system
    $1000-$1FFF:    Extra RAM space available for a program in an 8 KB system
                    not using cassette BASIC

$2000-$BFFF:    Unused address space, available for RAM in systems larger
                than 8 KB.

$C000-$CFFF:    Address space for optional cassette interface
    $C000-$C0FF:    Cassette interface I/O range
    $C100-$C1FF:    Cassette interface ROM

$D000-$DFFF:    I/O address space
    $D010-$D013:    Motorola 6820 PIA registers.
        $D010:          Keyboard input port
        $D011:          Control register for keyboard input port, with
                        key-available flag.
        $D012:          Display output port (bit 7 is a status input)
        $D013:          Control register for display output port
    (PIA registers also mirrored at $D014-$D017, $D018-$D01B, $D01C-$D01F,
    $D030-$D03F, $D050-$D05F, ... , $DFD0-$DFDF, $DFF0-$DFFF.)

$E000-$EFFF:    Extra RAM space available for a program in an 8 KB system
                modified to use cassette BASIC
                (The system simulated here always includes this RAM.)

$F000-$FFFF:    ROM address space
    $FF00-$FFFF:    Apple Monitor ROM

**********************************************************************/

#include "driver.h"
#include "cpu/m6502/m6502.h"
#include "machine/6821pia.h"
#include "includes/apple1.h"
#include "devices/snapquik.h"
#include "devices/cassette.h"


/* port i/o functions */

/* memory w/r functions */

static ADDRESS_MAP_START( apple1_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0xbfff) AM_NOP

	/* Cassette interface I/O space: */
	AM_RANGE(0xc000, 0xc0ff) AM_READWRITE(apple1_cassette_r, apple1_cassette_w)
	/* Cassette interface ROM: */
	AM_RANGE(0xc100, 0xc1ff) AM_ROM

	AM_RANGE(0xc200, 0xcfff) AM_NOP

	/* In $D000-$DFFF, PIA is selected by address bit 4 being high,
       and PIA registers are addressed with address bits 0-1.  All
       other address bits are ignored.  Thus $D010-$D013 is mirrored
       at all $Dxxx addresses with bit 4 high. */
	AM_RANGE(0xd010, 0xd013) AM_MIRROR(0x0fec) AM_READWRITE(pia_0_r, pia_0_w)
	/* $Dxxx addresses with bit 4 low are NOPs. */
	AM_RANGE(0xd000, 0xd00f) AM_NOP AM_MIRROR(0xfe0)

	/* We always include the remapped RAM for cassette BASIC, both for
       simplicity and to allow the running of BASIC programs. */
	AM_RANGE(0xe000, 0xefff) AM_RAM

	AM_RANGE(0xf000, 0xfeff) AM_NOP

	/* Monitor ROM: */
	AM_RANGE(0xff00, 0xffff) AM_ROM
ADDRESS_MAP_END

/* graphics output */

const gfx_layout apple1_charlayout =
{
	8, 8,				/* character cell is 7 pixels wide by 8 pixels high */
	64,					/* 64 characters in 2513 character generator ROM */
	1,					/* 1 bitplane */
	{ 0 },
	/* 5 visible pixels per row, starting at bit 3, with MSB being 0: */
	{ 3, 4, 5, 6, 7 },
	/* pixel rows stored from top to bottom: */
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8 * 8				/* 8 8-bit pixel rows per character */
};

static GFXDECODE_START( apple1 )
	GFXDECODE_ENTRY( "gfx1", 0x0000, apple1_charlayout, 0, 1 )
GFXDECODE_END

/* keyboard input */
/*
   It's very likely that the keyboard assgnments are totally wrong: the code in machine/apple1.c
   makes arbitrary assumptions about the mapping of the keys. The schematics that are available
   on the web can help revealing the real layout.
   The large picture of Woz's Apple I at http://home.earthlink.net/~judgementcall/apple1.jpg
   show probably how the real keyboard was meant to be: note how the shifted symbols on the digits
   and on some letters are different from the ones produced by current emulation and the presence
   of the gray keys.
*/

static INPUT_PORTS_START( apple1 )
	PORT_START("KEY0")	/* first sixteen keys */
	PORT_BIT( 0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_0)		PORT_CHAR('0')
	PORT_BIT( 0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_1)		PORT_CHAR('1')
	PORT_BIT( 0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_2)		PORT_CHAR('2')
	PORT_BIT( 0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_3)		PORT_CHAR('3')
	PORT_BIT( 0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_4) 		PORT_CHAR('4')
	PORT_BIT( 0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_5) 		PORT_CHAR('5')
	PORT_BIT( 0x0040, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_6) 		PORT_CHAR('6')
	PORT_BIT( 0x0080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_7) 		PORT_CHAR('7')
	PORT_BIT( 0x0100, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_8) 		PORT_CHAR('8')
	PORT_BIT( 0x0200, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_9) 		PORT_CHAR('9')
	PORT_BIT( 0x0400, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_MINUS)	PORT_CHAR('-')
	PORT_BIT( 0x0800, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_EQUALS)	PORT_CHAR('=')
	PORT_BIT( 0x1000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_OPENBRACE) PORT_CHAR('[')
	PORT_BIT( 0x2000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_CLOSEBRACE) PORT_CHAR(']')
	PORT_BIT( 0x4000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_COLON)	PORT_CHAR(';')
	PORT_BIT( 0x8000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_QUOTE)	PORT_CHAR('\'')

	PORT_START("KEY1")	/* second sixteen keys */
	PORT_BIT( 0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_COMMA)	PORT_CHAR(',')
	PORT_BIT( 0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_STOP)		PORT_CHAR('.')
	PORT_BIT( 0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_SLASH)	PORT_CHAR('/')
	PORT_BIT( 0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_BACKSLASH) PORT_CHAR('\\')
	PORT_BIT( 0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_A)		PORT_CHAR('A')
	PORT_BIT( 0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_B) 		PORT_CHAR('B')
	PORT_BIT( 0x0040, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_C) 		PORT_CHAR('C')
	PORT_BIT( 0x0080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_D) 		PORT_CHAR('D')
	PORT_BIT( 0x0100, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_E) 		PORT_CHAR('E')
	PORT_BIT( 0x0200, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_F) 		PORT_CHAR('F')
	PORT_BIT( 0x0400, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_G) 		PORT_CHAR('G')
	PORT_BIT( 0x0800, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_H) 		PORT_CHAR('H')
	PORT_BIT( 0x1000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_I) 		PORT_CHAR('I')
	PORT_BIT( 0x2000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_J) 		PORT_CHAR('J')
	PORT_BIT( 0x4000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_K) 		PORT_CHAR('K')
	PORT_BIT( 0x8000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_L) 		PORT_CHAR('L')

	PORT_START("KEY2")	/* third sixteen keys */
	PORT_BIT( 0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_M) 		PORT_CHAR('M')
	PORT_BIT( 0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_N) 		PORT_CHAR('N')
	PORT_BIT( 0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_O) 		PORT_CHAR('O')
	PORT_BIT( 0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_P) 		PORT_CHAR('P')
	PORT_BIT( 0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_Q) 		PORT_CHAR('Q')
	PORT_BIT( 0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_R) 		PORT_CHAR('R')
	PORT_BIT( 0x0040, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_S) 		PORT_CHAR('S')
	PORT_BIT( 0x0080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_T) 		PORT_CHAR('T')
	PORT_BIT( 0x0100, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_U) 		PORT_CHAR('U')
	PORT_BIT( 0x0200, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_V) 		PORT_CHAR('V')
	PORT_BIT( 0x0400, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_W) 		PORT_CHAR('W')
	PORT_BIT( 0x0800, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_X) 		PORT_CHAR('X')
	PORT_BIT( 0x1000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_Y) 		PORT_CHAR('Y')
	PORT_BIT( 0x2000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_Z) 		PORT_CHAR('Z')
	PORT_BIT( 0x4000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_ENTER)	PORT_CHAR(13)
	PORT_BIT( 0x8000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Backarrow") PORT_CODE(KEYCODE_BACKSPACE) PORT_CHAR('_')

	PORT_START("KEY3")	/* fourth sixteen keys */
	PORT_BIT( 0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_SPACE) PORT_CHAR(' ')
	PORT_BIT( 0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Escape") PORT_CODE(KEYCODE_ESC) PORT_CHAR(UCHAR_MAMEKEY(ESC))

	PORT_START("KEY4")	/* shift keys */
	PORT_BIT( 0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Shift (Left)") PORT_CODE(KEYCODE_LSHIFT) PORT_CHAR(UCHAR_SHIFT_1)
	PORT_BIT( 0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Shift (Right)") PORT_CODE(KEYCODE_RSHIFT) PORT_CHAR(UCHAR_SHIFT_1)
	PORT_BIT( 0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Control (Left)") PORT_CODE(KEYCODE_LCONTROL) PORT_CHAR(UCHAR_SHIFT_2)
	PORT_BIT( 0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Control (Right)") PORT_CODE(KEYCODE_RCONTROL) PORT_CHAR(UCHAR_SHIFT_2)

	PORT_START("KEY5")	/* RESET and CLEAR SCREEN pushbutton switches */
	PORT_BIT( 0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Reset") PORT_CODE(KEYCODE_F12) PORT_CHAR(UCHAR_MAMEKEY(F1))
	PORT_BIT( 0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Clear") PORT_CODE(KEYCODE_F2) PORT_CHAR(UCHAR_MAMEKEY(F2))
INPUT_PORTS_END

/* sound output */

static const cassette_config apple1_cassette_config =
{
	cassette_default_formats,
	NULL,
	CASSETTE_STOPPED
};

/* machine definition */
static MACHINE_DRIVER_START( apple1 )
	/* basic machine hardware */
	/* Actual CPU speed is 1.023 MHz, but RAM refresh effectively
       slows it to 960 kHz. */
	MDRV_CPU_ADD("main", M6502, 960000)        /* 1.023 MHz */
	MDRV_CPU_PROGRAM_MAP(apple1_map, 0)
	MDRV_SCREEN_ADD("main", RASTER)
	MDRV_SCREEN_REFRESH_RATE(60)
	/* Video is blanked for 70 out of 262 scanlines per refresh cycle.
       Each scanline is composed of 65 character times, 40 of which
       are visible, and each character time is 7 dot times; a dot time
       is 2 cycles of the fundamental 14.31818 MHz oscillator.  The
       total blanking time is about 4450 microseconds. */
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC((int) (70 * 65 * 7 * 2 / 14.31818)))
	MDRV_QUANTUM_TIME(HZ(60))

	MDRV_MACHINE_RESET( apple1 )

	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	/* It would be nice if we could implement some sort of display
       overscan here. */
	MDRV_SCREEN_SIZE(40 * 7, 24 * 8)
	MDRV_SCREEN_VISIBLE_AREA(0, 40 * 7 - 1, 0, 24 * 8 - 1)
	MDRV_GFXDECODE(apple1)
	MDRV_PALETTE_LENGTH(2)
	MDRV_PALETTE_INIT(black_and_white)

	MDRV_VIDEO_START(apple1)
	MDRV_VIDEO_UPDATE(apple1)

	/* snapshot */
	MDRV_SNAPSHOT_ADD(apple1, "snp", 0)

	MDRV_CASSETTE_ADD( "cassette", apple1_cassette_config )
MACHINE_DRIVER_END

ROM_START(apple1)
	ROM_REGION(0x10000, "main",0)
	ROM_LOAD("apple1.rom", 0xff00, 0x0100, CRC(a30b6af5) SHA1(224767aa499dc98767e042f375ced1359be8a35f))
	/* 256-byte cassette interface ROM: */
	ROM_LOAD("cassette.rom", 0xc100, 0x0100, CRC(11da1692) SHA1(2c536977bd85797453dba0646e3e94e9ff4f9236))
	/* 512-byte Signetics 2513 character generator ROM: */
	ROM_REGION(0x0200, "gfx1",0)
	ROM_LOAD("apple1.vid", 0x0000, 0x0200, CRC(a7e567fc) SHA1(b18aae0a2d4f92f5a7e22640719bbc4652f3f4ee))
ROM_END

static SYSTEM_CONFIG_START(apple1)
	/* Note that because we always include 4K of RAM at $E000-$EFFF,
       the RAM amounts listed here will be 4K below the actual RAM
       total. */
	CONFIG_RAM			(0x1000)
	CONFIG_RAM			(0x2000)
	CONFIG_RAM			(0x3000)
	CONFIG_RAM			(0x4000)
	CONFIG_RAM			(0x5000)
	CONFIG_RAM			(0x6000)
	CONFIG_RAM			(0x7000)
	CONFIG_RAM			(0x8000)
	CONFIG_RAM			(0x9000)
	CONFIG_RAM			(0xA000)
	CONFIG_RAM			(0xB000)
	CONFIG_RAM_DEFAULT	(0xC000)
SYSTEM_CONFIG_END

/*    YEAR  NAME    PARENT  COMPAT  MACHINE     INPUT       INIT    CONFIG  COMPANY             FULLNAME */
COMP( 1976,	apple1,	0,		0,		apple1,		apple1,		apple1,	apple1,	"Apple Computer",	"Apple I" , 0)
