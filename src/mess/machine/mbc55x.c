/*
    machine/mbc55x.c

    Machine driver for the Sanyo MBC-550 and MBC-555.

    Phill Harvey-Smith
    2011-01-29.
*/

#include "emu.h"
#include "memory.h"
#include "debugger.h"
#include "cpu/i86/i86.h"
#include "debug/debugcpu.h"
#include "debug/debugcon.h"
#include "imagedev/flopdrv.h"
#include "machine/ram.h"
#include "machine/pic8259.h"
#include "machine/pit8253.h"
#include "machine/ctronics.h"

#include "includes/mbc55x.h"


/*-------------------------------------------------------------------------*/
/* Defines, constants, and global variables                                */
/*-------------------------------------------------------------------------*/

/* Debugging */

#define DEBUG_SET(flags)    ((state->debug_machine & (flags))==(flags))

#define DEBUG_NONE			0x0000000
#define DMA_BREAK           0x0000001
#define DECODE_BIOS         0x0000002
#define DECODE_BIOS_RAW     0x0000004
#define DECODE_DOS21		0x0000008

static void decode_dos21(device_t *device,offs_t pc);


/* Floppy drives WD2793 */

static WRITE_LINE_DEVICE_HANDLER( mbc55x_fdc_intrq_w );
static WRITE_LINE_DEVICE_HANDLER( mbc55x_fdc_drq_w );

const wd17xx_interface mbc55x_wd17xx_interface =
{
	DEVCB_LINE_GND,
	DEVCB_LINE(mbc55x_fdc_intrq_w),
	DEVCB_LINE(mbc55x_fdc_drq_w),
	{FLOPPY_0, FLOPPY_1, FLOPPY_2, FLOPPY_3}
};

//static void mbc55x_recalculate_ints(running_machine *machine);

static void mbc55x_debug(running_machine *machine, int ref, int params, const char *param[]);

static int instruction_hook(device_t &device, offs_t curpc);

//static void fdc_reset(running_machine *machine);
//static void set_disk_int(running_machine *machine, int state);

//static void keyboard_reset(running_machine *machine);
//static TIMER_CALLBACK(keyscan_callback);

/* 8255 Configuration */

READ8_HANDLER(ppi8255_r)
{
	device_t *ppi8255 = space->machine->device(PPI8255_TAG);

	switch(offset)
	{
		case 0 	: return i8255a_r(ppi8255,0); break;
		case 2 	: return i8255a_r(ppi8255,1); break;
		case 4 	: return i8255a_r(ppi8255,2); break;
		case 6 	: return i8255a_r(ppi8255,3); break;
		default : return 0;
	}
}

WRITE8_HANDLER(ppi8255_w)
{
	device_t *ppi8255 = space->machine->device(PPI8255_TAG);

	switch(offset)
	{
		case 0 	: i8255a_w(ppi8255,0,data); break;
		case 2 	: i8255a_w(ppi8255,1,data); break;
		case 4 	: i8255a_w(ppi8255,2,data); break;
		case 6 	: i8255a_w(ppi8255,3,data); break;
	}
}

READ8_DEVICE_HANDLER(mbc55x_ppi_porta_r)
{
	return 0;
}

READ8_DEVICE_HANDLER(mbc55x_ppi_portb_r)
{
	return 0;
}

READ8_DEVICE_HANDLER(mbc55x_ppi_portc_r)
{
	return 0;
}

WRITE8_DEVICE_HANDLER(mbc55x_ppi_porta_w)
{
}

WRITE8_DEVICE_HANDLER(mbc55x_ppi_portb_w)
{
}

WRITE8_DEVICE_HANDLER(mbc55x_ppi_portc_w)
{
	device_t *wd17xx = device->machine->device(FDC_TAG);

	wd17xx_set_drive(wd17xx,(data & 0x03));
	wd17xx_set_side(wd17xx,((data & 0x04) >> 2));
}

I8255A_INTERFACE( mbc55x_ppi8255_interface )
{
	DEVCB_HANDLER(mbc55x_ppi_porta_r),
	DEVCB_HANDLER(mbc55x_ppi_portb_r),
	DEVCB_HANDLER(mbc55x_ppi_portc_r),
	DEVCB_HANDLER(mbc55x_ppi_porta_w),
	DEVCB_HANDLER(mbc55x_ppi_portb_w),
	DEVCB_HANDLER(mbc55x_ppi_portc_w)
};

/* PIC 8259 Configuration */

const struct pic8259_interface mbc55x_pic8259_config =
{
	DEVCB_CPU_INPUT_LINE(MAINCPU_TAG, INPUT_LINE_IRQ0)
};

READ8_HANDLER(pic8259_r)
{
	device_t *pic8259 = space->machine->device(PIC8259_TAG);

	switch (offset)
	{
		case 0	: return pic8259_r(pic8259,0); break;
		case 2	: return pic8259_r(pic8259,1); break;
		default :
			return 0;
	}
}

WRITE8_HANDLER(pic8259_w)
{
	device_t *pic8259 = space->machine->device(PIC8259_TAG);

	switch (offset)
	{
		case 0	: pic8259_w(pic8259,0,data); break;
		case 2	: pic8259_w(pic8259,1,data); break;
	}
}

