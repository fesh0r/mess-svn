#include "tandy2kb.h"



//**************************************************************************
//  MACROS / CONSTANTS
//**************************************************************************

#define I8048_TAG		"m1"

enum
{
	LED_1 = 0,
	LED_2
};


//**************************************************************************
//  DEVICE DEFINITIONS
//**************************************************************************

const device_type TANDY2K_KEYBOARD = &device_creator<tandy2k_keyboard_device>;



//-------------------------------------------------
//  ROM( tandy2k_keyboard )
//-------------------------------------------------

ROM_START( tandy2k_keyboard )
	ROM_REGION( 0x400, I8048_TAG, 0 )
	ROM_LOAD( "keyboard.m1", 0x0000, 0x0400, NO_DUMP )
ROM_END


//-------------------------------------------------
//  rom_region - device-specific ROM region
//-------------------------------------------------

const rom_entry *tandy2k_keyboard_device::device_rom_region() const
{
	return ROM_NAME( tandy2k_keyboard );
}


//-------------------------------------------------
//  ADDRESS_MAP( kb_io )
//-------------------------------------------------

static ADDRESS_MAP_START( tandy2k_keyboard_io, AS_IO, 8, tandy2k_keyboard_device )
	AM_RANGE(MCS48_PORT_P1, MCS48_PORT_P1) AM_WRITE(kb_p1_w)
	AM_RANGE(MCS48_PORT_P2, MCS48_PORT_P2) AM_WRITE(kb_p2_w)
	AM_RANGE(MCS48_PORT_BUS, MCS48_PORT_BUS) AM_READ(kb_p1_r)
ADDRESS_MAP_END


//-------------------------------------------------
//  MACHINE_DRIVER( tandy2k_keyboard )
//-------------------------------------------------

static MACHINE_CONFIG_FRAGMENT( tandy2k_keyboard )
	MCFG_CPU_ADD(I8048_TAG, I8048, 1000000) // ?
	MCFG_CPU_IO_MAP(tandy2k_keyboard_io)
	MCFG_DEVICE_DISABLE() // TODO
MACHINE_CONFIG_END


//-------------------------------------------------
//  machine_config_additions - device-specific
//  machine configurations
//-------------------------------------------------

machine_config_constructor tandy2k_keyboard_device::device_mconfig_additions() const
{
	return MACHINE_CONFIG_NAME( tandy2k_keyboard );
}


//-------------------------------------------------
//  INPUT_PORTS( tandy2k_keyboard )
//-------------------------------------------------

INPUT_PORTS_START( tandy2k_keyboard )
	PORT_START("Y0")
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD )

	PORT_START("Y1")
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD )

	PORT_START("Y2")
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD )

	PORT_START("Y3")
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD )

	PORT_START("Y4")
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD )

	PORT_START("Y5")
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD )

	PORT_START("Y6")
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD )

	PORT_START("Y7")
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD )

	PORT_START("Y8")
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD )

	PORT_START("Y9")
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD )

	PORT_START("Y10")
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD )

	PORT_START("Y11")
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD )
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD )
INPUT_PORTS_END


//-------------------------------------------------
//  input_ports - device-specific input ports
//-------------------------------------------------

ioport_constructor tandy2k_keyboard_device::device_input_ports() const
{
	return INPUT_PORTS_NAME( tandy2k_keyboard );
}



//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

//-------------------------------------------------
//  tandy2k_keyboard_device - constructor
//-------------------------------------------------

tandy2k_keyboard_device::tandy2k_keyboard_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
    : device_t(mconfig, TANDY2K_KEYBOARD, "Tandy 2000 Keyboard", tag, owner, clock),
	  m_maincpu(*this, I8048_TAG),
	  m_keylatch(0xffff),
	  m_clock(0),
	  m_data(0)
{
}


//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void tandy2k_keyboard_device::device_start()
{
	// resolve callbacks
    m_out_clock_func.resolve(m_out_clock_cb, *this);
    m_out_data_func.resolve(m_out_data_cb, *this);

	// state saving
	save_item(NAME(m_keylatch));
}


