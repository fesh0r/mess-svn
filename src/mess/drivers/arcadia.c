/******************************************************************************
Consolidation and enhancment of documentation by Manfred Schneider based on previous work from
 PeT mess@utanet.at and Paul Robson (autismuk@aol.com)

 Schematics, manuals and anything you can desire for at http://amigan.classicgaming.gamespy.com/

 TODO: implement the RESET key on the front panel
         find a dump of the charactyer ROM
         convert the drawing code to tilemap


 1. General
   SYSTEM
- Signetics 2650 CPU at 3.58/4 MHz(for NTSC) or at 4,433/5 (for PAL)
- 1k x 8 of RAM (physically present but only 512 byte available)
- 2 x 2 axis Analogue Joysticks
- 2 x 12 button Controllers
- 3 buttons on system unit and CPU Reset
   VIDEO
- 128 x 208 pixel resolution (alternate 128 x 104 mode available)
- 16 x 26 (can be 16x13) background display (2 colour 8 x 8 pixel characters)
- 4 x 8 x 8 Sprites (2 colour 8 x 8 pixels)
- total of 8 user defined characters available
   SOUND
- Single channel beeper


2. Memory map
The memory map of the 2001 is below.
0000 - 0FFF 4k ROM Block 1 (first 4k of a cartridge)
1000 - 13FF     mirror of $1800-$1BFF
1400 - 17FF     mirror of $1800-$1BFF
1800 - 1BFF     UVI2637 Area (detail description in video/arcadia.c)
    1800 - 18CF     Screen display , upper 13 lines, characters/palette high bits
    18D0 - 18EF Free for user programs.
    18F0 - 18F7     Sprite coordinates y0x0y1x1y2x2y3x3
    18F8 - 1908     registers of UVI
    1909 - 197F     Unmapped
    1980 - 19BF     User defined characters (8 possible, 8 bytes per character)
    19C0 - 19F7     Unmapped
    19F8 - 19FF     registers of UVI
    1A00 - 1ACF Screen display , lower 13 lines, characters/palette high bits
    1AD0 - 1AFF User memory
    1B00 - 1BFF     mirror of 1900-19FF
1C00 - 1FFF     mirror of 1800-1BFF
2000 - 2FFF 4k ROM Block 2 (for 8k carts such as Jungler)
3000 - 3FFF mirror of 1000-1FFF
4000 - 4FFF mirror of 0000-0FFF
5000 - 5FFF mirror of 1000-1FFF
6000 - 6FFF mirror of 0000-0FFF
7000 - 7FFF mirror of 1000-1FFF

The Palladium VCG memory map is as follows.
0000 - 0FFF 4k ROM Block 1 (first 4k of a cartridge)
1000 - 17FF     could be ROM or RAM but no Cartridge uses this
1800 - 1BFF     UVI2637 Area (detail description in video/arcadia.c)
    1800 - 18CF     Screen display , upper 13 lines, characters/palette high bits
    18D0 - 18EF Free for user programs.
    18F0 - 18F7     Sprite coordinates y0x0y1x1y2x2y3x3
    18F8 - 1908     registers of UVI
    1909 - 197F     Unmapped
    1980 - 19BF     User defined characters (8 possible, 8 bytes per character)
    19C0 - 19F7     Unmapped
    19F8 - 19FF     registers of UVI
    1A00 - 1ACF Screen display , lower 13 lines, characters/palette high bits
    1AD0 - 1AFF User memory
    1B00 - 1BFF     mirror of 1900-19FF
1C00 - 1FFF     could be ROM or RAM but no Cartridge uses this
2000 - 2FFF 4k ROM Block  (for 8k carts such as Jungler)
3000 - 3FFF could be ROM or RAM but no Cartridge uses this
4000 - 4FFF 4k ROM Block  (first 2K used by Golf)
5000 - 5FFF 4k ROM Block
6000 - 6FFF 4k ROM Block
7000 - 7FFF 4k ROM Block


3. ROM Images
ROM Images are loaded into 0000-0FFF. If the ROM is an 8k ROM the
second half of the Rom is located at 2000-2FFF. Except for the Golf cart
which is located from 0x0000-0x0FFF and a 2kbyte block from 0x4000 only on
Palladium VCG.


4. Controls
All key controls are indicated by a bit going to '1'. Unused bits at
the memory location are set to zero.

Keypads

1900-1902 (Player 1) 1904-1906 (Player 2)
The keypads are arranged as follows :-

        1       2       3
        4       5       6
        7       8       9
      Enter     0     Clear

Row 1/4/7 is 1900/4, Row 2/5/8/0 is 1901/5 and Row 3/6/9 is 1902/6
The topmost key is bit 3, the lowermost key is bit 0.

Location $1908 contains bit 0 Start,bit 1 Option,bit 2 Difficulty.
These keys are "latched" i.e. a press causes a logic 1 to appear
on the current frame.

The fire buttons are equivalent to Keypad #2 e.g. they are 1901 and
1905 bit 3.

Palladium has 4 additional key per keypad which are mapped at
1903 palladium player 1
1907 palladium player 2


5. Other information
Interrupts are not supported
The Read/Write 2650 CPU Port-Commands do not appear to be connected to
anything in hardware. No cartridge has been found which uses them.

 ******************************************************************************/

