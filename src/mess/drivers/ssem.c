/*
    Manchester Small-Scale Experimental Machine (SSEM)

    Driver by MooglyGuy
*/

#include "driver.h"
#include "cpu/ssem/ssem.h"
#include "devices/cartslot.h"
#include <stdarg.h>

static UINT8 *store;
static UINT8 store_line = 0;

/****************************************************\
* General helper functions                           *
\****************************************************/

// The SSEM stores its data, visually, with the leftmost bit corresponding to the least significant bit.
// The de facto snapshot format for other SSEM simulators stores the data physically in that format as well.
// Therefore, in MESS, every 32-bit word has its bits reversed, too, and as a result the values must be
// un-reversed before being used.
INLINE UINT32 reverse(UINT32 v)
{
    // Taken from http://www-graphics.stanford.edu/~seander/bithacks.html#ReverseParallel
    // swap odd and even bits
    v = ((v >> 1) & 0x55555555) | ((v & 0x55555555) << 1);
    // swap consecutive pairs
    v = ((v >> 2) & 0x33333333) | ((v & 0x33333333) << 2);
    // swap nibbles ...
    v = ((v >> 4) & 0x0F0F0F0F) | ((v & 0x0F0F0F0F) << 4);
    // swap bytes
    v = ((v >> 8) & 0x00FF00FF) | ((v & 0x00FF00FF) << 8);
    // swap 2-byte long pairs
    v = ( v >> 16             ) | ( v               << 16);

    return v;
}

/****************************************************\
* Address map                                        *
\****************************************************/

static ADDRESS_MAP_START( ssem_map, ADDRESS_SPACE_PROGRAM, 8 )
    AM_RANGE(0x00, 0x7f) AM_RAM AM_BASE(&store)// Primary store
ADDRESS_MAP_END

/****************************************************\
* Input ports and front panel handling               *
\****************************************************/

enum
{
    // Bit-edit buttons
    PANEL_BIT0,  PANEL_BIT1,  PANEL_BIT2,  PANEL_BIT3,  PANEL_BIT4,  PANEL_BIT5,  PANEL_BIT6,  PANEL_BIT7,
    PANEL_BIT8,  PANEL_BIT9,  PANEL_BIT10, PANEL_BIT11, PANEL_BIT12, PANEL_BIT13, PANEL_BIT14, PANEL_BIT15,
    PANEL_BIT16, PANEL_BIT17, PANEL_BIT18, PANEL_BIT19, PANEL_BIT20, PANEL_BIT21, PANEL_BIT22, PANEL_BIT23,
    PANEL_BIT24, PANEL_BIT25, PANEL_BIT26, PANEL_BIT27, PANEL_BIT28, PANEL_BIT29, PANEL_BIT30, PANEL_BIT31,

    // Page up, page down
    PANEL_PGUP, PANEL_PGDN,

    // Line up, line down
    PANEL_LNUP, PANEL_LNDN,

    // Halt
    PANEL_HALT,
};

