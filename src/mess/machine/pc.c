/***************************************************************************

	machine/pc.c

	Functions to emulate general aspects of the machine
	(RAM, ROM, interrupts, I/O ports)

	The information herein is heavily based on
	'Ralph Browns Interrupt List'
	Release 52, Last Change 20oct96

***************************************************************************/

#include <assert.h>
#include "driver.h"
#include "deprecat.h"
#include "includes/pc.h"

#include "machine/8255ppi.h"
#include "machine/ins8250.h"
#include "machine/mc146818.h"
#include "machine/pic8259.h"
#include "machine/pc_turbo.h"

#include "video/pc_vga.h"
#include "video/pc_cga.h"
#include "video/pc_aga.h"
#include "video/pc_mda.h"
#include "video/pc_t1t.h"

#include "machine/pit8253.h"

#include "includes/pc_mouse.h"
#include "machine/pckeybrd.h"

#include "includes/pclpt.h"
#include "machine/centroni.h"

#include "machine/pc_fdc.h"
#include "machine/pc_hdc.h"
#include "machine/nec765.h"
#include "includes/amstr_pc.h"
#include "includes/europc.h"
#include "includes/ibmpc.h"
#include "machine/pcshare.h"
#include "devices/cassette.h"
#include "audio/pc.h"

#include "machine/8237dma.h"


#define VERBOSE_PIO 0	/* PIO (keyboard controller) */

#define PIO_LOG(N,M,A) \
	if(VERBOSE_PIO>=N){ if( M )logerror("%11.6f: %-24s",attotime_to_double(timer_get_time()),(char*)M ); logerror A; }

static struct {
	const device_config	*pic8259_master;
	const device_config	*pic8259_slave;
	const device_config	*dma8237_1;
	const device_config	*dma8237_2;
} pc_devices;

/*************************************************************************
 *
 *      PC DMA stuff
 *
 *************************************************************************/

static UINT8 dma_offset[2][4];


READ8_HANDLER(pc_page_r)
{
	return 0xFF;
}


WRITE8_HANDLER(pc_page_w)
{
	switch(offset % 4)
	{
	case 1:
		dma_offset[0][2] = data;
		break;
	case 2:
		dma_offset[0][3] = data;
		break;
	case 3:
		dma_offset[0][0] = dma_offset[0][1] = data;
		break;
	}
}


static DMA8237_MEM_READ( pc_dma_read_byte )
{
	UINT8 result;
	offs_t page_offset = (((offs_t) dma_offset[0][channel]) << 16)
		& 0x0F0000;

	cpuintrf_push_context(0);
	result = program_read_byte(page_offset + offset);
	cpuintrf_pop_context();

	return result;
}


static DMA8237_MEM_WRITE( pc_dma_write_byte )
{
	offs_t page_offset = (((offs_t) dma_offset[0][channel]) << 16)
		& 0x0F0000;

	cpuintrf_push_context(0);
	program_write_byte(page_offset + offset, data);
	cpuintrf_pop_context();
}


static DMA8237_CHANNEL_READ( pc_dma8237_fdc_dack_r )
{
	return pc_fdc_dack_r();
}


static DMA8237_CHANNEL_READ( pc_dma8237_hdc_dack_r )
{
	return pc_hdc_dack_r();
}


static DMA8237_CHANNEL_WRITE( pc_dma8237_fdc_dack_w )
{
	pc_fdc_dack_w( data );
}


static DMA8237_CHANNEL_WRITE( pc_dma8237_hdc_dack_w )
{
	pc_hdc_dack_w( data );
}


static DMA8237_OUT_EOP( pc_dma8237_out_eop )
{
	pc_fdc_set_tc_state( state );
}


const struct dma8237_interface pc_dma8237_config =
{
	0,
	1.0e-6, // 1us

	pc_dma_read_byte,
	pc_dma_write_byte,

	{ 0, 0, pc_dma8237_fdc_dack_r, pc_dma8237_hdc_dack_r },
	{ 0, 0, pc_dma8237_fdc_dack_w, pc_dma8237_hdc_dack_w },
	pc_dma8237_out_eop
};


/*************************************************************
 *
 * pic8259 configuration
 *
 *************************************************************/

static PIC8259_SET_INT_LINE( pc_pic8259_master_set_int_line )
{
	cpunum_set_input_line(device->machine, 0, 0, interrupt ? HOLD_LINE : CLEAR_LINE);
}


