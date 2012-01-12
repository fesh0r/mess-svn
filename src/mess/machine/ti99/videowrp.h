/*
    TI-99/4(A) and /8 Video subsystem
    This device actually wraps the naked video chip implementation
    Michael Zapf, October 2010
*/

#ifndef __TIVIDEO__
#define __TIVIDEO__

#include "video/tms9928a.h"
#include "video/v9938.h"

#define TMS9928A_TAG	"tms9928a"
#define SCREEN_TAG		"screen"

#define READ8Z_DEVICE_HANDLER(name)		void name(ATTR_UNUSED device_t *device, ATTR_UNUSED offs_t offset, UINT8 *value)

#define TI_TMS991X 1
#define TI_V9938 2

typedef struct _ti99_video_config
{
	int							chip;
	void						(*callback)(device_t *, v99x8_device &, int);

} ti99_video_config;

READ16_DEVICE_HANDLER( ti_tms991x_r16 );
READ8Z_DEVICE_HANDLER( ti8_tms991x_rz );
WRITE16_DEVICE_HANDLER( ti_tms991x_w16 );
WRITE8_DEVICE_HANDLER( ti8_tms991x_w );
READ16_DEVICE_HANDLER( ti_video_rnop );
WRITE16_DEVICE_HANDLER( ti_video_wnop );
READ16_DEVICE_HANDLER( ti_v9938_r16 );
WRITE16_DEVICE_HANDLER( ti_v9938_w16 );
READ8Z_DEVICE_HANDLER( gen_v9938_rz );
WRITE8_DEVICE_HANDLER( gen_v9938_w );

void video_update_mouse( device_t *device, int delta_x, int delta_y, int buttons);

DECLARE_LEGACY_DEVICE( TIVIDEO, ti99_video );

#define MCFG_TI_TMS991x_ADD_NTSC(_tag, _chip, _tmsparam)	\
	MCFG_DEVICE_ADD(_tag, TIVIDEO, 0)										\
	MCFG_DEVICE_CONFIG_DATA32(ti99_video_config, chip, TI_TMS991X)			\
	MCFG_TMS9928A_ADD( TMS9928A_TAG, _chip, _tmsparam ) 					\
	MCFG_TMS9928A_SCREEN_ADD_NTSC( SCREEN_TAG ) 							\
	MCFG_SCREEN_UPDATE_DEVICE( TMS9928A_TAG, tms9928a_device, screen_update )

#define MCFG_TI_TMS991x_ADD_PAL(_tag, _chip, _screen_update, _tmsparam)		\
	MCFG_DEVICE_ADD(_tag, TIVIDEO, 0)										\
	MCFG_DEVICE_CONFIG_DATA32(ti99_video_config, chip, TI_TMS991X)			\
	MCFG_TMS9928A_ADD( TMS9928A_TAG, _chip, _tmsparam )						\
	MCFG_TMS9928A_SCREEN_ADD_PAL( SCREEN_TAG )								\
	MCFG_SCREEN_UPDATE_DEVICE( TMS9928A_TAG, tms9928a_device, screen_update )

#define MCFG_TI_V9938_ADD(_tag, _rate, _screen, _blank, _x, _y, _int)		\
	MCFG_DEVICE_ADD(_tag, TIVIDEO, 0)										\
	MCFG_DEVICE_CONFIG_DATAPTR(ti99_video_config, callback, _int)			\
	MCFG_DEVICE_CONFIG_DATA32(ti99_video_config, chip, TI_V9938)			\
	MCFG_V9938_ADD(_tag "_v9938", _screen, 0x20000)							\
	MCFG_V99X8_INTERRUPT_CALLBACK_STATIC(_int)								\
	MCFG_SCREEN_ADD(_screen, RASTER)										\
	MCFG_SCREEN_REFRESH_RATE(_rate)											\
	MCFG_SCREEN_UPDATE_DEVICE(_tag "_v9938", v9938_device, screen_update)	\
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(_blank))					\
	MCFG_SCREEN_SIZE(_x, _y)												\
	MCFG_SCREEN_VISIBLE_AREA(0, _x - 1, 0, _y - 1)							\
	MCFG_PALETTE_LENGTH(512)												\
	MCFG_PALETTE_INIT(v9938)

#endif /* __TIVIDEO__ */

