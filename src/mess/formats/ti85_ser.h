
#ifndef __TI85SERIAL_H_
#define __TI85SERIAL_H_


DECLARE_LEGACY_IMAGE_DEVICE(TI85SERIAL, ti85serial);
DECLARE_LEGACY_IMAGE_DEVICE(TI86SERIAL, ti86serial);

extern void ti85_update_serial(device_t *device);

#define MCFG_TI85SERIAL_ADD( _tag ) \
		MCFG_DEVICE_ADD( _tag, TI85SERIAL, 0 )

#define MCFG_TI86SERIAL_ADD( _tag ) \
		MCFG_DEVICE_ADD( _tag, TI86SERIAL, 0 )

WRITE8_DEVICE_HANDLER( ti85serial_red_out );
WRITE8_DEVICE_HANDLER( ti85serial_white_out );
READ8_DEVICE_HANDLER( ti85serial_red_in );
READ8_DEVICE_HANDLER( ti85serial_white_in );

#endif