static PIC8259_SET_INT_LINE( pc_pic8259_slave_set_int_line )
{
	pic8259_set_irq_line( pc_devices.pic8259_master, 2, interrupt);
}


const struct pic8259_interface pc_pic8259_master_config =
{
	pc_pic8259_master_set_int_line
};


const struct pic8259_interface pc_pic8259_slave_config =
{
	pc_pic8259_slave_set_int_line
};


/*************************************************************
 *
 * PCJR pic8259 configuration
 *
 * Part of the PCJR CRT POST test at address F0452/F0454 writes
 * to the PIC enabling an IRQ which is then immediately fired,
 * however it is expected that the actual IRQ is taken one
 * instruction later (the irq bit is reset by the instruction
 * at F0454). Delaying taking of an IRQ by one instruction for
 * all cases breaks floppy emulation. This seems to be a really
 * tight corner case. For now we delay the IRQ by one instruction
 * only for the PCJR and only when it's inside the POST checks.
 *
 *************************************************************/

static emu_timer	*pc_int_delay_timer;


static TIMER_CALLBACK( pcjr_delayed_pic8259_irq )
{
    cpunum_set_input_line(pc_devices.pic8259_master->machine, 0, 0, param ? HOLD_LINE : CLEAR_LINE);
}


static PIC8259_SET_INT_LINE( pcjr_pic8259_master_set_int_line )
{
	if ( cpunum_get_reg( 0, REG_PC ) == 0xF0454 )
	{
		timer_adjust_oneshot( pc_int_delay_timer, ATTOTIME_IN_CYCLES(1,0), interrupt );
	}
	else
	{
		cpunum_set_input_line(device->machine, 0, 0, interrupt ? HOLD_LINE : CLEAR_LINE);
	}
}


const struct pic8259_interface pcjr_pic8259_master_config =
{
	pcjr_pic8259_master_set_int_line
};


/*************************************************************
 *
 * pit8253 configuration
 *
 *************************************************************/

static PIT8253_OUTPUT_CHANGED( pc_timer0_w )
{
	pic8259_set_irq_line(pc_devices.pic8259_master, 0, state);
}
 

const struct pit8253_config pc_pit8253_config =
{
	{
		{
			4772720/4,				/* heartbeat IRQ */
			pc_timer0_w,
			NULL
		}, {
			4772720/4,				/* dram refresh */
			NULL,
			NULL
		}, {
			4772720/4,				/* pio port c pin 4, and speaker polling enough */
			NULL,
			pc_sh_speaker_change_clock
		}
	}
};


/*
  On the PC Jr the input for clock 1 seems to be selectable
  based on bit 4(/5?) written to output port A0h. This is not
  supported yet.
 */

const struct pit8253_config pcjr_pit8253_config =
{
	{
		{
			4772720/4,              /* heartbeat IRQ */
			pc_timer0_w,
			NULL
		}, {
			4772720/4,              /* dram refresh */
			NULL,
			NULL
		}, {
			4772720/4,              /* pio port c pin 4, and speaker polling enough */
			NULL,
			NULL					/* TIMER AUDIO signal */
		}
	}
};

/**********************************************************
 *
 * COM hardware
 *
 **********************************************************/

/* called when a interrupt is set/cleared from com hardware */
static INS8250_INTERRUPT( pc_com_interrupt_1 )
{
	pic8259_set_irq_line(pc_devices.pic8259_master, 4, state);
}

static INS8250_INTERRUPT( pc_com_interrupt_2 )
{
	pic8259_set_irq_line(pc_devices.pic8259_master, 3, state);
}

/* called when com registers read/written - used to update peripherals that
are connected */
static void pc_com_refresh_connected_common(const device_config *device, int n, int data)
{
	/* mouse connected to this port? */
	if (input_port_read_indexed(device->machine, 3) & (0x80>>n))
		pc_mouse_handshake_in(device,data);
}

static INS8250_HANDSHAKE_OUT( pc_com_handshake_out_0 ) { pc_com_refresh_connected_common( device, 0, data ); }
static INS8250_HANDSHAKE_OUT( pc_com_handshake_out_1 ) { pc_com_refresh_connected_common( device, 1, data ); }
static INS8250_HANDSHAKE_OUT( pc_com_handshake_out_2 ) { pc_com_refresh_connected_common( device, 2, data ); }
static INS8250_HANDSHAKE_OUT( pc_com_handshake_out_3 ) { pc_com_refresh_connected_common( device, 3, data ); }