static INPUT_CHANGED( panel_check )
{
    UINT8 edit0_state = input_port_read(field->port->machine, "EDIT0");
    UINT8 edit1_state = input_port_read(field->port->machine, "EDIT1");
    UINT8 edit2_state = input_port_read(field->port->machine, "EDIT2");
    UINT8 edit3_state = input_port_read(field->port->machine, "EDIT3");
    UINT8 misc_state = input_port_read(field->port->machine, "MISC");
    const device_config *ssem_cpu = devtag_get_device(field->port->machine, "maincpu");

    switch( (int)(FPTR)param )
    {
        case PANEL_BIT0:
            if(edit0_state & 0x01) store[(store_line << 2) | 0] ^= 0x80;
            break;
        case PANEL_BIT1:
            if(edit0_state & 0x02) store[(store_line << 2) | 0] ^= 0x40;
            break;
        case PANEL_BIT2:
            if(edit0_state & 0x04) store[(store_line << 2) | 0] ^= 0x20;
            break;
        case PANEL_BIT3:
            if(edit0_state & 0x08) store[(store_line << 2) | 0] ^= 0x10;
            break;
        case PANEL_BIT4:
            if(edit0_state & 0x10) store[(store_line << 2) | 0] ^= 0x08;
            break;
        case PANEL_BIT5:
            if(edit0_state & 0x20) store[(store_line << 2) | 0] ^= 0x04;
            break;
        case PANEL_BIT6:
            if(edit0_state & 0x40) store[(store_line << 2) | 0] ^= 0x02;
            break;
        case PANEL_BIT7:
            if(edit0_state & 0x80) store[(store_line << 2) | 0] ^= 0x01;
            break;
        case PANEL_BIT8:
            if(edit1_state & 0x01) store[(store_line << 2) | 1] ^= 0x80;
            break;
        case PANEL_BIT9:
            if(edit1_state & 0x02) store[(store_line << 2) | 1] ^= 0x40;
            break;
        case PANEL_BIT10:
            if(edit1_state & 0x04) store[(store_line << 2) | 1] ^= 0x20;
            break;
        case PANEL_BIT11:
            if(edit1_state & 0x08) store[(store_line << 2) | 1] ^= 0x10;
            break;
        case PANEL_BIT12:
            if(edit1_state & 0x10) store[(store_line << 2) | 1] ^= 0x08;
            break;
        case PANEL_BIT13:
            if(edit1_state & 0x20) store[(store_line << 2) | 1] ^= 0x04;
            break;
        case PANEL_BIT14:
            if(edit1_state & 0x40) store[(store_line << 2) | 1] ^= 0x02;
            break;
        case PANEL_BIT15:
            if(edit1_state & 0x80) store[(store_line << 2) | 1] ^= 0x01;
            break;
        case PANEL_BIT16:
            if(edit2_state & 0x01) store[(store_line << 2) | 2] ^= 0x80;
            break;
        case PANEL_BIT17:
            if(edit2_state & 0x02) store[(store_line << 2) | 2] ^= 0x40;
            break;
        case PANEL_BIT18:
            if(edit2_state & 0x04) store[(store_line << 2) | 2] ^= 0x20;
            break;
        case PANEL_BIT19:
            if(edit2_state & 0x08) store[(store_line << 2) | 2] ^= 0x10;
            break;
        case PANEL_BIT20:
            if(edit2_state & 0x10) store[(store_line << 2) | 2] ^= 0x08;
            break;
        case PANEL_BIT21:
            if(edit2_state & 0x20) store[(store_line << 2) | 2] ^= 0x04;
            break;
        case PANEL_BIT22:
            if(edit2_state & 0x40) store[(store_line << 2) | 2] ^= 0x02;
            break;
        case PANEL_BIT23:
            if(edit2_state & 0x80) store[(store_line << 2) | 2] ^= 0x01;
            break;
        case PANEL_BIT24:
            if(edit3_state & 0x01) store[(store_line << 2) | 3] ^= 0x80;
            break;
        case PANEL_BIT25:
            if(edit3_state & 0x02) store[(store_line << 2) | 3] ^= 0x40;
            break;
        case PANEL_BIT26:
            if(edit3_state & 0x04) store[(store_line << 2) | 3] ^= 0x20;
            break;
        case PANEL_BIT27:
            if(edit3_state & 0x08) store[(store_line << 2) | 3] ^= 0x10;
            break;
        case PANEL_BIT28:
            if(edit3_state & 0x10) store[(store_line << 2) | 3] ^= 0x08;
            break;
        case PANEL_BIT29:
            if(edit3_state & 0x20) store[(store_line << 2) | 3] ^= 0x04;
            break;
        case PANEL_BIT30:
            if(edit3_state & 0x40) store[(store_line << 2) | 3] ^= 0x02;
            break;
        case PANEL_BIT31:
            if(edit3_state & 0x80) store[(store_line << 2) | 3] ^= 0x01;
            break;
        case PANEL_LNUP:
            if(misc_state & 0x01)
            {
                store_line--;
            }
            break;
        case PANEL_LNDN:
            if(misc_state & 0x02)
            {
                store_line++;
            }
            break;
        case PANEL_HALT:
            if(misc_state & 0x04)
            {
                cpu_set_reg(ssem_cpu, SSEM_HALT, 1 - cpu_get_reg(ssem_cpu, SSEM_HALT));
            }
            break;
    }
}

