/***************************************************************************

  machine.c

  Functions to emulate general aspects of PK-01 Lviv (RAM, ROM, interrupts,
  I/O ports)

  Krzysztof Strzecha

***************************************************************************/

#include "emu.h"
#include "imagedev/cassette.h"
#include "imagedev/snapquik.h"
#include "cpu/i8085/i8085.h"
#include "machine/i8255a.h"
#include "includes/lviv.h"
#include "sound/speaker.h"
#include "machine/ram.h"

#define LVIV_SNAPSHOT_SIZE	82219





static void lviv_update_memory (running_machine *machine)
{
	lviv_state *state = machine->driver_data<lviv_state>();
	if (state->ppi_port_outputs[0][2] & 0x02)
	{
		memory_set_bankptr(machine,"bank1", ram_get_ptr(machine->device(RAM_TAG)));
		memory_set_bankptr(machine,"bank2", ram_get_ptr(machine->device(RAM_TAG)) + 0x4000);
	}
	else
	{
		memory_set_bankptr(machine,"bank1", ram_get_ptr(machine->device(RAM_TAG)) + 0x8000);
		memory_set_bankptr(machine,"bank2", ram_get_ptr(machine->device(RAM_TAG)) + 0xc000);
	}
}

static TIMER_CALLBACK( lviv_reset )
{
	machine->schedule_soft_reset();
}

DIRECT_UPDATE_HANDLER(lviv_directoverride)
{
	if (input_port_read(machine, "RESET") & 0x01)
		timer_set(machine, ATTOTIME_IN_USEC(10), NULL, 0, lviv_reset);
	return address;
}

static READ8_DEVICE_HANDLER ( lviv_ppi_0_porta_r )
{
	return 0xff;
}

static READ8_DEVICE_HANDLER ( lviv_ppi_0_portb_r )
{
	return 0xff;
}

static READ8_DEVICE_HANDLER ( lviv_ppi_0_portc_r )
{
	lviv_state *state = device->machine->driver_data<lviv_state>();
	UINT8 data = state->ppi_port_outputs[0][2] & 0x0f;
	if (cassette_input(device->machine->device("cassette")) > 0.038)
		data |= 0x10;
	if (state->ppi_port_outputs[0][0] & input_port_read(device->machine, "JOY"))
		data |= 0x80;
	return data;
}

static WRITE8_DEVICE_HANDLER ( lviv_ppi_0_porta_w )
{
	lviv_state *state = device->machine->driver_data<lviv_state>();
	state->ppi_port_outputs[0][0] = data;
}

static WRITE8_DEVICE_HANDLER ( lviv_ppi_0_portb_w )
{
	lviv_state *state = device->machine->driver_data<lviv_state>();
	state->ppi_port_outputs[0][1] = data;
	lviv_update_palette(device->machine, data&0x7f);
}

static WRITE8_DEVICE_HANDLER ( lviv_ppi_0_portc_w )	/* tape in/out, video memory on/off */
{
	lviv_state *state = device->machine->driver_data<lviv_state>();
	device_t *speaker = device->machine->device("speaker");
	state->ppi_port_outputs[0][2] = data;
	if (state->ppi_port_outputs[0][1]&0x80)
		speaker_level_w(speaker, data&0x01);
	cassette_output(device->machine->device("cassette"), (data & 0x01) ? -1.0 : 1.0);
	lviv_update_memory(device->machine);
}

static READ8_DEVICE_HANDLER ( lviv_ppi_1_porta_r )
{
	return 0xff;
}