/* PC interface to PC-com hardware. Done this way because PCW16 also
uses PC-com hardware and doesn't have the same setup! */
const ins8250_interface ibmpc_com_interface[4]=
{
	{
		1843200,
		pc_com_interrupt_1,
		NULL,
		pc_com_handshake_out_0,
		NULL
	},
	{
		1843200,
		pc_com_interrupt_2,
		NULL,
		pc_com_handshake_out_1,
		NULL
	},
	{
		1843200,
		pc_com_interrupt_1,
		NULL,
		pc_com_handshake_out_2,
		NULL
	},
	{
		1843200,
		pc_com_interrupt_2,
		NULL,
		pc_com_handshake_out_3,
		NULL
	}
};


/**********************************************************
 *
 * LPT interface
 *
 **********************************************************/

static const PC_LPT_CONFIG lpt_config[3]=
{
	{
		1,
		LPT_UNIDIRECTIONAL,
		NULL
	},
	{
		1,
		LPT_UNIDIRECTIONAL,
		NULL
	},
	{
		1,
		LPT_UNIDIRECTIONAL,
		NULL
	}
};

static const CENTRONICS_CONFIG cent_config[3]=
{
	{
		PRINTER_IBM,
		pc_lpt_handshake_in
	},
	{
		PRINTER_IBM,
		pc_lpt_handshake_in
	},
	{
		PRINTER_IBM,
		pc_lpt_handshake_in
	}
};



/**********************************************************
 *
 * NMI handling
 *
 **********************************************************/

static UINT8	nmi_enabled;

WRITE8_HANDLER( pc_nmi_enable_w )
{
	logerror( "%08X: changing NMI state to %s\n", activecpu_get_pc(), data & 0x80 ? "enabled" : "disabled" );

	nmi_enabled = data & 0x80;
}

/*************************************************************
 *
 * PCJR NMI and raw keybaord handling
 *
 * raw signals on the keyboard cable:
 * ---_-b0b1b2b3b4b5b6b7pa----------------------
 *    | | | | | | | | | | |
 *    | | | | | | | | | | *--- 11 stop bits ( -- = 1 stop bit )
 *    | | | | | | | | | *----- parity bit ( 0 = _-, 1 = -_ )
 *    | | | | | | | | *------- bit 7 ( 0 = _-, 1 = -_ )
 *    | | | | | | | *--------- bit 6 ( 0 = _-, 1 = -_ )
 *    | | | | | | *----------- bit 5 ( 0 = _-, 1 = -_ )
 *    | | | | | *------------- bit 4 ( 0 = _-, 1 = -_ )
 *    | | | | *--------------- bit 3 ( 0 = _-, 1 = -_ )
 *    | | | *----------------- bit 2 ( 0 = _-, 1 = -_ )
 *    | | *------------------- bit 1 ( 0 = _-, 1 = -_ )
 *    | *--------------------- bit 0 ( 0 = _-, 1 = -_ )
 *    *----------------------- start bit (always _- )
 *
 * An entire bit lasts for 440 uSec, half bit time is 220 uSec.
 * Transferring an entire byte takes 21 x 440uSec. The extra
 * time of the stop bits is to allow the CPU to do other things
 * besides decoding keyboard signals.
 *
 * These signals get inverted before going to the PCJR
 * handling hardware. The sequence for the start then
 * becomes:
 *
 * __-_b0b1.....
 *   |
 *   *---- on the 0->1 transition of the start bit a keyboard
 *         latch signal is set to 1 and an NMI is generated
 *         when enabled.
 *         The keyboard latch is reset by reading from the
 *         NMI enable port (A0h).
 *
 *************************************************************/

static struct {
	UINT8		transferring;
	UINT8		latch;
	UINT32		raw_keyb_data;
	int			signal_count;
	emu_timer	*keyb_signal_timer;
} pcjr_keyb;


READ8_HANDLER( pcjr_nmi_enable_r )
{
	pcjr_keyb.latch = 0;

	return nmi_enabled;
}