static INPUT_PORTS_START( ssem )
    PORT_START("EDIT0")
        PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Bit 0")     PORT_CODE(KEYCODE_1) PORT_CHANGED(panel_check, (void*)PANEL_BIT0)
        PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Bit 1")     PORT_CODE(KEYCODE_2) PORT_CHANGED(panel_check, (void*)PANEL_BIT1)
        PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Bit 2")     PORT_CODE(KEYCODE_3) PORT_CHANGED(panel_check, (void*)PANEL_BIT2)
        PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Bit 3")     PORT_CODE(KEYCODE_4) PORT_CHANGED(panel_check, (void*)PANEL_BIT3)
        PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Bit 4")     PORT_CODE(KEYCODE_5) PORT_CHANGED(panel_check, (void*)PANEL_BIT4)
        PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Bit 5")     PORT_CODE(KEYCODE_6) PORT_CHANGED(panel_check, (void*)PANEL_BIT5)
        PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Bit 6")     PORT_CODE(KEYCODE_7) PORT_CHANGED(panel_check, (void*)PANEL_BIT6)
        PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Bit 7")     PORT_CODE(KEYCODE_8) PORT_CHANGED(panel_check, (void*)PANEL_BIT7)

    PORT_START("EDIT1")
        PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Bit 8")     PORT_CODE(KEYCODE_Q) PORT_CHANGED(panel_check, (void*)PANEL_BIT8)
        PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Bit 9")     PORT_CODE(KEYCODE_W) PORT_CHANGED(panel_check, (void*)PANEL_BIT9)
        PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Bit 10")    PORT_CODE(KEYCODE_E) PORT_CHANGED(panel_check, (void*)PANEL_BIT10)
        PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Bit 11")    PORT_CODE(KEYCODE_R) PORT_CHANGED(panel_check, (void*)PANEL_BIT11)
        PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Bit 12")    PORT_CODE(KEYCODE_T) PORT_CHANGED(panel_check, (void*)PANEL_BIT12)
        PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Bit 13")    PORT_CODE(KEYCODE_Y) PORT_CHANGED(panel_check, (void*)PANEL_BIT13)
        PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Bit 14")    PORT_CODE(KEYCODE_U) PORT_CHANGED(panel_check, (void*)PANEL_BIT14)
        PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Bit 15")    PORT_CODE(KEYCODE_I) PORT_CHANGED(panel_check, (void*)PANEL_BIT15)

    PORT_START("EDIT2")
        PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Bit 16")    PORT_CODE(KEYCODE_A) PORT_CHANGED(panel_check, (void*)PANEL_BIT16)
        PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Bit 17")    PORT_CODE(KEYCODE_S) PORT_CHANGED(panel_check, (void*)PANEL_BIT17)
        PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Bit 18")    PORT_CODE(KEYCODE_D) PORT_CHANGED(panel_check, (void*)PANEL_BIT18)
        PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Bit 19")    PORT_CODE(KEYCODE_F) PORT_CHANGED(panel_check, (void*)PANEL_BIT19)
        PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Bit 20")    PORT_CODE(KEYCODE_G) PORT_CHANGED(panel_check, (void*)PANEL_BIT20)
        PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Bit 21")    PORT_CODE(KEYCODE_H) PORT_CHANGED(panel_check, (void*)PANEL_BIT21)
        PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Bit 22")    PORT_CODE(KEYCODE_J) PORT_CHANGED(panel_check, (void*)PANEL_BIT22)
        PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Bit 23")    PORT_CODE(KEYCODE_K) PORT_CHANGED(panel_check, (void*)PANEL_BIT23)

    PORT_START("EDIT3")
        PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Bit 24")    PORT_CODE(KEYCODE_Z) PORT_CHANGED(panel_check, (void*)PANEL_BIT24)
        PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Bit 25")    PORT_CODE(KEYCODE_X) PORT_CHANGED(panel_check, (void*)PANEL_BIT25)
        PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Bit 26")    PORT_CODE(KEYCODE_C) PORT_CHANGED(panel_check, (void*)PANEL_BIT26)
        PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Bit 27")    PORT_CODE(KEYCODE_V) PORT_CHANGED(panel_check, (void*)PANEL_BIT27)
        PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Bit 28")    PORT_CODE(KEYCODE_B) PORT_CHANGED(panel_check, (void*)PANEL_BIT28)
        PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Bit 29")    PORT_CODE(KEYCODE_N) PORT_CHANGED(panel_check, (void*)PANEL_BIT29)
        PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Bit 30")    PORT_CODE(KEYCODE_M) PORT_CHANGED(panel_check, (void*)PANEL_BIT30)
        PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Bit 31")    PORT_CODE(KEYCODE_COMMA) PORT_CHANGED(panel_check, (void*)PANEL_BIT31)

    PORT_START("MISC")
        PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Line Up")   PORT_CODE(KEYCODE_UP)   PORT_CHANGED(panel_check, (void*)PANEL_LNUP)
        PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Line Down") PORT_CODE(KEYCODE_DOWN) PORT_CHANGED(panel_check, (void*)PANEL_LNDN)
        PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_BUTTON1) PORT_NAME("Halt") PORT_CHANGED(panel_check, (void*)PANEL_HALT)
        PORT_BIT(0xf8, IP_ACTIVE_HIGH, IPT_UNUSED)