static READ8_DEVICE_HANDLER ( lviv_ppi_1_portb_r )	/* keyboard reading */
{
	lviv_state *state = device->machine->driver_data<lviv_state>();
	return	((state->ppi_port_outputs[1][0] & 0x01) ? 0xff : input_port_read(device->machine, "KEY0")) &
		((state->ppi_port_outputs[1][0] & 0x02) ? 0xff : input_port_read(device->machine, "KEY1")) &
		((state->ppi_port_outputs[1][0] & 0x04) ? 0xff : input_port_read(device->machine, "KEY2")) &
		((state->ppi_port_outputs[1][0] & 0x08) ? 0xff : input_port_read(device->machine, "KEY3")) &
		((state->ppi_port_outputs[1][0] & 0x10) ? 0xff : input_port_read(device->machine, "KEY4")) &
		((state->ppi_port_outputs[1][0] & 0x20) ? 0xff : input_port_read(device->machine, "KEY5")) &
		((state->ppi_port_outputs[1][0] & 0x40) ? 0xff : input_port_read(device->machine, "KEY6")) &
		((state->ppi_port_outputs[1][0] & 0x80) ? 0xff : input_port_read(device->machine, "KEY7"));
}

static READ8_DEVICE_HANDLER ( lviv_ppi_1_portc_r )     /* keyboard reading */
{
	lviv_state *state = device->machine->driver_data<lviv_state>();
	return	((state->ppi_port_outputs[1][2] & 0x01) ? 0xff : input_port_read(device->machine, "KEY8")) &
		((state->ppi_port_outputs[1][2] & 0x02) ? 0xff : input_port_read(device->machine, "KEY9" )) &
		((state->ppi_port_outputs[1][2] & 0x04) ? 0xff : input_port_read(device->machine, "KEY10")) &
		((state->ppi_port_outputs[1][2] & 0x08) ? 0xff : input_port_read(device->machine, "KEY11"));
}

static WRITE8_DEVICE_HANDLER ( lviv_ppi_1_porta_w )	/* kayboard scaning */
{
	lviv_state *state = device->machine->driver_data<lviv_state>();
	state->ppi_port_outputs[1][0] = data;
}

static WRITE8_DEVICE_HANDLER ( lviv_ppi_1_portb_w )
{
	lviv_state *state = device->machine->driver_data<lviv_state>();
	state->ppi_port_outputs[1][1] = data;
}

static WRITE8_DEVICE_HANDLER ( lviv_ppi_1_portc_w )	/* kayboard scaning */
{
	lviv_state *state = device->machine->driver_data<lviv_state>();
	state->ppi_port_outputs[1][2] = data;
}


/* I/O */
 READ8_HANDLER ( lviv_io_r )
{
	lviv_state *state = space->machine->driver_data<lviv_state>();
	if (state->startup_mem_map)
	{
		return 0;	/* ??? */
	}
	else
	{
		switch ((offset >> 4) & 0x3)
		{
		case 0:
			return i8255a_r(space->machine->device("ppi8255_0"), offset & 3);

		case 1:
			return i8255a_r(space->machine->device("ppi8255_1"), offset & 3);

		case 2:
		case 3:
		default:
			/* reserved for extension? */
			return 0;	/* ??? */
		}
	}
}

WRITE8_HANDLER ( lviv_io_w )
{
	lviv_state *state = space->machine->driver_data<lviv_state>();
	address_space *cpuspace = cputag_get_address_space(space->machine, "maincpu", ADDRESS_SPACE_PROGRAM);
	if (state->startup_mem_map)
	{
		state->startup_mem_map = 0;

		memory_install_write_bank(cpuspace, 0x0000, 0x3fff, 0, 0, "bank1");
		memory_install_write_bank(cpuspace, 0x4000, 0x7fff, 0, 0, "bank2");
		memory_install_write_bank(cpuspace, 0x8000, 0xbfff, 0, 0, "bank3");
		memory_unmap_write(cpuspace, 0xC000, 0xffff, 0, 0);

		memory_set_bankptr(space->machine,"bank1", ram_get_ptr(space->machine->device(RAM_TAG)));
		memory_set_bankptr(space->machine,"bank2", ram_get_ptr(space->machine->device(RAM_TAG)) + 0x4000);
		memory_set_bankptr(space->machine,"bank3", ram_get_ptr(space->machine->device(RAM_TAG)) + 0x8000);
		memory_set_bankptr(space->machine,"bank4", space->machine->region("maincpu")->base() + 0x010000);
	}
	else
	{
		switch ((offset >> 4) & 0x3)
		{
		case 0:
			i8255a_w(space->machine->device("ppi8255_0"), offset & 3, data);
			break;

		case 1:
			i8255a_w(space->machine->device("ppi8255_1"), offset & 3, data);
			break;

		case 2:
		case 3:
			/* reserved for extension? */
			break;
		}
	}
}