#include <assert.h>
#include "driver.h"
#include "cpu/s2650/s2650.h"
#include "includes/arcadia.h"
#include "devices/cartslot.h"

static ADDRESS_MAP_START( arcadia_mem, ADDRESS_SPACE_PROGRAM, 8)
	AM_RANGE( 0x0000, 0x0fff) AM_ROM
	AM_RANGE( 0x1800, 0x1aff) AM_READWRITE( arcadia_video_r, arcadia_video_w )
	AM_RANGE( 0x2000, 0x2fff) AM_ROM
ADDRESS_MAP_END

static ADDRESS_MAP_START( arcadia_io, ADDRESS_SPACE_IO, 8)
//{ S2650_CTRL_PORT,S2650_CTRL_PORT, },
//{ S2650_DATA_PORT,S2650_DATA_PORT, },
	AM_RANGE( S2650_SENSE_PORT,S2650_SENSE_PORT) AM_READ( arcadia_vsync_r)
ADDRESS_MAP_END

/* The Emerson Arcadia 2001 controllers have 2 fire buttons on the side,
   but actually they are wired to keypad button #2. The following definitions
   are meant to document this fact. The keypad has the following layout:

     1  2  3
     4  5  6
     7  8  9
    Cl  0 En                                                                  */

static INPUT_PORTS_START( arcadia )
	PORT_START("panel")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_OTHER )    PORT_NAME("Start")           PORT_CODE(KEYCODE_S)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_OTHER )    PORT_NAME("Option")          PORT_CODE(KEYCODE_O)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER )    PORT_NAME("Select")          PORT_CODE(KEYCODE_E)
//  PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OTHER )    PORT_NAME("Reset")           PORT_CODE(KEYCODE_R)         Not implemented
	PORT_START("controller1_col1")
	PORT_BIT( 0xf0, 0xf0, IPT_UNUSED)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON1  ) PORT_NAME("P1 Keypad 1")     PORT_CODE(KEYCODE_1)         PORT_PLAYER(1)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON4  ) PORT_NAME("P1 Keypad 4")     PORT_CODE(KEYCODE_4)         PORT_PLAYER(1)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON7  ) PORT_NAME("P1 Keypad 7")     PORT_CODE(KEYCODE_7)         PORT_PLAYER(1)
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON10 ) PORT_NAME("P1 Keypad Clear") PORT_CODE(KEYCODE_DEL)       PORT_PLAYER(1)
	PORT_START("controller1_col2")
	PORT_BIT( 0xf0, 0xf0, IPT_UNUSED)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON2  ) PORT_NAME("P1 Keypad 2 (& Fire Buttons)")     PORT_CODE(KEYCODE_2)         PORT_PLAYER(1)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON5  ) PORT_NAME("P1 Keypad 5")     PORT_CODE(KEYCODE_5)         PORT_PLAYER(1)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON8  ) PORT_NAME("P1 Keypad 8")     PORT_CODE(KEYCODE_8)         PORT_PLAYER(1)
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON11 ) PORT_NAME("P1 Keypad 0")     PORT_CODE(KEYCODE_0)         PORT_PLAYER(1)
	PORT_START("controller1_col3")
	PORT_BIT( 0xf0, 0xf0, IPT_UNUSED)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON3  ) PORT_NAME("P1 Keypad 3")     PORT_CODE(KEYCODE_3)         PORT_PLAYER(1)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON6  ) PORT_NAME("P1 Keypad 6")     PORT_CODE(KEYCODE_6)         PORT_PLAYER(1)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON9  ) PORT_NAME("P1 Keypad 9")     PORT_CODE(KEYCODE_9)         PORT_PLAYER(1)
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON12 ) PORT_NAME("P1 Keypad Enter") PORT_CODE(KEYCODE_ENTER)     PORT_PLAYER(1)
	PORT_START("controller1_extra")
	PORT_BIT( 0xff, 0xf0, IPT_UNUSED) // used in palladium
	PORT_START("controller2_col1")
	PORT_BIT( 0xf0, 0xf0, IPT_UNUSED)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON1  ) PORT_NAME("P2 Keypad 1")     PORT_CODE(KEYCODE_1_PAD)     PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON4  ) PORT_NAME("P2 Keypad 4")     PORT_CODE(KEYCODE_4_PAD)     PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON7  ) PORT_NAME("P2 Keypad 7")     PORT_CODE(KEYCODE_7_PAD)     PORT_PLAYER(2)
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON10 ) PORT_NAME("P2 Keypad Clear") PORT_CODE(KEYCODE_DEL_PAD)   PORT_PLAYER(2)
	PORT_START("controller2_col2")
	PORT_BIT( 0xf0, 0xf0, IPT_UNUSED)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON2  ) PORT_NAME("P2 Keypad 2 (& Fire Buttons)")     PORT_CODE(KEYCODE_2_PAD)     PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON5  ) PORT_NAME("P2 Keypad 5")     PORT_CODE(KEYCODE_5_PAD)     PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON8  ) PORT_NAME("P2 Keypad 8")     PORT_CODE(KEYCODE_8_PAD)     PORT_PLAYER(2)
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON11 ) PORT_NAME("P2 Keypad 0")     PORT_CODE(KEYCODE_0_PAD)     PORT_PLAYER(2)
	PORT_START("controller2_col3")
	PORT_BIT( 0xf0, 0xf0, IPT_UNUSED)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON3  ) PORT_NAME("P2 Keypad 3")     PORT_CODE(KEYCODE_3_PAD)     PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON6  ) PORT_NAME("P2 Keypad 6")     PORT_CODE(KEYCODE_6_PAD)     PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON9  ) PORT_NAME("P2 Keypad 9")     PORT_CODE(KEYCODE_9_PAD)     PORT_PLAYER(2)
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON12 ) PORT_NAME("P2 Keypad Enter") PORT_CODE(KEYCODE_ENTER_PAD) PORT_PLAYER(2)
	PORT_START("controller2_extra")
	PORT_BIT( 0xff, 0xf0, IPT_UNUSED) // used in palladium