INPUT_PORTS_END

/****************************************************\
* Video hardware                                     *
\****************************************************/

static const UINT8 char_glyphs[0x80][8] =
{
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x10, 0x38, 0x10, 0x00, 0x00, 0x00 },
    { 0x38, 0x7c, 0xfe, 0xfe, 0xfe, 0x7c, 0x38, 0x00 },
    { 0xff, 0xff, 0xef, 0xc7, 0xef, 0xff, 0xff, 0xff },
    { 0xc7, 0x83, 0x01, 0x01, 0x01, 0x83, 0xc7, 0xff },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x18, 0x00 },
    { 0x66, 0x66, 0xcc, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x6c, 0x6c, 0xfe, 0x6c, 0xfe, 0x6c, 0x6c, 0x00 },
    { 0x18, 0x3e, 0x60, 0x3c, 0x06, 0x7c, 0x18, 0x00 },
    { 0xc6, 0xcc, 0x18, 0x30, 0x60, 0xc6, 0x86, 0x00 },
    { 0x78, 0xcc, 0x78, 0x70, 0xce, 0xcc, 0x7e, 0x00 },
    { 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x0c, 0x18, 0x30, 0x30, 0x30, 0x18, 0x0c, 0x00 },
    { 0x30, 0x18, 0x0c, 0x0c, 0x0c, 0x18, 0x30, 0x00 },
    { 0x00, 0x66, 0x3c, 0xff, 0x3c, 0x66, 0x00, 0x00 },
    { 0x00, 0x18, 0x18, 0x7e, 0x18, 0x18, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x30 },
    { 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00 },
    { 0x02, 0x06, 0x0c, 0x18, 0x30, 0x60, 0xc0, 0x00 },
    { 0x3c, 0x66, 0x76, 0x7e, 0x6e, 0x66, 0x3c, 0x00 },
    { 0x18, 0x38, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x00 },
    { 0x3c, 0x66, 0x06, 0x0c, 0x30, 0x60, 0x7e, 0x00 },
    { 0x3c, 0x66, 0x06, 0x0c, 0x06, 0x66, 0x3c, 0x00 },
    { 0x66, 0x66, 0x66, 0x7e, 0x06, 0x06, 0x06, 0x00 },
    { 0x7e, 0x60, 0x7c, 0x06, 0x06, 0x66, 0x3c, 0x00 },
    { 0x3c, 0x66, 0x60, 0x7c, 0x66, 0x66, 0x3c, 0x00 },
    { 0x7e, 0x66, 0x06, 0x0c, 0x18, 0x18, 0x18, 0x00 },
    { 0x3c, 0x66, 0x66, 0x3c, 0x66, 0x66, 0x3c, 0x00 },
    { 0x3c, 0x66, 0x66, 0x3e, 0x06, 0x66, 0x3c, 0x00 },
    { 0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x00 },
    { 0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x30 },
    { 0x0c, 0x18, 0x30, 0x60, 0x30, 0x18, 0x0c, 0x00 },
    { 0x00, 0x00, 0x7e, 0x00, 0x7e, 0x00, 0x00, 0x00 },
    { 0x30, 0x18, 0x0c, 0x06, 0x0c, 0x18, 0x30, 0x00 },
    { 0x3c, 0x66, 0x06, 0x0c, 0x18, 0x00, 0x18, 0x00 },
    { 0x3c, 0x66, 0x6e, 0x6e, 0x60, 0x66, 0x3c, 0x00 },
    { 0x3c, 0x66, 0x66, 0x7e, 0x66, 0x66, 0x66, 0x00 },
    { 0x7c, 0x66, 0x66, 0x7c, 0x66, 0x66, 0x7c, 0x00 },
    { 0x3c, 0x66, 0x60, 0x60, 0x60, 0x66, 0x3c, 0x00 },
    { 0x7c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7c, 0x00 },
    { 0x7e, 0x60, 0x60, 0x78, 0x60, 0x60, 0x7e, 0x00 },
    { 0x7e, 0x60, 0x60, 0x78, 0x60, 0x60, 0x60, 0x00 },
    { 0x3c, 0x66, 0x60, 0x6e, 0x66, 0x66, 0x3c, 0x00 },
    { 0x66, 0x66, 0x66, 0x7e, 0x66, 0x66, 0x66, 0x00 },
    { 0x3c, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x00 },
    { 0x06, 0x06, 0x06, 0x06, 0x66, 0x66, 0x3c, 0x00 },
    { 0x66, 0x6c, 0x78, 0x70, 0x78, 0x6c, 0x66, 0x00 },
    { 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x7e, 0x00 },
    { 0xc6, 0xee, 0xfe, 0xfe, 0xd6, 0xc6, 0xc6, 0x00 },
    { 0x66, 0x76, 0x7e, 0x7e, 0x6e, 0x66, 0x66, 0x00 },
    { 0x3c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x00 },
    { 0x7c, 0x66, 0x66, 0x7c, 0x60, 0x60, 0x60, 0x00 },
    { 0x3c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x06 },
    { 0x7c, 0x66, 0x66, 0x7c, 0x66, 0x66, 0x66, 0x00 },
    { 0x3c, 0x66, 0x60, 0x3c, 0x06, 0x66, 0x3c, 0x00 },
    { 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00 },
    { 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x00 },
    { 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x18, 0x00 },
    { 0xc6, 0xc6, 0xd6, 0xfe, 0xfe, 0xee, 0xc6, 0x00 },
    { 0x66, 0x66, 0x3c, 0x18, 0x3c, 0x66, 0x66, 0x00 },
    { 0x66, 0x66, 0x66, 0x3c, 0x18, 0x18, 0x18, 0x00 },
    { 0x7e, 0x06, 0x0c, 0x18, 0x30, 0x60, 0x7e, 0x00 },
    { 0x3c, 0x30, 0x30, 0x30, 0x30, 0x30, 0x3c, 0x00 },
    { 0x80, 0xc0, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x00 },
    { 0x3c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x3c, 0x00 },
    { 0x10, 0x38, 0x6c, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f },
    { 0x30, 0x18, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x3c, 0x66, 0x66, 0x7e, 0x66, 0x66, 0x66, 0x00 },
    { 0x7c, 0x66, 0x66, 0x7c, 0x66, 0x66, 0x7c, 0x00 },
    { 0x3c, 0x66, 0x60, 0x60, 0x60, 0x66, 0x3c, 0x00 },
    { 0x7c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7c, 0x00 },
    { 0x7e, 0x60, 0x60, 0x78, 0x60, 0x60, 0x7e, 0x00 },
    { 0x7e, 0x60, 0x60, 0x78, 0x60, 0x60, 0x60, 0x00 },
    { 0x3c, 0x66, 0x60, 0x6e, 0x66, 0x66, 0x3c, 0x00 },
    { 0x66, 0x66, 0x66, 0x7e, 0x66, 0x66, 0x66, 0x00 },
    { 0x3c, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x00 },
    { 0x06, 0x06, 0x06, 0x06, 0x66, 0x66, 0x3c, 0x00 },
    { 0x66, 0x6c, 0x78, 0x70, 0x78, 0x6c, 0x66, 0x00 },
    { 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x7e, 0x00 },
    { 0xc6, 0xee, 0xfe, 0xfe, 0xd6, 0xc6, 0xc6, 0x00 },
    { 0x66, 0x76, 0x7e, 0x7e, 0x6e, 0x66, 0x66, 0x00 },
    { 0x3c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x00 },
    { 0x7c, 0x66, 0x66, 0x7c, 0x60, 0x60, 0x60, 0x00 },
    { 0x3c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x06 },
    { 0x7c, 0x66, 0x66, 0x7c, 0x66, 0x66, 0x66, 0x00 },
    { 0x3c, 0x66, 0x60, 0x3c, 0x06, 0x66, 0x3c, 0x00 },
    { 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00 },
    { 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x00 },
    { 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x18, 0x00 },
    { 0xc6, 0xc6, 0xd6, 0xfe, 0xfe, 0xee, 0xc6, 0x00 },
    { 0x66, 0x66, 0x3c, 0x18, 0x3c, 0x66, 0x66, 0x00 },
    { 0x66, 0x66, 0x66, 0x3c, 0x18, 0x18, 0x18, 0x00 },
    { 0x7e, 0x06, 0x0c, 0x18, 0x30, 0x60, 0x7e, 0x00 },
    { 0x0c, 0x18, 0x18, 0x30, 0x18, 0x18, 0x0c, 0x00 },
    { 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00 },
    { 0x30, 0x18, 0x18, 0x0c, 0x18, 0x18, 0x30, 0x00 },
    { 0x00, 0x00, 0x76, 0xdc, 0x00, 0x00, 0x00, 0x00 },
    { 0xff, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0xff },
};

