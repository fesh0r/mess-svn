/******************************************************************************
 PeT mess@utanet.at Nov 2000
Updated by Dan Boris, 3/4/2007
Rewrite in progress, Dirk Best, 2007-07-31

ToDo:
    - Printer. Tried to implement this but it was not working, currently disabled.
    - Dual tape interface
    - Implement punchtape reader/writer and TTY keyboard
    - Front panel Reset switch (switch S1)
    - Front panel Run/Step switch (switch S2)

******************************************************************************/

/* Core includes */
#include "driver.h"
#include "includes/aim65.h"

/* peripheral chips */
#include "machine/6522via.h"
#include "machine/6532riot.h"
#include "machine/6821pia.h"

/* cartridge device */
#include "devices/cartslot.h"

/* for the layout */
#include "rendlay.h"
#include "aim65.lh"



/******************************************************************************
 Memory Maps
******************************************************************************/


/* Note: RAM is mapped dynamically in machine/aim65.c */
static ADDRESS_MAP_START( aim65_mem , ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE( 0x1000, 0x9fff ) AM_NOP /* User available expansions */
	AM_RANGE( 0xa000, 0xa00f ) AM_MIRROR(0x3f0) AM_READWRITE( via_1_r, via_1_w ) /* User VIA */
	AM_RANGE( 0xa400, 0xa47f ) AM_RAM /* RIOT RAM */
	AM_RANGE( 0xa480, 0xa497 ) AM_READWRITE( r6532_0_r, r6532_0_w )
	AM_RANGE( 0xa498, 0xa7ff ) AM_NOP /* Not available */
	AM_RANGE( 0xa800, 0xa80f ) AM_MIRROR(0x3f0) AM_READWRITE( via_0_r, via_0_w )
	AM_RANGE( 0xac00, 0xac03 ) AM_READWRITE( pia_0_r, pia_0_w )
	AM_RANGE( 0xac04, 0xac43 ) AM_RAM /* PIA RAM */
	AM_RANGE( 0xac44, 0xafff ) AM_NOP /* Not available */
	AM_RANGE( 0xb000, 0xffff ) AM_ROM /* 5 ROM sockets */
ADDRESS_MAP_END



/******************************************************************************
 Input Ports
******************************************************************************/


static INPUT_PORTS_START( aim65 )
	PORT_START_TAG("keyboard_0")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Space")       PORT_CODE(KEYCODE_SPACE)      PORT_CHAR(32)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME(".  >")        PORT_CODE(KEYCODE_STOP)       PORT_CHAR('.') PORT_CHAR('>')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("M")           PORT_CODE(KEYCODE_M)          PORT_CHAR('m')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("B")           PORT_CODE(KEYCODE_B)          PORT_CHAR('b')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("C")           PORT_CODE(KEYCODE_C)          PORT_CHAR('c')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Z")           PORT_CODE(KEYCODE_Z)          PORT_CHAR('z')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START_TAG("keyboard_1")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("LF  @")       PORT_CODE(KEYCODE_QUOTE)      PORT_CHAR(10)  PORT_CHAR('@')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("L")           PORT_CODE(KEYCODE_L)          PORT_CHAR('l')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("J")           PORT_CODE(KEYCODE_J)          PORT_CHAR('j')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("G")           PORT_CODE(KEYCODE_G)          PORT_CHAR('g')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("D")           PORT_CODE(KEYCODE_D)          PORT_CHAR('d')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("A")           PORT_CODE(KEYCODE_A)          PORT_CHAR('a')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START_TAG("keyboard_2")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Print")       PORT_CODE(KEYCODE_BACKSPACE)  PORT_CHAR(UCHAR_MAMEKEY(PRTSCR))
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("P")           PORT_CODE(KEYCODE_P)          PORT_CHAR('p')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("I")           PORT_CODE(KEYCODE_I)          PORT_CHAR('i')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Y")           PORT_CODE(KEYCODE_Y)          PORT_CHAR('y')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("R")           PORT_CODE(KEYCODE_R)          PORT_CHAR('r')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("W")           PORT_CODE(KEYCODE_W)          PORT_CHAR('w')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Esc")         PORT_CODE(KEYCODE_TAB)        PORT_CHAR(UCHAR_MAMEKEY(ESC))

	PORT_START_TAG("keyboard_3")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Return")      PORT_CODE(KEYCODE_ENTER)      PORT_CHAR(13)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("-  =")        PORT_CODE(KEYCODE_OPENBRACE)  PORT_CHAR('-') PORT_CHAR('=')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("O")           PORT_CODE(KEYCODE_O)          PORT_CHAR('o')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("U")           PORT_CODE(KEYCODE_U)          PORT_CHAR('u')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("T")           PORT_CODE(KEYCODE_T)          PORT_CHAR('t')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("E")           PORT_CODE(KEYCODE_E)          PORT_CHAR('e')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Q")           PORT_CODE(KEYCODE_Q)          PORT_CHAR('q')

	PORT_START_TAG("keyboard_4")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Ctrl")        PORT_CODE(KEYCODE_CAPSLOCK)   PORT_CHAR(UCHAR_SHIFT_2)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME(":  *")        PORT_CODE(KEYCODE_MINUS)      PORT_CHAR(':') PORT_CHAR('*')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("9  )")        PORT_CODE(KEYCODE_9)          PORT_CHAR('9') PORT_CHAR(')')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("7  '")        PORT_CODE(KEYCODE_7)          PORT_CHAR('7') PORT_CHAR('\'')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("5  %")        PORT_CODE(KEYCODE_5)          PORT_CHAR('5') PORT_CHAR('%')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("3  #")        PORT_CODE(KEYCODE_3)          PORT_CHAR('3') PORT_CHAR('#')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("1  !")        PORT_CODE(KEYCODE_1)          PORT_CHAR('1') PORT_CHAR('!')

	PORT_START_TAG("keyboard_5")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Left Shift")  PORT_CODE(KEYCODE_LSHIFT)     PORT_CHAR(UCHAR_SHIFT_1)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("0")           PORT_CODE(KEYCODE_0)          PORT_CHAR('0')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("8  (")        PORT_CODE(KEYCODE_8)          PORT_CHAR('8') PORT_CHAR('(')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("6  &")        PORT_CODE(KEYCODE_6)          PORT_CHAR('6') PORT_CHAR('&')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("4  $")        PORT_CODE(KEYCODE_4)          PORT_CHAR('4') PORT_CHAR('$')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("2  \"")       PORT_CODE(KEYCODE_2)          PORT_CHAR('2') PORT_CHAR('\"')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("F3")          PORT_CODE(KEYCODE_EQUALS)     PORT_CHAR(UCHAR_MAMEKEY(F3))

	PORT_START_TAG("keyboard_6")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Right Shift") PORT_CODE(KEYCODE_RSHIFT)     PORT_CHAR(UCHAR_SHIFT_1)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Del")         PORT_CODE(KEYCODE_BACKSPACE)  PORT_CHAR(8)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME(";  +")        PORT_CODE(KEYCODE_COLON)      PORT_CHAR(';') PORT_CHAR('+')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("K")           PORT_CODE(KEYCODE_K)          PORT_CHAR('k')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("H")           PORT_CODE(KEYCODE_H)          PORT_CHAR('h')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("F")           PORT_CODE(KEYCODE_F)          PORT_CHAR('f')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("S")           PORT_CODE(KEYCODE_S)          PORT_CHAR('s')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("F2")          PORT_CODE(KEYCODE_CLOSEBRACE) PORT_CHAR(UCHAR_MAMEKEY(F2))

	PORT_START_TAG("keyboard_7")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("/  ?")        PORT_CODE(KEYCODE_SLASH)      PORT_CHAR('/') PORT_CHAR('?')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME(",  <")        PORT_CODE(KEYCODE_COMMA)      PORT_CHAR(',') PORT_CHAR('<')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("N")           PORT_CODE(KEYCODE_N)          PORT_CHAR('n')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("V")           PORT_CODE(KEYCODE_V)          PORT_CHAR('v')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("X")           PORT_CODE(KEYCODE_X)          PORT_CHAR('x')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("F1")          PORT_CODE(KEYCODE_BACKSLASH)  PORT_CHAR(UCHAR_MAMEKEY(F1))

	PORT_START_TAG("switches")
	PORT_DIPNAME(0x08, 0x08, "KB/TTY")
	PORT_DIPLOCATION("S3:1")
	PORT_DIPSETTING( 0x00, "TTY")
	PORT_DIPSETTING( 0x08, "KB")
