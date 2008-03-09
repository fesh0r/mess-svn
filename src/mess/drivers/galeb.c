/***************************************************************************

		Galeb driver by Miodrag Milanovic

		01/03/2008 Updated to work with latest SVN code
		23/02/2008 Sound support added.
		22/02/2008 Preliminary driver.

		Driver is based on work of Josip Perusanec
		     
****************************************************************************/


#include "driver.h"
#include "cpu/m6502/m6502.h"
#include "sound/dac.h"
#include "includes/galeb.h"
  
GFXDECODE_START( galeb )
	GFXDECODE_ENTRY( REGION_GFX1, 0x0000, galeb_charlayout, 0, 1 )
GFXDECODE_END
  
/* Address maps */
static ADDRESS_MAP_START(galeb_mem, ADDRESS_SPACE_PROGRAM, 8)
    AM_RANGE( 0x0000, 0x1fff ) AM_RAM  // RAM
    AM_RANGE( 0xbfe0, 0xbfe7 ) AM_READ ( galeb_keyboard_r )    
    AM_RANGE( 0xbfe0, 0xbfe0 ) AM_WRITE( galeb_speaker_w  )    
    AM_RANGE( 0xb000, 0xb3ff ) AM_RAM  // video ram  
    AM_RANGE( 0xc000, 0xc7ff ) AM_ROM  // BASIC 01 ROM
    AM_RANGE( 0xc800, 0xcfff ) AM_ROM  // BASIC 02 ROM
    AM_RANGE( 0xd000, 0xd7ff ) AM_ROM  // BASIC 03 ROM
    AM_RANGE( 0xd800, 0xdfff ) AM_ROM  // BASIC 04 ROM
    AM_RANGE( 0xf000, 0xf7ff ) AM_ROM  // Monitor ROM
    AM_RANGE( 0xf800, 0xffff ) AM_ROM  // System ROM
ADDRESS_MAP_END

/* Input ports */
INPUT_PORTS_START( galeb )
	PORT_START /* line 0 */
		PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Delete") PORT_CODE(KEYCODE_BACKSPACE)
		PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Space") PORT_CODE(KEYCODE_SPACE)
		PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Enter") PORT_CODE(KEYCODE_ENTER)
		PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("ESC") PORT_CODE(KEYCODE_ESC)
		PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_UNUSED)
		PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("CTL") PORT_CODE(KEYCODE_RCONTROL)
		PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("CTL") PORT_CODE(KEYCODE_LCONTROL)
		PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("GR") PORT_CODE(KEYCODE_LALT)
		PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("GR") PORT_CODE(KEYCODE_RALT)
		PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Shift") PORT_CODE(KEYCODE_RSHIFT)
		PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Shift") PORT_CODE(KEYCODE_LSHIFT)
	PORT_START /* line 1 */
		PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("^") PORT_CODE(KEYCODE_TILDE)
		PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Down - Up") PORT_CODE(KEYCODE_DOWN)
		PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME(":") PORT_CODE(KEYCODE_COLON)
		PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("1") PORT_CODE(KEYCODE_1)
		PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_UNUSED)
		PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Q") PORT_CODE(KEYCODE_Q)
		PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("A") PORT_CODE(KEYCODE_A)
		PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Y") PORT_CODE(KEYCODE_Z)
	PORT_START /* line 2 */
		PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Dj") PORT_CODE(KEYCODE_CLOSEBRACE)
		PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Left - Right") PORT_CODE(KEYCODE_LEFT)
		PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME(";") PORT_CODE(KEYCODE_EQUALS)
		PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("2") PORT_CODE(KEYCODE_2)
		PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("/") PORT_CODE(KEYCODE_SLASH)
		PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("W") PORT_CODE(KEYCODE_W)
		PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("S") PORT_CODE(KEYCODE_S)
		PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("X") PORT_CODE(KEYCODE_X)
	PORT_START /* line 3 */
		PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Sh") PORT_CODE(KEYCODE_OPENBRACE)
		PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("-")  PORT_CODE(KEYCODE_MINUS)
		PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Cj") PORT_CODE(KEYCODE_QUOTE)
		PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("3")  PORT_CODE(KEYCODE_3)
		PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Zh") PORT_CODE(KEYCODE_BACKSLASH)
		PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("E") PORT_CODE(KEYCODE_E)
		PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("D") PORT_CODE(KEYCODE_D)
		PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("C") PORT_CODE(KEYCODE_C)
	PORT_START /* line 4 */
		PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("P") PORT_CODE(KEYCODE_P)
		PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("0") PORT_CODE(KEYCODE_0) 
		PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Ch") PORT_CODE(KEYCODE_COLON)
		PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("4") PORT_CODE(KEYCODE_4)
		PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME(".") PORT_CODE(KEYCODE_STOP)
		PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("R") PORT_CODE(KEYCODE_R)
		PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("F") PORT_CODE(KEYCODE_F)
		PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("V") PORT_CODE(KEYCODE_V)
	PORT_START /* line 5 */
		PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("O") PORT_CODE(KEYCODE_O)
		PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("9") PORT_CODE(KEYCODE_9)
		PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("L") PORT_CODE(KEYCODE_L)
		PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("5") PORT_CODE(KEYCODE_5)
		PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME(",") PORT_CODE(KEYCODE_COMMA)
		PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("T") PORT_CODE(KEYCODE_T)
		PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("G") PORT_CODE(KEYCODE_G)
		PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("B") PORT_CODE(KEYCODE_B)
	PORT_START /* line 6 */
		PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_UNUSED)
		PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_UNUSED)
		PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("J") PORT_CODE(KEYCODE_J)
		PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("6") PORT_CODE(KEYCODE_6)
		PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_UNUSED)
		PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Z") PORT_CODE(KEYCODE_Y)
		PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("H") PORT_CODE(KEYCODE_H)
		PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("N") PORT_CODE(KEYCODE_N)
	PORT_START /* line 7 */
		PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("I") PORT_CODE(KEYCODE_I)
		PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("8") PORT_CODE(KEYCODE_8)
		PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("K") PORT_CODE(KEYCODE_K)
		PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("7") PORT_CODE(KEYCODE_7)
		PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("M") PORT_CODE(KEYCODE_M)
		PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("U") PORT_CODE(KEYCODE_U)
		PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_UNUSED)
		PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_UNUSED)