static void glyph_print(running_machine *machine, bitmap_t *bitmap, INT32 x, INT32 y, const char *msg, ...)
{
    va_list arg_list;
    char buf[32768];
    INT32 index = 0;
    const device_config *screen = video_screen_first(machine->config);
    rectangle visarea = *video_screen_get_visible_area(screen);

    va_start( arg_list, msg );
    vsprintf( buf, msg, arg_list );
    va_end( arg_list );

    for(index = 0; index < strlen(buf) && index < 32768; index++)
    {
        UINT8 cur = (UINT8)buf[index];
        if(cur < 0x80)
        {
            INT32 line = 0;
            for(line = 0; line < 8; line++)
            {
                UINT32 *d = BITMAP_ADDR32(bitmap, y + line, 0);
                INT32 bit = 0;
                for(bit = 0; bit < 8; bit++)
                {
                    if(char_glyphs[cur][line] & (1 << (7 - bit)))
                    {
                        d[x+bit] = 0xffffffff;
                    }
                    else
                    {
                        d[x+bit] = 0;
                    }
                }
            }
        }

        x += 8;
        if(x >= visarea.max_x)
        {
            x = 0;
            y += 8;
            if(y >= visarea.max_y)
            {
                y = 0;
            }
        }
    }
}

static VIDEO_UPDATE( ssem )
{
    UINT32 line = 0;
    const device_config *ssem_cpu = devtag_get_device(screen->machine, "maincpu");
    UINT32 accum = cpu_get_reg(ssem_cpu, SSEM_A);
    UINT32 bit = 0;
    UINT32 word = 0;

    for(line = 0; line < 32; line++)
    {
        word = (store[(line << 2) | 0] << 24) |
               (store[(line << 2) | 1] << 16) |
               (store[(line << 2) | 2] <<  8) |
               (store[(line << 2) | 3] <<  0);
        for(bit = 0; bit < 32; bit++)
        {
            if(word & (1 << (31 - bit)))
            {
                glyph_print(screen->machine, bitmap, bit << 3, line << 3, "%c", line == store_line ? 4 : 2);
            }
            else
            {
                glyph_print(screen->machine, bitmap, bit << 3, line << 3, "%c", line == store_line ? 3 : 1);
            }
        }
    }

    for(bit = 0; bit < 32; bit++)
    {
        if(accum & (1 << bit))
        {
            glyph_print(screen->machine, bitmap, bit << 3, 264, "%c", 2);
        }
        else
        {
            glyph_print(screen->machine, bitmap, bit << 3, 264, "%c", 1);
        }
    }

    word = reverse((store[(store_line << 2) | 0] << 24) |
                   (store[(store_line << 2) | 1] << 16) |
                   (store[(store_line << 2) | 2] <<  8) |
                   (store[(store_line << 2) | 3] <<  0));
    glyph_print(screen->machine, bitmap, 0, 272, "LINE:%02d  VALUE:%08x  HALT:%d", store_line, word, cpu_get_reg(ssem_cpu, SSEM_HALT));
    return 0;
}