/* FIXME: the joystick are analog - the actual definition is merely an hack */

#if 0
    // shit, auto centering too slow, so only using 5 bits, and scaling at videoside
    PORT_START("controller1_joy_x")
    PORT_BIT( 0x1fe,0x10,IPT_AD_STICK_X)
    PORT_SENSITIVITY(1)
    PORT_KEYDELTA(2000)
    PORT_MINMAX(0,0x1f)
    PORT_CODE_DEC(KEYCODE_LEFT)
    PORT_CODE_INC(KEYCODE_RIGHT)
    PORT_CODE_DEC(JOYCODE_1_LEFT)
    PORT_CODE_INC(JOYCODE_1_RIGHT)
    PORT_PLAYER(1)
    PORT_RESET

    PORT_START("controller1_joy_y")
    PORT_BIT( 0x1fe,0x10,IPT_AD_STICK_Y)
    PORT_SENSITIVITY(1)
    PORT_KEYDELTA(2000)
    PORT_MINMAX(0,0x1f)
    PORT_CODE_DEC(KEYCODE_UP)
    PORT_CODE_INC(KEYCODE_DOWN)
    PORT_CODE_DEC(JOYCODE_1_UP)
    PORT_CODE_INC(JOYCODE_1_DOWN)
    PORT_PLAYER(1)
    PORT_RESET

    PORT_START("controller2_joy_x")
    PORT_BIT( 0x1ff,0x10,IPT_AD_STICK_X)
    PORT_SENSITIVITY(100)
    PORT_KEYDELTA(10)
    PORT_MINMAX(0,0x1f)
    PORT_CODE_DEC(KEYCODE_DEL)
    PORT_CODE_INC(KEYCODE_PGDN)
    PORT_CODE_DEC(JOYCODE_2_LEFT)
    PORT_CODE_INC(JOYCODE_2_RIGHT)
    PORT_PLAYER(2)
    PORT_RESET

    PORT_START("controller2_joy_y")
    PORT_BIT( 0x1ff,0x10,IPT_AD_STICK_Y)
    PORT_SENSITIVITY(100)
    PORT_KEYDELTA(10)
    PORT_MINMAX(0,0x1f)
    PORT_CODE_DEC(KEYCODE_HOME)
    PORT_CODE_INC(KEYCODE_END)
    PORT_CODE_DEC(JOYCODE_2_UP)
    PORT_CODE_INC(JOYCODE_2_DOWN)
    PORT_PLAYER(2)
    PORT_RESET