INPUT_PORTS_END
 
/* Machine driver */
static MACHINE_DRIVER_START( galeb )
    /* basic machine hardware */
    MDRV_CPU_ADD_TAG("main", M6502, 1000000)    
    MDRV_CPU_PROGRAM_MAP(galeb_mem, 0) 
    MDRV_MACHINE_RESET( galeb )
 		
    /* video hardware */    	
		MDRV_SCREEN_ADD("main", RASTER)      	
		MDRV_SCREEN_REFRESH_RATE(50)
		MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500)) /* not accurate */
		MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
		MDRV_SCREEN_SIZE(48*8, 16*8)
		MDRV_SCREEN_VISIBLE_AREA(0, 48*8-1, 0, 16*8-1)
		MDRV_GFXDECODE( galeb )
		
		MDRV_PALETTE_LENGTH(2)
		MDRV_PALETTE_INIT(black_and_white)
 
    MDRV_VIDEO_START(galeb)
    MDRV_VIDEO_UPDATE(galeb)
    
    /* audio hardware */
		MDRV_SPEAKER_STANDARD_MONO("mono")
		MDRV_SOUND_ADD(DAC, 0)
		MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 8.00)	    
MACHINE_DRIVER_END
 
/* ROM definition */
ROM_START( galeb )
    ROM_REGION( 0x10000, REGION_CPU1, ROMREGION_ERASEFF )
    ROM_LOAD( "bas01.rom", 0xc000, 0x0800, CRC(9b19ed58) SHA1(ebfc27af8dbabfb233f9888e6a0a0dfc87ae1691) )
    ROM_LOAD( "bas02.rom", 0xc800, 0x0800, CRC(3f320a84) SHA1(4ea082b4269dca6152426b1f720c7508122d3cb7) )
    ROM_LOAD( "bas03.rom", 0xd000, 0x0800, CRC(f122ad10) SHA1(3c7c1dd67268230d179a00b0f8b35be80c2b7035) )
    ROM_LOAD( "bas04.rom", 0xd800, 0x0800, CRC(b5372a83) SHA1(f93b73d98b943c6791f46617418fb5e4238d75bd) )
    ROM_LOAD( "exmd.rom",  0xf000, 0x0800, CRC(1bcb1375) SHA1(fda3361d238720a3d309644093da9832d5aff661) )    
    ROM_LOAD( "makbug.rom",0xf800, 0x0800, CRC(91e38e79) SHA1(2b6439a09a470cda9c81b9d453c6380b99716989) )
		ROM_REGION(0x0800, REGION_GFX1,0)
		ROM_LOAD ("chrgen.bin", 0x0000, 0x0800, CRC(409a800e) SHA1(0efe429dd6c0568032636e691d9865a623afeb55))
        
ROM_END

/* Driver */
 
/*    YEAR  NAME   PARENT  COMPAT  MACHINE 	INPUT   INIT  CONFIG COMPANY 				 FULLNAME   FLAGS */
COMP( 1981, galeb,     0,      0, 		galeb, 	galeb, 	galeb, NULL,  "PEL Varazdin", "Galeb",		 0)