/****************************************************\
* Image helper functions                             *
\****************************************************/

static void strlower(char *buf)
{
    if(buf)
    {
        int i = 0;
        for(i = 0; i < strlen(buf); i++)
        {
            if(buf[i] >= 'A' && buf[i] <= 'Z')
            {
                buf[i] |= 0x20;
            }
        }
    }
}

/****************************************************\
* Image loading                                      *
\****************************************************/

static DEVICE_IMAGE_LOAD(ssem_store)
{
    const char* image_name = image_filename(image);
    char image_ext[5] = { 0 };
    char image_line[100] = { 0 };
    char token_buf[100] = { 0 };
    int num_lines = 0;
    int i = 0;

    // Isolate file extension and convert to lower-case
    memcpy(image_ext, image_name + (strlen(image_name) - 4), 5);
    strlower(image_ext);

    image_fgets(image, image_line, 99);
    sscanf(image_line, "%d", &num_lines);

    if(num_lines)
    {
        for(i = 0; i < num_lines; i++)
        {
            UINT32 line = 0;
            image_fgets(image, image_line, 99);

            // Isolate and convert 4-digit decimal address
            memcpy(token_buf, image_line, 4);
            token_buf[4] = '\0';
            sscanf(token_buf, "%04d", &line);

            if(strcmp(image_ext, ".snp") == 0)
            {
                UINT32 word = 0;
                int b = 0;

                // Parse a line such as: 0000:00000110101001000100000100000100
                for(b = 0; b < 32; b++)
                {
                    if(image_line[5 + b] == '1')
                    {
                        word |= 1 << (31 - b);
                    }
                }

                store[(line << 2) + 0] = (word >> 24) & 0x000000ff;
                store[(line << 2) + 1] = (word >> 16) & 0x000000ff;
                store[(line << 2) + 2] = (word >>  8) & 0x000000ff;
                store[(line << 2) + 3] = (word >>  0) & 0x000000ff;
            }
            else if(strcmp(image_ext, ".asm") == 0)
            {
                char op_buf[4] = { 0 };
                INT32 value = 0;
                UINT32 unsigned_value = 0;
                UINT32 word = 0;

                // Isolate the opcode and convert to lower-case
                memcpy(op_buf, image_line + 5, 3);
                op_buf[3] = '\0';
                strlower(op_buf);

                // Isolate the value
                sscanf(image_line + 9, "%d", &value);
                unsigned_value = reverse((UINT32)value);

                if(strcmp(op_buf, "num") == 0)
                {
                    word = unsigned_value;
                }
                else if(strcmp(op_buf, "jmp") == 0)
                {
                    word = 0x00000000 | unsigned_value ;
                }
                else if(strcmp(op_buf, "jrp") == 0)
                {
                    word = 0x00040000 | unsigned_value;
                }
                else if(strcmp(op_buf, "ldn") == 0)
                {
                    word = 0x00020000 | unsigned_value;
                }
                else if(strcmp(op_buf, "sto") == 0)
                {
                    word = 0x00060000 | unsigned_value;
                }
                else if(strcmp(op_buf, "sub") == 0)
                {
                    word = 0x00010000 | unsigned_value;
                }
                else if(strcmp(op_buf, "cmp") == 0)
                {
                    word = 0x00030000 | unsigned_value;
                }
                else if(strcmp(op_buf, "stp") == 0)
                {
                    word = 0x00070000 | unsigned_value;
                }

                store[(line << 2) + 0] = (word >> 24) & 0x000000ff;
                store[(line << 2) + 1] = (word >> 16) & 0x000000ff;
                store[(line << 2) + 2] = (word >>  8) & 0x000000ff;
                store[(line << 2) + 3] = (word >>  0) & 0x000000ff;
            }
        }
    }

    return INIT_PASS;
}