static TIMER_CALLBACK( pcjr_keyb_signal_callback )
{
	pcjr_keyb.raw_keyb_data = pcjr_keyb.raw_keyb_data >> 1;
	pcjr_keyb.signal_count--;

	if ( pcjr_keyb.signal_count <= 0 )
	{
		timer_adjust_periodic( pcjr_keyb.keyb_signal_timer, attotime_never, 0, attotime_never );
		pcjr_keyb.transferring = 0;
	}
}


static void pcjr_set_keyb_int(int state)
{
	if ( state )
	{
		UINT8	data = pc_keyb_read();
		UINT8	parity = 0;
		int		i;

		/* Calculate the raw data */
		for( i = 0; i < 8; i++ )
		{
			if ( ( 1 << i ) & data )
			{
				parity ^= 1;
			}
		}
		pcjr_keyb.raw_keyb_data = 0;
		pcjr_keyb.raw_keyb_data = ( pcjr_keyb.raw_keyb_data << 2 ) | ( parity ? 1 : 2 );
		for( i = 0; i < 8; i++ )
		{
			pcjr_keyb.raw_keyb_data = ( pcjr_keyb.raw_keyb_data << 2 ) | ( ( data & 0x80 ) ? 1 : 2 );
			data <<= 1;
		}
		/* Insert start bit */
		pcjr_keyb.raw_keyb_data = ( pcjr_keyb.raw_keyb_data << 2 ) | 1;
		pcjr_keyb.signal_count = 20 + 22;

		/* we are now transferring a byte of keyboard data */
		pcjr_keyb.transferring = 1;

		/* Set timer */
		timer_adjust_periodic( pcjr_keyb.keyb_signal_timer, ATTOTIME_IN_USEC(220), 0, ATTOTIME_IN_USEC(220) );

		/* Trigger NMI */
		if ( ! pcjr_keyb.latch )
		{
			pcjr_keyb.latch = 1;
			if ( nmi_enabled & 0x80 )
			{
				cpunum_set_input_line( Machine, 0, INPUT_LINE_NMI, PULSE_LINE );
			}
		}
	}
}


static void pcjr_keyb_init(void)
{
	pcjr_keyb.transferring = 0;
	pcjr_keyb.latch = 0;
	pcjr_keyb.raw_keyb_data = 0;
	pcjr_keyb.keyb_signal_timer = timer_alloc( pcjr_keyb_signal_callback, NULL );
	pc_keyb_set_clock( 1 );
}



/**********************************************************
 *
 * PPI8255 interface
 *
 **********************************************************/

static struct {
	int portc_switch_high;
	int speaker;
	int keyboard_disabled;
	UINT8	portb;
} pc_ppi={ 0 };


static READ8_HANDLER (pc_ppi_porta_r)
{
	int data;

	/* KB port A */
	if (pc_ppi.keyboard_disabled)
	{
		/*   0  0 - no floppy drives
		 *   1  Not used
		 * 2-3  The number of memory banks on the system board
		 * 4-5  Display mode
		 *	    11 = monochrome
		 *      10 - color 80x25
		 *      01 - color 40x25
		 * 6-7  The number of floppy disk drives
		 */
		data = input_port_read_indexed(machine, 1);
	}
	else
	{
		data = pc_keyb_read();
	}
    PIO_LOG(1,"PIO_A_r",("$%02x\n", data));
    return data;
}


static READ8_HANDLER (pc_ppi_portb_r )
{
	int data;

	data = 0xff;
	PIO_LOG(1,"PIO_B_r",("$%02x\n", data));
	return data;
}


static READ8_HANDLER ( pc_ppi_portc_r )
{
	int timer2_output = pit8253_get_output( device_list_find_by_tag( machine->config->devicelist, PIT8253, "pit8253" ), 2 );
	int data=0xff;

	data&=~0x80; // no parity error
	data&=~0x40; // no error on expansion board
	/* KB port C: equipment flags */
//	if (pc_port[0x61] & 0x08)
	if (pc_ppi.portc_switch_high)
	{
		/* read hi nibble of S2 */
		data = (data&0xf0)|((input_port_read_indexed(machine, 1) >> 4) & 0x0f);
		PIO_LOG(1,"PIO_C_r (hi)",("$%02x\n", data));
	}
	else
	{
		/* read lo nibble of S2 */
		data = (data&0xf0)|(input_port_read_indexed(machine, 1) & 0x0f);
		PIO_LOG(1,"PIO_C_r (lo)",("$%02x\n", data));
	}

	if ( ! ( pc_ppi.portb & 0x08 ) )
	{
		double tap_val = cassette_input( image_from_devtype_and_index( IO_CASSETTE, 0 ) );

		if ( tap_val < 0 )
		{
			data &= ~0x10;
		}
		else
		{
			data |= 0x10;
		}
	}
	else
	{
		if ( pc_ppi.portb & 0x01 )
		{
			data = ( data & ~0x10 ) | ( timer2_output ? 0x10 : 0x00 );
		}
	}
	data = ( data & ~0x20 ) | ( timer2_output ? 0x20 : 0x00 );

	return data;
}