INPUT_PORTS_END



/******************************************************************************
 Machine Drivers
******************************************************************************/


static MACHINE_DRIVER_START( aim65 )
	/* basic machine hardware */
	MDRV_CPU_ADD(M6502, AIM65_CLOCK) /* 1 MHz */
	MDRV_CPU_PROGRAM_MAP(aim65_mem, 0)

	MDRV_DEFAULT_LAYOUT(layout_aim65)

	/* dummy values */
	MDRV_SCREEN_REFRESH_RATE(60)
	MDRV_SCREEN_VBLANK_TIME(DEFAULT_REAL_60HZ_VBLANK_DURATION)

	MDRV_VIDEO_START( aim65 )
MACHINE_DRIVER_END



/******************************************************************************
 ROM Definitions
******************************************************************************/


ROM_START( aim65 )
	ROM_REGION(0x10000, REGION_CPU1, 0)
	ROM_CART_LOAD(0, "z26", 0xb000, 0x1000, ROM_OPTIONAL)
	ROM_CART_LOAD(1, "z25", 0xc000, 0x1000, ROM_OPTIONAL)
	ROM_CART_LOAD(2, "z24", 0xd000, 0x1000, ROM_OPTIONAL)
	ROM_LOAD("aim65mon.z23", 0xe000, 0x1000, CRC(90e44afe) SHA1(78e38601edf6bfc787b58750555a636b0cf74c5c))
	ROM_LOAD("aim65mon.z22", 0xf000, 0x1000, CRC(d01914b0) SHA1(e5b5ddd4cd43cce073a718ee4ba5221f2bc84eaf))