I8255A_INTERFACE( lviv_ppi8255_interface_0 )
{
	DEVCB_HANDLER(lviv_ppi_0_porta_r),
	DEVCB_HANDLER(lviv_ppi_0_portb_r),
	DEVCB_HANDLER(lviv_ppi_0_portc_r),
	DEVCB_HANDLER(lviv_ppi_0_porta_w),
	DEVCB_HANDLER(lviv_ppi_0_portb_w),
	DEVCB_HANDLER(lviv_ppi_0_portc_w)
};

I8255A_INTERFACE( lviv_ppi8255_interface_1 )
{
	DEVCB_HANDLER(lviv_ppi_1_porta_r),
	DEVCB_HANDLER(lviv_ppi_1_portb_r),
	DEVCB_HANDLER(lviv_ppi_1_portc_r),
	DEVCB_HANDLER(lviv_ppi_1_porta_w),
	DEVCB_HANDLER(lviv_ppi_1_portb_w),
	DEVCB_HANDLER(lviv_ppi_1_portc_w)
};

MACHINE_RESET( lviv )
{
	lviv_state *state = machine->driver_data<lviv_state>();
	address_space *space = cputag_get_address_space(machine, "maincpu", ADDRESS_SPACE_PROGRAM);
	space->set_direct_update_handler(direct_update_delegate_create_static(lviv_directoverride, *machine));

	state->video_ram = ram_get_ptr(machine->device(RAM_TAG)) + 0xc000;

	state->startup_mem_map = 1;

	memory_unmap_write(space, 0x0000, 0x3fff, 0, 0);
	memory_unmap_write(space, 0x4000, 0x7fff, 0, 0);
	memory_unmap_write(space, 0x8000, 0xbfff, 0, 0);
	memory_unmap_write(space, 0xC000, 0xffff, 0, 0);

	memory_set_bankptr(machine,"bank1", machine->region("maincpu")->base() + 0x010000);
	memory_set_bankptr(machine,"bank2", machine->region("maincpu")->base() + 0x010000);
	memory_set_bankptr(machine,"bank3", machine->region("maincpu")->base() + 0x010000);
	memory_set_bankptr(machine,"bank4", machine->region("maincpu")->base() + 0x010000);

	/*timer_pulse(machine, TIME_IN_NSEC(200), NULL, 0, lviv_draw_pixel);*/

	/*memset(ram_get_ptr(machine->device(RAM_TAG)), 0, sizeof(unsigned char)*0xffff);*/
}


/*******************************************************************************
Lviv snapshot files (SAV)
-------------------------

00000 - 0000D:  'LVOV/DUMP/2.0/' (like LVT-header)
0000E - 0000F:  'H+' (something additional)
00010           00h
00011 - 0C010:  RAM (0000 - BFFF)
0C011 - 10010:  ROM (C000 - FFFF)
10011 - 14010:  Video RAM (4000 - 7FFF)
14011 - 14110:  Ports map (00 - FF)
14111 - 1411C:  Registers (B,C,D,E,H,L,A,F,SP,PC)
1411D - 1412A:  ??? (something additional)
*******************************************************************************/