static WRITE8_HANDLER ( pc_ppi_porta_w )
{
	/* KB controller port A */
	PIO_LOG(1,"PIO_A_w",("$%02x\n", data));
}


static WRITE8_HANDLER ( pc_ppi_portb_w )
{
	/* KB controller port B */
	pc_ppi.portb = data;
	pc_ppi.portc_switch_high = data & 0x08;
	pc_ppi.keyboard_disabled = data & 0x80;
	pit8253_gate_w( device_list_find_by_tag( machine->config->devicelist, PIT8253, "pit8253" ), 2, data & 1);
	pc_sh_speaker(machine, data & 0x03);
	pc_keyb_set_clock(data & 0x40);

	cassette_change_state( image_from_devtype_and_index( IO_CASSETTE, 0 ), ( data & 0x08 ) ? CASSETTE_MOTOR_DISABLED : CASSETTE_MOTOR_ENABLED, CASSETTE_MASK_MOTOR);

	if (data & 0x80)
		pc_keyb_clear();
}


static WRITE8_HANDLER ( pc_ppi_portc_w )
{
	/* KB controller port C */
	PIO_LOG(1,"PIO_C_w",("$%02x\n", data));
}


/* PC-XT has a 8255 which is connected to keyboard and other
status information */
const ppi8255_interface pc_ppi8255_interface =
{
	pc_ppi_porta_r,
	pc_ppi_portb_r,
	pc_ppi_portc_r,
	pc_ppi_porta_w,
	pc_ppi_portb_w,
	pc_ppi_portc_w
};


static WRITE8_HANDLER ( pcjr_ppi_portb_w )
{
	/* KB controller port B */
	pc_ppi.portb = data;
	pc_ppi.portc_switch_high = data & 0x08;
	pit8253_gate_w( device_list_find_by_tag( machine->config->devicelist, PIT8253, "pit8253" ), 2, data & 1);
	pc_sh_speaker(machine, data & 0x03);

	cassette_change_state( image_from_devtype_and_index( IO_CASSETTE, 0 ), ( data & 0x08 ) ? CASSETTE_MOTOR_DISABLED : CASSETTE_MOTOR_ENABLED, CASSETTE_MASK_MOTOR);
}


/*
 * On a PCJR none of the port A bits are connected.
 */
static READ8_HANDLER (pcjr_ppi_porta_r )
{
	int data;

	data = 0xff;
	PIO_LOG(1,"PIO_A_r",("$%02x\n", data));
	return data;
}


/*
 * Port C connections on a PCJR (notes from schematics):
 * PC0 - KYBD LATCH
 * PC1 - MODEM CD INSTALLED
 * PC2 - DISKETTE CD INSTALLED
 * PC3 - ATR CD IN
 * PC4 - cassette audio
 * PC5 - OUT2 from 8253
 * PC6 - KYBD IN
 * PC7 - (keyboard) CABLE CONNECTED
 */
static READ8_HANDLER ( pcjr_ppi_portc_r )
{
	int timer2_output = pit8253_get_output( device_list_find_by_tag( machine->config->devicelist, PIT8253, "pit8253" ), 2 );
	int data=0xff;

	data&=~0x80;
	data &= ~0x04;		/* floppy drive installed */
	data = ( data & ~0x01 ) | ( pcjr_keyb.latch ? 0x01: 0x00 );
	if ( ! ( pc_ppi.portb & 0x08 ) )
	{
		double tap_val = cassette_input( image_from_devtype_and_index( IO_CASSETTE, 0 ) );

		if ( tap_val < 0 )
		{
			data &= ~0x10;
		}
		else
		{
			data |= 0x10;
		}
	}
	else
	{
		if ( pc_ppi.portb & 0x01 )
		{
			data = ( data & ~0x10 ) | ( timer2_output ? 0x10 : 0x00 );
		}
	}
	data = ( data & ~0x20 ) | ( timer2_output ? 0x20 : 0x00 );
	data = ( data & ~0x40 ) | ( ( pcjr_keyb.raw_keyb_data & 0x01 ) ? 0x40 : 0x00 );

	return data;
}