ROM_END


/* Currently dumped and available software:
 *
 * Name        Loc  CRC32     SHA1
 * -------------------------------------------------------------------
 * Assembler   Z24  0878b399  483e92b57d64be51643a9f6490521a8572aa2f68
 * Basic V1.1  Z25  d7b42d2a  4bbdb28d332429825adea0266ed9192786d9e392
 * Basic V1.1  Z26  36a61f39  f5ce0126cb594a565e730973fd140d03c298cefa
 * Forth V1.3  Z25  0671d019  dd2a1613e435c833634100cf4a22c6cff70c7a26
 * Forth V1.3  Z26  a80ad472  42a2e8c86829a2fe48090e6665ff9fe25b12b070
 * Mathpack    Z24  4889af55  5e9541ddfc06e3802d09b30d1bd89c5da914c76e
 * Monitor     Z22  d01914b0  e5b5ddd4cd43cce073a718ee4ba5221f2bc84eaf
 * Monitor     Z23  90e44afe  78e38601edf6bfc787b58750555a636b0cf74c5c
 * PL/65 V1.0  Z25  76dcf864  e937c54ed109401f796640cd45b27dfefb76667e
 * PL/65 V1.0  Z26  2ac71abd  6df5e3125bebefac80d51d9337555f54bdf0d8ea
 *
 */



/******************************************************************************
 System Config
******************************************************************************/


SYSTEM_CONFIG_START( aim65 )
	CONFIG_DEVICE(cartslot_device_getinfo)
	CONFIG_RAM_DEFAULT(4 * 1024) /* 4KB RAM */
	CONFIG_RAM        (3 * 1024) /* 3KB RAM */
	CONFIG_RAM        (2 * 1024) /* 2KB RAM */
	CONFIG_RAM        (1 * 1024) /* 1KB RAM */
SYSTEM_CONFIG_END



/******************************************************************************
 Drivers
******************************************************************************/


/*    YEAR  NAME    PARENT  COMPAT  MACHINE INPUT   INIT    CONFIG  COMPANY     FULLNAME  FLAGS */
COMP(1977,	aim65,	0,		0,		aim65,	aim65,	aim65,	aim65,	"Rockwell",	"AIM 65", 0)
