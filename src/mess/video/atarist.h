#ifndef _VIDEO_ATARIST_H_
#define _VIDEO_ATARIST_H_

#define Y2		32084988.0
#define Y2_NTSC	32042400.0

#define ATARIST_HBSTART_PAL		512
#define ATARIST_HBEND_PAL		0
#define ATARIST_HBSTART_NTSC	508
#define ATARIST_HBEND_NTSC		0
#define ATARIST_HTOT_PAL		516
#define ATARIST_HTOT_NTSC		512

#define ATARIST_HBDEND_PAL		14
#define ATARIST_HBDSTART_PAL	94
#define ATARIST_HBDEND_NTSC		13
#define ATARIST_HBDSTART_NTSC	93

#define ATARIST_VBEND_PAL		0
#define ATARIST_VBEND_NTSC		0
#define ATARIST_VBSTART_PAL		312
#define ATARIST_VBSTART_NTSC	262
#define ATARIST_VTOT_PAL		313
#define ATARIST_VTOT_NTSC		263

#define ATARIST_VBDEND_PAL		63
#define ATARIST_VBDSTART_PAL	263
#define ATARIST_VBDEND_NTSC		34
#define ATARIST_VBDSTART_NTSC	234

READ16_HANDLER( atarist_shifter_base_r );
WRITE16_HANDLER( atarist_shifter_base_w );
READ16_HANDLER( atarist_shifter_counter_r );
READ16_HANDLER( atarist_shifter_sync_r );
WRITE16_HANDLER( atarist_shifter_sync_w );
READ16_HANDLER( atarist_shifter_palette_r );
WRITE16_HANDLER( atarist_shifter_palette_w );
READ16_HANDLER( atarist_shifter_mode_r );
WRITE16_HANDLER( atarist_shifter_mode_w );

READ16_HANDLER( atariste_shifter_base_low_r );
WRITE16_HANDLER( atariste_shifter_base_low_w );
READ16_HANDLER( atariste_shifter_counter_r );
WRITE16_HANDLER( atariste_shifter_counter_w );
READ16_HANDLER( atariste_shifter_lineofs_r );
WRITE16_HANDLER( atariste_shifter_lineofs_w );
READ16_HANDLER( atariste_shifter_pixelofs_r );
WRITE16_HANDLER( atariste_shifter_pixelofs_w );
READ16_HANDLER( atariste_shifter_palette_r );
WRITE16_HANDLER( atariste_shifter_palette_w );

VIDEO_START( atarist );
VIDEO_UPDATE( atarist );

#endif