static void lviv_setup_snapshot (running_machine *machine,UINT8 * data)
{
	lviv_state *state = machine->driver_data<lviv_state>();
	unsigned char lo,hi;

	/* Set registers */
	lo = data[0x14112] & 0x0ff;
	hi = data[0x14111] & 0x0ff;
	cpu_set_reg(machine->device("maincpu"), I8085_BC, (hi << 8) | lo);
	lo = data[0x14114] & 0x0ff;
	hi = data[0x14113] & 0x0ff;
	cpu_set_reg(machine->device("maincpu"), I8085_DE, (hi << 8) | lo);
	lo = data[0x14116] & 0x0ff;
	hi = data[0x14115] & 0x0ff;
	cpu_set_reg(machine->device("maincpu"), I8085_HL, (hi << 8) | lo);
	lo = data[0x14118] & 0x0ff;
	hi = data[0x14117] & 0x0ff;
	cpu_set_reg(machine->device("maincpu"), I8085_AF, (hi << 8) | lo);
	lo = data[0x14119] & 0x0ff;
	hi = data[0x1411a] & 0x0ff;
	cpu_set_reg(machine->device("maincpu"), I8085_SP, (hi << 8) | lo);
	lo = data[0x1411b] & 0x0ff;
	hi = data[0x1411c] & 0x0ff;
	cpu_set_reg(machine->device("maincpu"), I8085_PC, (hi << 8) | lo);

	/* Memory dump */
	memcpy (ram_get_ptr(machine->device(RAM_TAG)), data+0x0011, 0xc000);
	memcpy (ram_get_ptr(machine->device(RAM_TAG))+0xc000, data+0x10011, 0x4000);

	/* Ports */
	state->ppi_port_outputs[0][0] = data[0x14011+0xc0];
	state->ppi_port_outputs[0][1] = data[0x14011+0xc1];
	lviv_update_palette(machine, state->ppi_port_outputs[0][1]&0x7f);
	state->ppi_port_outputs[0][2] = data[0x14011+0xc2];
	lviv_update_memory(machine);
}

static void dump_registers(running_machine *machine)
{
	logerror("PC   = %04x\n", (unsigned) cpu_get_reg(machine->device("maincpu"), I8085_PC));
	logerror("SP   = %04x\n", (unsigned) cpu_get_reg(machine->device("maincpu"), I8085_SP));
	logerror("AF   = %04x\n", (unsigned) cpu_get_reg(machine->device("maincpu"), I8085_AF));
	logerror("BC   = %04x\n", (unsigned) cpu_get_reg(machine->device("maincpu"), I8085_BC));
	logerror("DE   = %04x\n", (unsigned) cpu_get_reg(machine->device("maincpu"), I8085_DE));
	logerror("HL   = %04x\n", (unsigned) cpu_get_reg(machine->device("maincpu"), I8085_HL));
}

static int lviv_verify_snapshot (UINT8 * data, UINT32 size)
{
	const char* tag = "LVOV/DUMP/2.0/";

	if( strncmp( tag, (char*)data, strlen(tag) ) )
	{
		logerror("Not a Lviv snapshot\n");
		return IMAGE_VERIFY_FAIL;
	}

	if (size != LVIV_SNAPSHOT_SIZE)
	{
		logerror ("Incomplete snapshot file\n");
		return IMAGE_VERIFY_FAIL;
	}

	logerror("returning ID_OK\n");
	return IMAGE_VERIFY_PASS;
}

SNAPSHOT_LOAD( lviv )
{
	UINT8 *lviv_snapshot_data;

	lviv_snapshot_data = (UINT8*)malloc(LVIV_SNAPSHOT_SIZE);
	if (!lviv_snapshot_data)
	{
		logerror ("Unable to load snapshot file\n");
		return IMAGE_INIT_FAIL;
	}

	image.fread( lviv_snapshot_data, LVIV_SNAPSHOT_SIZE);

	if( lviv_verify_snapshot(lviv_snapshot_data, snapshot_size) == IMAGE_VERIFY_FAIL)
	{
		free(lviv_snapshot_data);
		return IMAGE_INIT_FAIL;
	}

	lviv_setup_snapshot (image.device().machine,lviv_snapshot_data);

	dump_registers(image.device().machine);

	free(lviv_snapshot_data);

	logerror("Snapshot file loaded\n");
	return IMAGE_INIT_PASS;
}