//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void tandy2k_keyboard_device::device_reset()
{
}


//-------------------------------------------------
//  power_w -
//-------------------------------------------------

WRITE_LINE_MEMBER( tandy2k_keyboard_device::power_w )
{
	// TODO
}


//-------------------------------------------------
//  reset_w -
//-------------------------------------------------

WRITE_LINE_MEMBER( tandy2k_keyboard_device::reset_w )
{
	if (!state)
	{
		device_reset();
	}
}


//-------------------------------------------------
//  busy_w -
//-------------------------------------------------

WRITE_LINE_MEMBER( tandy2k_keyboard_device::busy_w )
{
	m_maincpu->set_input_line(MCS48_INPUT_IRQ, state ? CLEAR_LINE : ASSERT_LINE);
}


//-------------------------------------------------
//  data_r -
//-------------------------------------------------

READ_LINE_MEMBER( tandy2k_keyboard_device::data_r )
{
	return m_data;
}


//-------------------------------------------------
//  kb_p1_r -
//-------------------------------------------------

READ8_MEMBER( tandy2k_keyboard_device::kb_p1_r )
{
  /*

        bit     description

        0       X0
        1       X1
        2       X2
        3       X3
        4       X4
        5       X5
        6       X6
        7       X7

    */

	UINT8 data = 0xff;

	if (!BIT(m_keylatch, 0)) data &= input_port_read(machine(), "Y0");
	if (!BIT(m_keylatch, 1)) data &= input_port_read(machine(), "Y1");
	if (!BIT(m_keylatch, 2)) data &= input_port_read(machine(), "Y2");
	if (!BIT(m_keylatch, 3)) data &= input_port_read(machine(), "Y3");
	if (!BIT(m_keylatch, 4)) data &= input_port_read(machine(), "Y4");
	if (!BIT(m_keylatch, 5)) data &= input_port_read(machine(), "Y5");
	if (!BIT(m_keylatch, 6)) data &= input_port_read(machine(), "Y6");
	if (!BIT(m_keylatch, 7)) data &= input_port_read(machine(), "Y7");
	if (!BIT(m_keylatch, 8)) data &= input_port_read(machine(), "Y8");
	if (!BIT(m_keylatch, 9)) data &= input_port_read(machine(), "Y9");
	if (!BIT(m_keylatch, 10)) data &= input_port_read(machine(), "Y10");
	if (!BIT(m_keylatch, 11)) data &= input_port_read(machine(), "Y11");

	return ~data;
}


//-------------------------------------------------
//  kb_p1_w -
//-------------------------------------------------

WRITE8_MEMBER( tandy2k_keyboard_device::kb_p1_w )
{
	/*

        bit     description

        0       Y0
        1       Y1
        2       Y2
        3       Y3
        4       Y4
        5       Y5
        6       Y6
        7       Y7

    */

	// keyboard row
	m_keylatch = (m_keylatch & 0xff00) | data;
}


//-------------------------------------------------
//  kb_p2_w -
//-------------------------------------------------

WRITE8_MEMBER( tandy2k_keyboard_device::kb_p2_w )
{
	/*

        bit     description

        0       Y8
        1       Y9
        2       Y10
        3       Y11
        4       LED 2
        5       LED 1
        6       CLOCK
        7       DATA

    */

	// keyboard row
	m_keylatch = ((data & 0x0f) << 8) | (m_keylatch & 0xff);

	// led output
	output_set_led_value(LED_2, !BIT(data, 4));
	output_set_led_value(LED_1, !BIT(data, 5));

	// keyboard clock
	int clock = BIT(data, 6);

	if (m_clock != clock)
	{
		m_clock = clock;

		m_out_clock_func(m_clock);
	}

	// keyboard data
	int kbddat = BIT(data, 7);

	if (m_data != kbddat)
	{
		m_data = kbddat;

		m_out_data_func(m_data);
	}
}