#else
	PORT_START("joysticks")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT )		PORT_PLAYER(1) PORT_8WAY
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT )	PORT_PLAYER(1) PORT_8WAY
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN )		PORT_PLAYER(1) PORT_8WAY
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP )		PORT_PLAYER(1) PORT_8WAY
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT )		PORT_PLAYER(2) PORT_8WAY
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT )	PORT_PLAYER(2) PORT_8WAY
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN )		PORT_PLAYER(2) PORT_8WAY
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP )		PORT_PLAYER(2) PORT_8WAY
#endif
INPUT_PORTS_END

/* The Palladium Video-Computer-Game controllers have no fire buttons on their side
   but have a 16 keys unlabeled keypad. The keys are fully independent and layed out
   according to this weird scheme, which keeps backward compatibility to Arcadia 2001's
   electrical wiring.

        2
    13 14 15
     1 16  3
     4  5  6
     7  8  9
    10 11 12                                                                           */

static INPUT_PORTS_START( vcg )
	PORT_START("panel")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_OTHER )    PORT_NAME("Start")        PORT_CODE(KEYCODE_S)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_OTHER )    PORT_NAME("Selector A")   PORT_CODE(KEYCODE_A)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER )    PORT_NAME("Selector B")   PORT_CODE(KEYCODE_B)
//  PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OTHER )    PORT_NAME("Reset")        PORT_CODE(KEYCODE_R)         Not implemented
	PORT_START("controller1_col1")
	PORT_BIT( 0xf0, 0xf0, IPT_UNUSED) // some bits must be high
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON1  ) PORT_NAME("P1 Keypad 1")  PORT_CODE(KEYCODE_1)         PORT_PLAYER(1)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON4  ) PORT_NAME("P1 Keypad 4")  PORT_CODE(KEYCODE_4)         PORT_PLAYER(1)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON7  ) PORT_NAME("P1 Keypad 7")  PORT_CODE(KEYCODE_7)         PORT_PLAYER(1)
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON10 ) PORT_NAME("P1 Keypad 10") PORT_CODE(KEYCODE_DEL)       PORT_PLAYER(1)
	PORT_START("controller1_col2")
	PORT_BIT( 0xf0, 0xf0, IPT_UNUSED)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON2  ) PORT_NAME("P1 Keypad 2")  PORT_CODE(KEYCODE_2)         PORT_PLAYER(1)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON5  ) PORT_NAME("P1 Keypad 5")  PORT_CODE(KEYCODE_5)         PORT_PLAYER(1)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON8  ) PORT_NAME("P1 Keypad 8")  PORT_CODE(KEYCODE_8)         PORT_PLAYER(1)
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON11 ) PORT_NAME("P1 Keypad 11") PORT_CODE(KEYCODE_0)         PORT_PLAYER(1)
	PORT_START("controller1_col3")
	PORT_BIT( 0xf0, 0xf0, IPT_UNUSED)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON3  ) PORT_NAME("P1 Keypad 3")  PORT_CODE(KEYCODE_3)         PORT_PLAYER(1)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON6  ) PORT_NAME("P1 Keypad 6")  PORT_CODE(KEYCODE_6)         PORT_PLAYER(1)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON9  ) PORT_NAME("P1 Keypad 9")  PORT_CODE(KEYCODE_9)         PORT_PLAYER(1)
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON12 ) PORT_NAME("P1 Keypad 12") PORT_CODE(KEYCODE_ENTER)     PORT_PLAYER(1)
	PORT_START("controller1_extra")
	PORT_BIT( 0xf0, 0xf0, IPT_UNUSED)
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON13 ) PORT_NAME("P1 Keypad 13") PORT_CODE(KEYCODE_U)         PORT_PLAYER(1)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON14 ) PORT_NAME("P1 Keypad 14") PORT_CODE(KEYCODE_I)         PORT_PLAYER(1)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON15 ) PORT_NAME("P1 Keypad 15") PORT_CODE(KEYCODE_O)         PORT_PLAYER(1)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON16 ) PORT_NAME("P1 Keypad 16") PORT_CODE(KEYCODE_K)         PORT_PLAYER(1)
	PORT_START("controller2_col1")
	PORT_BIT( 0xf0, 0xf0, IPT_UNUSED)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON1  ) PORT_NAME("P2 Keypad 1")  PORT_CODE(KEYCODE_1_PAD)     PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON4  ) PORT_NAME("P2 Keypad 4")  PORT_CODE(KEYCODE_4_PAD)     PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON7  ) PORT_NAME("P2 Keypad 7")  PORT_CODE(KEYCODE_7_PAD)     PORT_PLAYER(2)
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON10 ) PORT_NAME("P2 Keypad 10") PORT_CODE(KEYCODE_DEL_PAD)   PORT_PLAYER(2)
	PORT_START("controller2_col2")
	PORT_BIT( 0xf0, 0xf0, IPT_UNUSED)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON2  ) PORT_NAME("P2 Keypad 2")  PORT_CODE(KEYCODE_2_PAD)     PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON5  ) PORT_NAME("P2 Keypad 5")  PORT_CODE(KEYCODE_5_PAD)     PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON8  ) PORT_NAME("P2 Keypad 8")  PORT_CODE(KEYCODE_8_PAD)     PORT_PLAYER(2)
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON11 ) PORT_NAME("P2 Keypad 11") PORT_CODE(KEYCODE_0_PAD)     PORT_PLAYER(2)
	PORT_START("controller2_col3")
	PORT_BIT( 0xf0, 0xf0, IPT_UNUSED)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON3  ) PORT_NAME("P2 Keypad 3")  PORT_CODE(KEYCODE_3_PAD)     PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON6  ) PORT_NAME("P2 Keypad 6")  PORT_CODE(KEYCODE_6_PAD)     PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON9  ) PORT_NAME("P2 Keypad 9")  PORT_CODE(KEYCODE_9_PAD)     PORT_PLAYER(2)
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON12 ) PORT_NAME("P2 Keypad 12") PORT_CODE(KEYCODE_ENTER_PAD) PORT_PLAYER(2)
	PORT_START("controller2_extra")
	PORT_BIT( 0xf0, 0xf0, IPT_UNUSED)
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON13 ) PORT_NAME("P2 Keypad 13") PORT_CODE(KEYCODE_SLASH_PAD) PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON14 ) PORT_NAME("P2 Keypad 14") PORT_CODE(KEYCODE_ASTERISK)  PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON15 ) PORT_NAME("P2 Keypad 15") PORT_CODE(KEYCODE_MINUS_PAD) PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON16 ) PORT_NAME("P2 Keypad 16") PORT_CODE(KEYCODE_PLUS_PAD)  PORT_PLAYER(2)

