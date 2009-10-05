#ifndef UPD71071_H_
#define UPD71071_H_

#include "driver.h"

typedef struct _upd71071_interface upd71071_intf;
struct _upd71071_interface
{
	const char* cputag;
	int clock;
	UINT16 (*dma_read[4])(running_machine* machine);
	void (*dma_write[4])(running_machine* machine, UINT16 data);
};

void upd71071_dmarq(const device_config* device,int state,int channel);

DEVICE_GET_INFO(upd71071);

#define UPD71071 DEVICE_GET_INFO_NAME(upd71071)

#define MDRV_UPD71071_ADD(_tag, _config) \
	MDRV_DEVICE_ADD(_tag, UPD71071, 0) \
	MDRV_DEVICE_CONFIG(_config)

READ8_DEVICE_HANDLER(upd71071_r);
WRITE8_DEVICE_HANDLER(upd71071_w);

#endif /*UPD71071_H_*/
