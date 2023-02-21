#include "BluetoothA2DPSink.h"

BluetoothA2DPSink a2dp_sink;

void setup() {

	// The pin config as per the setup
	
	// on esPod, pins 5, 16, 17, resp.

	const i2s_pin_config_t pin_config = {
		.bck_io_num = 5,   						// Serial Clock (SCK)
		.ws_io_num = 16,    					// Word Select (WS)
		.data_out_num = 17,				 		// not used (only for speakers)
		.data_in_num = I2S_PIN_NO_CHANGE,		// Serial Data (SD)
	};

	a2dp_sink.set_pin_config(pin_config);

	a2dp_sink.set_volume(220);
	a2dp_sink.start("fais peter les watts");
}

void loop() {
}