/* FIXME: the joystick are analog - the actual definition is merely an hack */

#if 0
    // shit, auto centering too slow, so only using 5 bits, and scaling at videoside
    PORT_START("controller1_joy_x")
    PORT_BIT( 0x1fe,0x10,IPT_AD_STICK_X)
    PORT_SENSITIVITY(1)
    PORT_KEYDELTA(2000)
    PORT_MINMAX(0,0x1f)
    PORT_CODE_DEC(KEYCODE_LEFT)
    PORT_CODE_INC(KEYCODE_RIGHT)
    PORT_CODE_DEC(JOYCODE_1_LEFT)
    PORT_CODE_INC(JOYCODE_1_RIGHT)
    PORT_PLAYER(1)
    PORT_RESET

    PORT_START("controller1_joy_y")
    PORT_BIT( 0x1fe,0x10,IPT_AD_STICK_Y)
    PORT_SENSITIVITY(1)
    PORT_KEYDELTA(2000)
    PORT_MINMAX(0,0x1f)
    PORT_CODE_DEC(KEYCODE_UP)
    PORT_CODE_INC(KEYCODE_DOWN)
    PORT_CODE_DEC(JOYCODE_1_UP)
    PORT_CODE_INC(JOYCODE_1_DOWN)
    PORT_PLAYER(1)
    PORT_RESET

    PORT_START("controller2_joy_x")
    PORT_BIT( 0x1ff,0x10,IPT_AD_STICK_X)
    PORT_SENSITIVITY(100)
    PORT_KEYDELTA(10)
    PORT_MINMAX(0,0x1f)
    PORT_CODE_DEC(KEYCODE_DEL)
    PORT_CODE_INC(KEYCODE_PGDN)
    PORT_CODE_DEC(JOYCODE_2_LEFT)
    PORT_CODE_INC(JOYCODE_2_RIGHT)
    PORT_PLAYER(2)
    PORT_RESET

    PORT_START("controller2_joy_y")
    PORT_BIT( 0x1ff,0x10,IPT_AD_STICK_Y)
    PORT_SENSITIVITY(100)
    PORT_KEYDELTA(10)
    PORT_MINMAX(0,0x1f)
    PORT_CODE_DEC(KEYCODE_HOME)
    PORT_CODE_INC(KEYCODE_END)
    PORT_CODE_DEC(JOYCODE_2_UP)
    PORT_CODE_INC(JOYCODE_2_DOWN)
    PORT_PLAYER(2)
    PORT_RESET