static IRQ_CALLBACK(mbc55x_irq_callback)
{
	device_t *pic8259 = device->machine->device(PIC8259_TAG);

	return pic8259_acknowledge( pic8259 );
}

/* PIT8253 Configuration */

const struct pit8253_config mbc55x_pit8253_config =
{
	{
		{
			PIT_C0_CLOCK,
			DEVCB_NULL,
			DEVCB_DEVICE_LINE(PIC8259_TAG, pic8259_ir0_w)
		}, 
		{
			PIT_C1_CLOCK,
			DEVCB_NULL,
			DEVCB_DEVICE_LINE(PIC8259_TAG, pic8259_ir1_w)
		}, 
		{
			PIT_C2_CLOCK,
			DEVCB_NULL,
			DEVCB_NULL
		}
	}
};

READ8_HANDLER(pit8253_r)
{
	device_t *pit8253 = space->machine->device(PIT8253_TAG);

	switch(offset)
	{
		case 0 	: return pit8253_r(pit8253,0); break;
		case 2 	: return pit8253_r(pit8253,1); break;
		case 4 	: return pit8253_r(pit8253,2); break;
		case 6 	: return pit8253_r(pit8253,3); break;
		default : return 0;
	}
}

WRITE8_HANDLER(pit8253_w)
{
	device_t *pit8253 = space->machine->device(PIT8253_TAG);

	switch(offset)
	{
		case 0 	: pit8253_w(pit8253,0,data); break;
		case 2 	: pit8253_w(pit8253,1,data); break;
		case 4 	: pit8253_w(pit8253,2,data); break;
		case 6 	: pit8253_w(pit8253,3,data); break;
	}
}

/* Video ram page register */

READ8_HANDLER(vram_page_r)
{
	mbc55x_state *state	= space->machine->driver_data<mbc55x_state>();

	return state->vram_page;
}

WRITE8_HANDLER(vram_page_w)
{
	mbc55x_state *state	= space->machine->driver_data<mbc55x_state>();

	logerror("%s : set vram page to %02X\n",cpuexec_describe_context(space->machine),data);

	state->vram_page=data;
}

READ8_HANDLER(mbc55x_disk_r)
{
	device_t *wd17xx = space->machine->device(FDC_TAG);

	switch(offset)
	{
		case 0 	: return wd17xx_status_r(wd17xx,0);	break;
		case 2 	: return wd17xx_track_r(wd17xx,0);	break;
		case 4 	: return wd17xx_sector_r(wd17xx,0);	break;
		case 6 	: return wd17xx_data_r(wd17xx,0);	break;
		default : return 0;
	}
}

WRITE8_HANDLER(mbc55x_disk_w)
{
	device_t *wd17xx = space->machine->device(FDC_TAG);

	switch(offset)
	{
		case 0 	: wd17xx_command_w(wd17xx,0,data);	break;
		case 2 	: wd17xx_track_w(wd17xx,0,data);	break;
		case 4 	: wd17xx_sector_w(wd17xx,0,data);	break;
		case 6 	: wd17xx_data_w(wd17xx,0,data);	break;
	}
}

static WRITE_LINE_DEVICE_HANDLER( mbc55x_fdc_intrq_w )
{
}

static WRITE_LINE_DEVICE_HANDLER( mbc55x_fdc_drq_w )
{
}

/* msm8251 serial */

const msm8251_interface mbc55x_msm8251a_interface = 
{
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL
};

READ8_HANDLER(mbc55x_kb_usart_r)
{
	device_t *msm8251 = space->machine->device(MSM8251A_TAG);

	switch (offset)
	{
		case 0	: return msm8251_data_r(msm8251,0); break;
		case 2	: return msm8251_status_r(msm8251,0); break;
		default :
			return 0;
	}
}

WRITE8_HANDLER(mbc55x_kb_usart_w)
{
	device_t *msm8251 = space->machine->device(MSM8251A_TAG);

	switch (offset)
	{
		case 0	: msm8251_data_w(msm8251,0,data); break;
		case 2	: msm8251_control_w(msm8251,0,data); break;
	}
}

DRIVER_INIT(mbc55x)
{
}