const ppi8255_interface pcjr_ppi8255_interface =
{
	pcjr_ppi_porta_r,
	pc_ppi_portb_r,
	pcjr_ppi_portc_r,
	pc_ppi_porta_w,
	pcjr_ppi_portb_w,
	pc_ppi_portc_w
};


/**********************************************************
 *
 * NEC uPD765 floppy interface
 *
 **********************************************************/

#define FDC_DMA 2

static void pc_fdc_interrupt(int state)
{
	if ( pc_devices.pic8259_master ) {
		pic8259_set_irq_line(pc_devices.pic8259_master, 6, state);
	}
}

static void pc_fdc_dma_drq(int state, int read_)
{
	dma8237_drq_write( (device_config*)device_list_find_by_tag( Machine->config->devicelist, DMA8237, "dma8237" ), FDC_DMA, state);
}


static const struct pc_fdc_interface fdc_interface_nc =
{
	NEC765A,
	NEC765_RDY_PIN_NOT_CONNECTED,
	pc_fdc_interrupt,
	pc_fdc_dma_drq,
};

static void pc_set_irq_line(int irq, int state) {
	pic8259_set_irq_line(pc_devices.pic8259_master, irq, state);
}

static void pc_set_keyb_int(int state) {
	pc_set_irq_line( 1, state );
}

void mess_init_pc_common(UINT32 flags, void (*set_keyb_int_func)(int), void (*set_hdc_int_func)(int,int)) {
	init_pc_common(flags, set_keyb_int_func);

	/* MESS managed RAM */
	if ( mess_ram )
		memory_set_bankptr( 10, mess_ram );

	/* FDC/HDC hardware */
	pc_hdc_setup(set_hdc_int_func);

	pc_lpt_config(0, lpt_config);
	centronics_config(0, cent_config);
	pc_lpt_set_device(0, &CENTRONICS_PRINTER_DEVICE);
	pc_lpt_config(1, lpt_config+1);
	centronics_config(1, cent_config+1);
	pc_lpt_set_device(1, &CENTRONICS_PRINTER_DEVICE);
	pc_lpt_config(2, lpt_config+2);
	centronics_config(2, cent_config+2);
	pc_lpt_set_device(2, &CENTRONICS_PRINTER_DEVICE);

	/* serial mouse */
	pc_mouse_initialise();
}

DRIVER_INIT( pccga )
{
	mess_init_pc_common(PCCOMMON_KEYBOARD_PC, pc_set_keyb_int, pc_set_irq_line);
	pc_rtc_init();
	pc_turbo_setup(0, 3, 0x02, 4.77/12, 1);
}

DRIVER_INIT( bondwell )
{
	mess_init_pc_common(PCCOMMON_KEYBOARD_PC, pc_set_keyb_int, pc_set_irq_line);
	pc_turbo_setup(0, 3, 0x02, 4.77/12, 1);
}

DRIVER_INIT( pcmda )
{
	mess_init_pc_common(PCCOMMON_KEYBOARD_PC, pc_set_keyb_int, pc_set_irq_line);
	pc_turbo_setup(0, 3, 0x02, 4.77/12, 1);
}

DRIVER_INIT( europc )
{
	UINT8 *gfx = &memory_region(REGION_GFX1)[0x8000];
	UINT8 *rom = &memory_region(REGION_CPU1)[0];
	int i;

    /* just a plain bit pattern for graphics data generation */
    for (i = 0; i < 256; i++)
		gfx[i] = i;

	/*
	  fix century rom bios bug !
	  if year <79 month (and not CENTURY) is loaded with 0x20
	*/
	if (rom[0xff93e]==0xb6){ // mov dh,
		UINT8 a;
		rom[0xff93e]=0xb5; // mov ch,
		for (i=0xf8000, a=0; i<0xfffff; i++ ) a+=rom[i];
		rom[0xfffff]=256-a;
	}

	mess_init_pc_common(PCCOMMON_KEYBOARD_PC, pc_set_keyb_int, pc_set_irq_line);

	europc_rtc_init();
//	europc_rtc_set_time(machine);
}