#else
	PORT_START("joysticks")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT )		PORT_PLAYER(1) PORT_8WAY
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT )	PORT_PLAYER(1) PORT_8WAY
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN )		PORT_PLAYER(1) PORT_8WAY
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP )		PORT_PLAYER(1) PORT_8WAY
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT )		PORT_PLAYER(2) PORT_8WAY
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT )	PORT_PLAYER(2) PORT_8WAY
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN )		PORT_PLAYER(2) PORT_8WAY
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP )		PORT_PLAYER(2) PORT_8WAY
#endif
INPUT_PORTS_END

static const gfx_layout arcadia_charlayout =
{
	8,						/*width*/
	1,						/*heigth*/
	256,					/* 256 characters */
	1,						/* 1 bits per pixel */
	{ 0 },                  /* no bitplanes; 1 bit per pixel */
	{ 0, 1, 2, 3, 4, 5, 6, 7 },	/* x offsets */
	{ 0 },					/* y offsets */
	1*8
};

static GFXDECODE_START( arcadia )
	GFXDECODE_ENTRY( "gfx1", 0x0000, arcadia_charlayout, 0, 68 )
GFXDECODE_END

static const rgb_t arcadia_colors[] =
{
	RGB_WHITE,					/* white */
	MAKE_RGB(0xff, 0xff, 0x00),	/* yellow */
	MAKE_RGB(0x00, 0xff, 0xff),	/* cyan */
	MAKE_RGB(0x00, 0xff, 0x00),	/* green */
	MAKE_RGB(0xff, 0x00, 0xff),	/* magenta */
	MAKE_RGB(0xff, 0x00, 0x00),	/* red */
	MAKE_RGB(0x00, 0x00, 0xff),	/* blue */
	RGB_BLACK					/* black */
};

static const unsigned short arcadia_palette[128+8] =  /* bgnd, fgnd */
{
	0,1,2,3,4,5,6,7,

	0,0, 0,1, 0,2, 0,3, 0,4, 0,5, 0,6, 0,7,
	1,0, 1,1, 1,2, 1,3, 1,4, 1,5, 1,6, 1,7,
	2,0, 2,1, 2,2, 2,3, 2,4, 2,5, 2,6, 2,7,
	3,0, 3,1, 3,2, 3,3, 3,4, 3,5, 3,6, 3,7,
	4,0, 4,1, 4,2, 4,3, 4,4, 4,5, 4,6, 4,7,
	5,0, 5,1, 5,2, 5,3, 5,4, 5,5, 5,6, 5,7,
	6,0, 6,1, 6,2, 6,3, 6,4, 6,5, 6,6, 6,7,
	7,0, 7,1, 7,2, 7,3, 7,4, 7,5, 7,6, 7,7
};

static PALETTE_INIT( arcadia )
{
	int i;

	machine->colortable = colortable_alloc(machine, 8);

	for (i = 0; i < 8; i++)
		colortable_palette_set_color(machine->colortable, i, arcadia_colors[i]);

	for (i = 0; i < 128+8; i++)
		colortable_entry_set_value(machine->colortable, i, arcadia_palette[i]);
}

static DEVICE_IMAGE_LOAD( arcadia_cart )
{
	UINT8 *rom = memory_region(image->machine, "maincpu");
	int size;

	memset(rom, 0, 0x8000);
	size = image_length(image);

	if (size > memory_region_length(image->machine, "maincpu"))
		size = memory_region_length(image->machine, "maincpu");

	if (image_fread(image, rom, size) != size)
		return INIT_FAIL;

	if (size > 0x1000)
		memmove(rom + 0x2000, rom + 0x1000, size - 0x1000);
        if (size > 0x2000)
                memmove(rom + 0x4000, rom + 0x3000, size - 0x2000);

#if 1
	// golf cartridge support
	// 4kbyte at 0x0000
	// 2kbyte at 0x4000
        if (size<=0x2000) memcpy (rom+0x4000, rom+0x2000, 0x1000);
#else
	/* this is a testpatch for the golf cartridge
       so it could be burned in a arcadia 2001 cartridge
       activate it and use debugger to save patched version */
	// not enough yet (some pointers stored as data?)
	struct { UINT16 address; UINT8 old; UINT8 neu; }
	patch[]= {
		{ 0x0077,0x40,0x20 },
		{ 0x011e,0x40,0x20 },
		{ 0x0348,0x40,0x20 },
		{ 0x03be,0x40,0x20 },
		{ 0x04ce,0x40,0x20 },
		{ 0x04da,0x40,0x20 },
		{ 0x0562,0x42,0x22 },
		{ 0x0617,0x40,0x20 },
		{ 0x0822,0x40,0x20 },
		{ 0x095e,0x42,0x22 },
		{ 0x09d3,0x42,0x22 },
		{ 0x0bb0,0x42,0x22 },
		{ 0x0efb,0x40,0x20 },
		{ 0x0ec1,0x43,0x23 },
		{ 0x0f00,0x40,0x20 },
		{ 0x0f12,0x40,0x20 },
		{ 0x0ff5,0x43,0x23 },
		{ 0x0ff7,0x41,0x21 },
		{ 0x0ff9,0x40,0x20 },
		{ 0x0ffb,0x41,0x21 },
		{ 0x20ec,0x42,0x22 }
	};
	for (int i=0; i<ARRAY_LENGTH(patch); i++) {
	    assert(rom[patch[i].address]==patch[i].old);
	    rom[patch[i].address]=patch[i].neu;
	}
#endif
	return INIT_PASS;
}