static void set_ram_size(running_machine *machine)
{
	mbc55x_state 	*state 		= machine->driver_data<mbc55x_state>();
	address_space 	*space 		= cputag_get_address_space( machine, MAINCPU_TAG, ADDRESS_SPACE_PROGRAM );
	int 			ramsize 	= ram_get_size(machine->device(RAM_TAG));
	int 			nobanks		= ramsize / RAM_BANK_SIZE;
	char 			bank[10];
	int 			bankno;
	UINT8 			*ram    	= &ram_get_ptr(machine->device(RAM_TAG))[0];
	UINT8			*map_base;
	int				bank_base;


	logerror("Ramsize is %d bytes\n",ramsize);
	logerror("RAM_BANK_SIZE=%d, nobanks=%d\n",RAM_BANK_SIZE,nobanks);

	// Main memory mapping

	for(bankno=0; bankno<RAM_BANK_COUNT; bankno++)
	{
		sprintf(bank,"bank%x",bankno);
		bank_base=bankno*RAM_BANK_SIZE;
		map_base=&ram[bank_base];

		if(bankno<nobanks)
		{
			memory_set_bankptr(machine, bank, map_base);
			memory_install_readwrite_bank(space, bank_base, bank_base+(RAM_BANK_SIZE-1), 0, 0, bank);
			logerror("Mapping bank %d at %05X to RAM\n",bankno,bank_base);
		}
		else
		{
			memory_nop_readwrite(space,bank_base, bank_base+(RAM_BANK_SIZE-1), 0, 0);
			logerror("Mapping bank %d at %05X to NOP\n",bankno,bank_base);
		}
	}

	// Graphics red and blue plane memory mapping, green is in main memory
	memory_set_bankptr(machine, RED_PLANE_TAG, &state->video_mem[RED_PLANE_OFFSET]);
	memory_install_readwrite_bank(space, RED_PLANE_MEMBASE, RED_PLANE_MEMBASE+(COLOUR_PLANE_SIZE-1), 0, 0, RED_PLANE_TAG);
	memory_set_bankptr(machine, BLUE_PLANE_TAG, &state->video_mem[BLUE_PLANE_OFFSET]);
	memory_install_readwrite_bank(space, BLUE_PLANE_MEMBASE, BLUE_PLANE_MEMBASE+(COLOUR_PLANE_SIZE-1), 0, 0, BLUE_PLANE_TAG);
}

MACHINE_RESET( mbc55x )
{
	set_ram_size(machine);
	cpu_set_irq_callback(machine->device(MAINCPU_TAG), mbc55x_irq_callback);
}

MACHINE_START( mbc55x )
{
	mbc55x_state *state = machine->driver_data<mbc55x_state>();
	/* init cpu */
//	mbc55x_cpu_init(machine);

	/* setup debug commands */
	if (machine->debug_flags & DEBUG_FLAG_ENABLED)
	{
		debug_console_register_command(machine, "mbc55x_debug", CMDFLAG_NONE, 0, 0, 1, mbc55x_debug);

		/* set up the instruction hook */
		machine->device(MAINCPU_TAG)->debug()->set_instruction_hook(instruction_hook);
	}

	state->debug_machine=DEBUG_NONE;
}


static void mbc55x_debug(running_machine *machine, int ref, int params, const char *param[])
{
	mbc55x_state *state = machine->driver_data<mbc55x_state>();
	if(params>0)
	{
		sscanf(param[0],"%d",&state->debug_machine);
	}
	else
	{
		debug_console_printf(machine,"Error usage : mbc55x_debug <debuglevel>\n");
		debug_console_printf(machine,"Current debuglevel=%02X\n",state->debug_machine);
	}
}

/*-----------------------------------------------
    instruction_hook - per-instruction hook
-----------------------------------------------*/

static int instruction_hook(device_t &device, offs_t curpc)
{
	mbc55x_state 	*state = device.machine->driver_data<mbc55x_state>();
	address_space 	*space = cpu_get_address_space(&device, ADDRESS_SPACE_PROGRAM);
	UINT8           *addr_ptr;

	addr_ptr = (UINT8*)space->get_read_ptr(curpc);

	if ((addr_ptr !=NULL) && (addr_ptr[0]==0xCD))
	{
//		logerror("int %02X called\n",addr_ptr[1]);

		if(DEBUG_SET(DECODE_DOS21) && (addr_ptr[1]==0x21))
			decode_dos21(&device,curpc);
	}

	return 0;
}

static void decode_dos21(device_t *device,offs_t pc)
{
	device_t *cpu = device->machine->device(MAINCPU_TAG);

	UINT16  ax = cpu_get_reg(cpu,I8086_AX);
	UINT16  bx = cpu_get_reg(cpu,I8086_BX);
	UINT16  cx = cpu_get_reg(cpu,I8086_CX);
	UINT16  dx = cpu_get_reg(cpu,I8086_DX);
	UINT16  cs = cpu_get_reg(cpu,I8086_CS);
	UINT16  ds = cpu_get_reg(cpu,I8086_DS);
	UINT16  es = cpu_get_reg(cpu,I8086_ES);
	UINT16  ss = cpu_get_reg(cpu,I8086_SS);

	UINT16  si = cpu_get_reg(cpu,I8086_SI);
	UINT16  di = cpu_get_reg(cpu,I8086_DI);
	UINT16  bp = cpu_get_reg(cpu,I8086_BP);

	logerror("=======================================================================\n");
	logerror("DOS Int 0x21 call at %05X\n",pc); 
	logerror("AX=%04X, BX=%04X, CX=%04X, DX=%04X\n",ax,bx,cx,dx);
	logerror("CS=%04X, DS=%04X, ES=%04X, SS=%04X\n",cs,ds,es,ss);
	logerror("SI=%04X, DI=%04X, BP=%04X\n",si,di,bp);
	logerror("=======================================================================\n");

	if((ax & 0xff00)==0x0900)
		debugger_break(device->machine);
}