DRIVER_INIT( t1000hx )
{
	mess_init_pc_common(PCCOMMON_KEYBOARD_PC, pc_set_keyb_int, pc_set_irq_line);
	pc_turbo_setup(0, 3, 0x02, 4.77/12, 1);
}

DRIVER_INIT( pc200 )
{
	UINT8 *gfx = &memory_region(REGION_GFX1)[0x8000];
	int i;

    /* just a plain bit pattern for graphics data generation */
    for (i = 0; i < 256; i++)
		gfx[i] = i;

	memory_install_read16_handler(machine, 0, ADDRESS_SPACE_PROGRAM, 0xb0000, 0xbffff, 0, 0, pc200_videoram16le_r );
	memory_install_write16_handler(machine, 0, ADDRESS_SPACE_PROGRAM, 0xb0000, 0xbffff, 0, 0, pc200_videoram16le_w );
	videoram_size=0x10000;
	videoram=memory_region(REGION_CPU1)+0xb0000;
	memory_install_read16_handler(machine, 0, ADDRESS_SPACE_IO, 0x278, 0x27b, 0, 0, pc200_16le_port378_r );

	mess_init_pc_common(PCCOMMON_KEYBOARD_PC, pc_set_keyb_int, pc_set_irq_line);
}

DRIVER_INIT( pc1512 )
{
	UINT8 *gfx = &memory_region(REGION_GFX1)[0x8000];
	int i;

    /* just a plain bit pattern for graphics data generation */
    for (i = 0; i < 256; i++)
		gfx[i] = i;

	memory_install_read16_handler(machine, 0, ADDRESS_SPACE_PROGRAM, 0xb8000, 0xbbfff, 0, 0x0C000, SMH_BANK1 );
	memory_install_write16_handler(machine, 0, ADDRESS_SPACE_PROGRAM, 0xb8000, 0xbbfff, 0, 0x0C000, pc1512_videoram16le_w );

	memory_install_read16_handler(machine, 0, ADDRESS_SPACE_IO, 0x3d0, 0x3df, 0, 0, pc1512_16le_r );
	memory_install_write16_handler(machine, 0, ADDRESS_SPACE_IO, 0x3d0, 0x3df, 0, 0, pc1512_16le_w );

	memory_install_read16_handler(machine, 0, ADDRESS_SPACE_IO, 0x278, 0x27b, 0, 0, pc16le_parallelport2_r );


	mess_init_pc_common(PCCOMMON_KEYBOARD_PC, pc_set_keyb_int, pc_set_irq_line);
	mc146818_init(MC146818_IGNORE_CENTURY);
}


DRIVER_INIT( pcjr )
{
	mess_init_pc_common(PCCOMMON_KEYBOARD_PC, pcjr_set_keyb_int, pc_set_irq_line);
}



static void pc_map_vga_memory(offs_t begin, offs_t end, read8_machine_func rh, write8_machine_func wh)
{
	int buswidth;
	buswidth = cputype_databus_width(Machine->config->cpu[0].type, ADDRESS_SPACE_PROGRAM);
	switch(buswidth)
	{
		case 8:
			memory_install_read8_handler(Machine, 0, ADDRESS_SPACE_PROGRAM, 0xA0000, 0xBFFFF, 0, 0, SMH_NOP);
			memory_install_write8_handler(Machine, 0, ADDRESS_SPACE_PROGRAM, 0xA0000, 0xBFFFF, 0, 0, SMH_NOP);

			memory_install_read8_handler(Machine, 0, ADDRESS_SPACE_PROGRAM, begin, end, 0, 0, rh);
			memory_install_write8_handler(Machine, 0, ADDRESS_SPACE_PROGRAM, begin, end, 0, 0, wh);
			break;

		default:
			fatalerror("VGA:  Bus width %d not supported\n", buswidth);
			break;
	}
}



static const struct pc_vga_interface vga_interface =
{
	1,
	pc_map_vga_memory,

	input_port_0_r,

	ADDRESS_SPACE_IO,
	0x0000
};