static MACHINE_DRIVER_START( arcadia )
	/* basic machine hardware */
	MDRV_CPU_ADD("maincpu", S2650, 3580000/4)        /* 0.895 MHz */
	MDRV_CPU_PROGRAM_MAP(arcadia_mem)
	MDRV_CPU_IO_MAP(arcadia_io)
	MDRV_CPU_PERIODIC_INT(arcadia_video_line, 262*60)
	MDRV_QUANTUM_TIME(HZ(60))

    /* video hardware */
	MDRV_SCREEN_ADD("screen", RASTER)
	MDRV_SCREEN_REFRESH_RATE(60)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(128+2*XPOS, 262)
	MDRV_SCREEN_VISIBLE_AREA(0, 2*XPOS+128-1, 0, 262-1)
	MDRV_GFXDECODE( arcadia )
	MDRV_PALETTE_LENGTH(ARRAY_LENGTH(arcadia_palette))
	MDRV_PALETTE_INIT( arcadia )

	MDRV_VIDEO_START( arcadia )
	MDRV_VIDEO_UPDATE( arcadia )

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")
	MDRV_SOUND_ADD("custom", ARCADIA, 0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.00)

	/* cartridge */
	MDRV_CARTSLOT_ADD("cart")
	MDRV_CARTSLOT_EXTENSION_LIST("bin")
	MDRV_CARTSLOT_NOT_MANDATORY
	MDRV_CARTSLOT_LOAD(arcadia_cart)
MACHINE_DRIVER_END


ROM_START(arcadia)
	ROM_REGION(0x8000,"maincpu", ROMREGION_ERASEFF)
	ROM_REGION(0x100,"gfx1", ROMREGION_ERASEFF)
ROM_END

ROM_START(vcg)
	ROM_REGION(0x8000,"maincpu", ROMREGION_ERASEFF)
	ROM_REGION(0x100,"gfx1", ROMREGION_ERASEFF)
ROM_END


/***************************************************************************

  Game driver(s)

***************************************************************************/

