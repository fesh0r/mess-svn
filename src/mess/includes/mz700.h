/******************************************************************************
 *  Sharp MZ700
 *
 *  variables and function prototypes
 *
 *  Juergen Buchmueller <pullmoll@t-online.de>, Jul 2000
 *
 *  Reference: http://sharpmz.computingmuseum.com
 *
 ******************************************************************************/

#ifndef MZ700_H_
#define MZ700_H_


/*----------- defined in machine/mz700.c -----------*/

DRIVER_INIT(mz700);
MACHINE_RESET(mz700);

 READ8_HANDLER ( mz700_mmio_r );
WRITE8_HANDLER ( mz700_mmio_w );
WRITE8_HANDLER ( mz700_bank_w );

extern  READ8_HANDLER( mz800_crtc_r );
extern  READ8_HANDLER( mz800_mmio_r );
extern  READ8_HANDLER( mz800_bank_r );
extern  READ8_HANDLER( mz800_ramdisk_r );

extern WRITE8_HANDLER( mz800_write_format_w );
extern WRITE8_HANDLER( mz800_read_format_w );
extern WRITE8_HANDLER( mz800_display_mode_w );
extern WRITE8_HANDLER( mz800_scroll_border_w );
extern WRITE8_HANDLER( mz800_mmio_w );
extern WRITE8_HANDLER ( mz800_bank_w );
extern WRITE8_HANDLER( mz800_ramdisk_w );
extern WRITE8_HANDLER( mz800_ramaddr_w );
extern WRITE8_HANDLER( mz800_palette_w );

extern WRITE8_HANDLER( videoram0_w );
extern WRITE8_HANDLER( videoram1_w );
extern WRITE8_HANDLER( videoram2_w );
extern WRITE8_HANDLER( videoram3_w );
extern WRITE8_HANDLER( pcgram_w );

extern DRIVER_INIT( mz800 );


/*----------- defined in video/mz700.c -----------*/

extern int mz700_frame_time;

//extern void mz700_init_colors (unsigned char *palette, unsigned short *colortable, const unsigned char *color_prom);
extern PALETTE_INIT(mz700);
//extern int mz700_vh_start (void);
//extern void mz700_vh_stop (void);
//extern void mz700_vh_screenrefresh (mame_bitmap *bitmap, int full_refresh);
extern VIDEO_START(mz700);
extern VIDEO_UPDATE(mz700);


#endif /* MZ700_H_ */
