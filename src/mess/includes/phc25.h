#ifndef __PHC25__
#define __PHC25__

#define SCREEN_TAG		"screen"
#define Z80_TAG			"z80"
#define AY8910_TAG		"ay8910"
#define MC6847_TAG		"mc6847"
#define CASSETTE_TAG	"cassette"
#define CENTRONICS_TAG	"centronics"

#define PHC25_VIDEORAM_SIZE		0x1800

class phc25_state : public driver_device
{
public:
	phc25_state(running_machine &machine, const driver_device_config_base &config)
		: driver_device(machine, config) { }

	/* video state */
	UINT8 *video_ram;
	UINT8 *char_rom;
	UINT8 char_size;
	UINT8 char_correct;
	UINT8 char_substact;

	/* devices */
	device_t *mc6847;
	device_t *centronics;
	device_t *cassette;
};

#endif