static DRIVER_INIT( arcadia )
{
	int i;
	UINT8 *gfx=memory_region(machine, "gfx1");
	for (i=0; i<256; i++) gfx[i]=i;
#if 0
	// this is here to allow developement of some simple testroutines
	// for a real console
	{
	    UINT8 *rom=memory_region(machine, "maincpu");
	    /* this is a simple routine to display all rom characters
           on the display for a snapshot */
	    static const UINT8 prog[]={ // address 0 of course
		0x20, // eorz, 0
		0x1b, 0x01, // bctr,a $0004
		0x17, // retc a
		0x76, 0x20, // ppsu ii

		// fill screen
		0x04, 0x00, // lodi,0 0
		0x04|1, 0x00, // lodi,1 0
		0xcc|1, 0x78, 0x10, //a: stra,0 $1800,r1
		0x75,9, //cpsl wc|c
		0x84,0x41, // addi,0 0x41
		0x75,9, //cpsl wc|c
		0x84|1, 0x01, // addi,1 1
		0xe4|1, 0x40, // comi,1 40
		0x98, 0x80-15, // bcfr,0 a

		0x04, 0xff, // lodi,0 7
		0xcc, 0x18, 0xfc, // stra,0 $19f8
		0x04, 0x00, // lodi,0 7
		0xcc, 0x18, 0xfd, // stra,0 $18fd
		0x04, 0x07, // lodi,0 7
		0xcc, 0x19, 0xf9, // stra,0 $19f9

		0x04, 0x00, // lodi,0 7
		0xcc, 0x19, 0xbe, // stra,0 $19bf
		0x04, 0x00, // lodi,0 7
		0xcc, 0x19, 0xbf, // stra,0 $19bf

		//loop: 0x0021
		// print keyboards
		0x04|1, 0x00, //y:lodi,1 0
		0x0c|1, 0x79, 0x00, //x: ldra,0 1900,r1
		0x44|0, 0x0f, //andi,0 0f
		0x64|0, 0x10, //ori,0  10
		0xcc|1, 0x78, 0x01, //stra,0 1840,r1
		0x75,9, //cpsl wc|c
		0x84|1, 0x01, //addi,1 1
		0xe4|1, 0x09, //comi,1 9
		0x98, 0x80-18, //bcfr,0 x

		// cycle colors
		0x0c|1, 0x19, 0x00, //ldra,1 1900
		0x44|1, 0xf, //andi,0 0f
		0xe4|1, 1, //comi,1 1
		0x98, +10, //bcfr,0 c
		0x0c, 0x19, 0xbf,//ldra,0 19f9,0
		0x84, 1, //addi,0 1
		0xcc, 0x19, 0xbf, //stra,0 19f9,0
		0x18|3, 12, // bctr,a
		0xe4|1, 2, //c:comi,1 2
		0x98, +10, //bcfr,0 d
		0x0c, 0x19, 0xbf, //ldra,0 19f9,0
		0x84, 8, //addi,0 8
		0xcc, 0x19, 0xbf, //stra,0 19f9,0
		0x18|3, 12, // bctr,a

		// cycle colors
		0xe4|1, 4, //comi,1 4
		0x98, +10, //bcfr,0 c
		0x0c, 0x19, 0xbe,//ldra,0 19f9,0
		0x84, 1, //addi,0 1
		0xcc, 0x19, 0xbe, //stra,0 19f9,0
		0x18|3, 12, // bctr,a
		0xe4|1, 8, //c:comi,1 2
		0x98, +8+9, //bcfr,0 d
		0x0c, 0x19, 0xbe, //ldra,0 19f9,0
		0x84, 8, //addi,0 8
		0xcc, 0x19, 0xbe, //stra,0 19f9,0

		0x0c, 0x19, 0x00, //b: ldra,0 1900
		0x44|0, 0xf, //andi,0 0f
		0xe4, 0, //comi,0 0
		0x98, 0x80-9, //bcfr,0 b

		0x0c, 0x19, 0xbe, //ldra,0 19bf
		0xcc, 0x19, 0xf8, //stra,0 19f8
		0x0c, 0x19, 0xbf, //ldra,0 19bf
		0xcc, 0x19, 0xf9, //stra,0 19f8

		0x0c, 0x19, 0xbe, //ldra,0 17ff
		0x44|0, 0xf, //andi,0 7
		0x64|0, 0x10, //ori,0  10
		0xcc, 0x18, 0x0d, //stra,0 180f
		0x0c, 0x19, 0xbe, //x: ldra,0 19bf
		0x50, 0x50, 0x50, 0x50, //shr,0 4
		0x44|0, 0xf, //andi,0 7
		0x64|0, 0x10, //ori,0  10
		0xcc, 0x18, 0x0c, //stra,0 180e

		0x0c, 0x19, 0xbf, //ldra,0 17ff
		0x44|0, 0xf, //andi,0 7
		0x64|0, 0x10, //ori,0  10
		0xcc, 0x18, 0x0f, //stra,0 180f
		0x0c, 0x19, 0xbf, //x: ldra,0 19bf
		0x50, 0x50, 0x50, 0x50, //shr,0 4
		0x44|0, 0xf, //andi,0 7
		0x64|0, 0x10, //ori,0  10
		0xcc, 0x18, 0x0e, //stra,0 180e

		0x0c, 0x18, 0x00, //ldra,0 1800
		0x84, 1, //addi,0 1
		0xcc, 0x18, 0x00, //stra,0 1800

//      0x1b, 0x80-20-29-26-9-8-2 // bctr,a y
		0x1c|3, 0, 0x32, // bcta,3 loop

		// calling too many subdirectories causes cpu to reset!
		// bxa causes trap
	    };
#if 1
	    FILE *f;
	    f=fopen("chartest.bin","wb");
	    fwrite(prog, ARRAY_LENGTH(prog), sizeof(prog[0]), f);
	    fclose(f);
#endif
	    for (i=0; i<ARRAY_LENGTH(prog); i++) rom[i]=prog[i];

	}
#endif
}

/*    YEAR  NAME        PARENT      COMPAT  MACHINE   INPUT     INIT        COMPANY     FULLNAME */
// marketed from several firms/names

CONS(1982,	arcadia,	0,			0,		arcadia,  arcadia,  arcadia,	"Emerson",		"Arcadia 2001", GAME_IMPERFECT_SOUND )
// schmid tvg 2000 (developer? PAL)

// different cartridge connector
// hanimex mpt 03 model

// different cartridge connector (same size as mpt03, but different pinout!)
// 16 keys instead of 12
CONS(198?, vcg,		arcadia,	0,		arcadia,  vcg,		arcadia,	"Palladium",		"Video-Computer-Game", GAME_IMPERFECT_SOUND | GAME_NOT_WORKING )
