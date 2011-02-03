/***************************************************************************

        Ondra driver by Miodrag Milanovic

        08/09/2008 Preliminary driver.

****************************************************************************/


#include "emu.h"
#include "cpu/z80/z80.h"
#include "imagedev/cassette.h"
#include "includes/ondra.h"
#include "machine/ram.h"


static device_t *cassette_device_image(running_machine *machine)
{
	return machine->device("cassette");
}


static READ8_HANDLER( ondra_keyboard_r )
{
	UINT8 retVal = 0x00;
	UINT8 ondra_keyboard_line = offset & 0x000f;
	static const char *const keynames[] = { "LINE0", "LINE1", "LINE2", "LINE3", "LINE4", "LINE5", "LINE6", "LINE7", "LINE8", "LINE9" };
	double valcas = cassette_input(cassette_device_image(space->machine));
	if ( valcas < 0.00) {
		retVal = 0x80;
	}
	if (ondra_keyboard_line > 9) {
		retVal |= 0x1f;
	} else {
		retVal |= input_port_read(space->machine, keynames[ondra_keyboard_line]);
	}
	return retVal;
}

static void ondra_update_banks(running_machine *machine)
{
	ondra_state *state = machine->driver_data<ondra_state>();
	UINT8 *mem = machine->region("maincpu")->base();
	if (state->bank1_status==0) {
		memory_unmap_write(cputag_get_address_space(machine, "maincpu", ADDRESS_SPACE_PROGRAM), 0x0000, 0x3fff, 0, 0);
		memory_set_bankptr(machine, "bank1", mem + 0x010000);
	} else {
		memory_install_write_bank(cputag_get_address_space(machine, "maincpu", ADDRESS_SPACE_PROGRAM), 0x0000, 0x3fff, 0, 0, "bank1");
		memory_set_bankptr(machine, "bank1", ram_get_ptr(machine->device(RAM_TAG)) + 0x0000);
	}
	memory_set_bankptr(machine, "bank2", ram_get_ptr(machine->device(RAM_TAG)) + 0x4000);
	if (state->bank2_status==0) {
		memory_install_readwrite_bank(cputag_get_address_space(machine, "maincpu", ADDRESS_SPACE_PROGRAM), 0xe000, 0xffff, 0, 0, "bank3");
		memory_set_bankptr(machine, "bank3", ram_get_ptr(machine->device(RAM_TAG)) + 0xe000);
	} else {
		memory_unmap_write(cputag_get_address_space(machine, "maincpu", ADDRESS_SPACE_PROGRAM), 0xe000, 0xffff, 0, 0);
		memory_install_read8_handler (cputag_get_address_space(machine, "maincpu", ADDRESS_SPACE_PROGRAM), 0xe000, 0xffff, 0, 0, ondra_keyboard_r);
	}
}

WRITE8_HANDLER( ondra_port_03_w )
{
	ondra_state *state = space->machine->driver_data<ondra_state>();
	state->video_enable = data & 1;
	state->bank1_status = (data >> 1) & 1;
	state->bank2_status = (data >> 2) & 1;
	ondra_update_banks(space->machine);
	cassette_output(cassette_device_image(space->machine), ((data >> 3) & 1) ? -1.0 : +1.0);
}

WRITE8_HANDLER( ondra_port_09_w )
{
}

WRITE8_HANDLER( ondra_port_0a_w )
{
}

static TIMER_CALLBACK(nmi_check_callback)
{
	if ((input_port_read(machine, "NMI") & 1) == 1)
	{
		cputag_set_input_line(machine, "maincpu", INPUT_LINE_NMI, PULSE_LINE);
	}
}

MACHINE_RESET( ondra )
{
	ondra_state *state = machine->driver_data<ondra_state>();
	state->bank1_status = 0;
	state->bank2_status = 0;
	ondra_update_banks(machine);
}

MACHINE_START(ondra)
{
	timer_pulse(machine, attotime::from_hz(10), NULL, 0, nmi_check_callback);
}