DRIVER_INIT( pc1640 )
{
	pc_vga_init(machine, &vga_interface, NULL);
	memory_install_read16_handler(machine, 0, ADDRESS_SPACE_PROGRAM, 0xa0000, 0xaffff, 0, 0, SMH_BANK1 );
	memory_install_read16_handler(machine, 0, ADDRESS_SPACE_PROGRAM, 0xb0000, 0xb7fff, 0, 0, SMH_BANK2 );
	memory_install_read16_handler(machine, 0, ADDRESS_SPACE_PROGRAM, 0xb8000, 0xbffff, 0, 0, SMH_BANK3 );

	memory_install_write16_handler(machine, 0, ADDRESS_SPACE_PROGRAM, 0xa0000, 0xaffff, 0, 0, SMH_BANK1 );
	memory_install_write16_handler(machine, 0, ADDRESS_SPACE_PROGRAM, 0xb0000, 0xb7fff, 0, 0, SMH_BANK2 );
	memory_install_write16_handler(machine, 0, ADDRESS_SPACE_PROGRAM, 0xb8000, 0xbffff, 0, 0, SMH_BANK3 );

	memory_install_read16_handler(machine, 0, ADDRESS_SPACE_IO, 0x3b0, 0x3bf, 0, 0, vga_port16le_03b0_r );
	memory_install_read16_handler(machine, 0, ADDRESS_SPACE_IO, 0x3c0, 0x3cf, 0, 0, paradise_ega16le_03c0_r );
	memory_install_read16_handler(machine, 0, ADDRESS_SPACE_IO, 0x3d0, 0x3df, 0, 0, pc1640_16le_port3d0_r );

	memory_install_write16_handler(machine, 0, ADDRESS_SPACE_IO, 0x3b0, 0x3bf, 0, 0, vga_port16le_03b0_w );
	memory_install_write16_handler(machine, 0, ADDRESS_SPACE_IO, 0x3c0, 0x3cf, 0, 0, vga_port16le_03c0_w );
	memory_install_write16_handler(machine, 0, ADDRESS_SPACE_IO, 0x3d0, 0x3df, 0, 0, vga_port16le_03d0_w );

	memory_install_read16_handler(machine, 0, ADDRESS_SPACE_IO, 0x278, 0x27b, 0, 0, pc1640_16le_port278_r );
	memory_install_read16_handler(machine, 0, ADDRESS_SPACE_IO, 0x4278, 0x427b, 0, 0, pc1640_16le_port4278_r );

	mess_init_pc_common(PCCOMMON_KEYBOARD_PC, pc_set_keyb_int, pc_set_irq_line);

	mc146818_init(MC146818_IGNORE_CENTURY);
}

DRIVER_INIT( pc_vga )
{
	mess_init_pc_common(PCCOMMON_KEYBOARD_PC, pc_set_keyb_int, pc_set_irq_line);

	pc_vga_init(machine, &vga_interface, NULL);
}

static IRQ_CALLBACK(pc_irq_callback)
{
	return pic8259_acknowledge( pc_devices.pic8259_master );
}


MACHINE_START( pc )
{
	pc_fdc_init( &fdc_interface_nc );
}


MACHINE_RESET( pc )
{
	cpunum_set_irq_callback(0, pc_irq_callback);

	pc_devices.pic8259_master = device_list_find_by_tag( machine->config->devicelist, PIC8259, "pic8259_master" );
	pc_devices.pic8259_slave = device_list_find_by_tag( machine->config->devicelist, PIC8259, "pic8259_slave" );
	pc_devices.dma8237_1 = device_list_find_by_tag( machine->config->devicelist, DMA8237, "dma8237_1" );
	pc_devices.dma8237_2 = device_list_find_by_tag( machine->config->devicelist, DMA8237, "dma8237_2" );
	pc_mouse_set_serial_port( device_list_find_by_tag( machine->config->devicelist, INS8250, "ins8250_0" ) );
}


MACHINE_RESET( pcjr )
{
	MACHINE_RESET_CALL( pc );
	pcjr_keyb_init();
	pc_int_delay_timer = timer_alloc( pcjr_delayed_pic8259_irq, NULL );
}


/**************************************************************************
 *
 *      Interrupt handlers.
 *
 **************************************************************************/

INTERRUPT_GEN( pc_frame_interrupt )
{
	pc_keyboard();
}

INTERRUPT_GEN( pc_vga_frame_interrupt )
{
	//vga_timer();
	pc_keyboard();
}

INTERRUPT_GEN( pcjr_frame_interrupt )
{
	if ( pcjr_keyb.transferring == 0 )
	{
		pc_keyboard();
	}
}