/****************************************************\
* Machine definition                                 *
\****************************************************/

static MACHINE_RESET( ssem )
{
    store_line = 0;
}

static MACHINE_DRIVER_START( ssem )
    /* basic machine hardware */
    MDRV_CPU_ADD("maincpu", SSEM, 700)
    MDRV_CPU_PROGRAM_MAP(ssem_map)

    MDRV_MACHINE_RESET(ssem)

    /* video hardware */
    MDRV_SCREEN_ADD("screen", RASTER)
    MDRV_SCREEN_REFRESH_RATE(50)
    MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
    MDRV_SCREEN_FORMAT(BITMAP_FORMAT_RGB32)
    MDRV_SCREEN_SIZE(256, 280)
    MDRV_SCREEN_VISIBLE_AREA(0, 255, 0, 279)
    MDRV_PALETTE_LENGTH(2)
    MDRV_PALETTE_INIT(black_and_white)

    MDRV_VIDEO_START(generic_bitmapped)
    MDRV_VIDEO_UPDATE(ssem)

    /* cartridge */
    MDRV_CARTSLOT_ADD("cart")
    MDRV_CARTSLOT_EXTENSION_LIST("snp,asm")
    MDRV_CARTSLOT_MANDATORY
    MDRV_CARTSLOT_LOAD(ssem_store)
MACHINE_DRIVER_END

ROM_START( ssem )
    ROM_REGION( 0x80, "maincpu",0 )  /* Main Store */
    ROM_FILL( 0x0000, 0x80, 0x00 )
ROM_END

/*   YEAR      NAME    PARENT    COMPAT   MACHINE     INPUT      INIT    COMPANY                   FULLNAME */
COMP(1948,     ssem,        0,        0,     ssem,     ssem,        0,   "Manchester University", "Small-Scale Experimental Machine (SSEM), 'Baby'", GAME_NO_SOUND )
