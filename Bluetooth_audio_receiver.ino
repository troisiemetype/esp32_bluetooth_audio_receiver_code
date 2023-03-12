#include "BluetoothA2DPSink.h"

#include "display.h"

const uint8_t PIN_LED_STATUS = 14;

const uint8_t PIN_BTN_1 = 32;
const uint8_t PIN_BTN_BOOT = 0;						// Left  front button

const uint8_t PIN_DAC_FILTER = 15;
const uint8_t PIN_DAC_DEMP = 18;
const uint8_t PIN_DAC_SOFT_MUTE = 19;


BluetoothA2DPSink a2dp_sink;

/*
 *	Metadata IDs: 
 *		0x01	track name
 *		0x02	artist name
 *		0x04	album name				//	Is used as a (very long) description for radios.
 *		0x08	track number
 *		0x10	album tracks number
 *		0x20	genre
 *		0x40	playing time 			// Can be -1 if streamoing radio.
 *		
 */
void metadataCB(uint8_t id, const uint8_t* data){
//	Serial.printf("AVRC metadata rsp: attribute id 0x%x, %s\n", id, data);

	switch(id){
		case 0x01:
			display_title((const char*)data);
			break;
		case 0x02:
			display_artist((const char*)data);
			break;
		case 0x04:
			display_album((const char*)data);
			break;
		case 0x40:
			// Seems not to be send by devices.
//			display_playingTime((const char*)data);
			break;
		default:
			// Do nothing
			break;
	}
}

void rssiCB(esp_bt_gap_cb_param_t::read_rssi_delta_param& rssi){
	Serial.printf("new RSSI : %i\n", rssi);
}

void stateChangedCB(esp_a2d_connection_state_t state, void * data){
	Serial.printf("new state : %s\n", a2dp_sink.to_str(state));
	switch(state){
		case ESP_A2D_CONNECTION_STATE_DISCONNECTED:
			display_peer("");
			break;
		case ESP_A2D_CONNECTION_STATE_CONNECTED:
//			log_d("connected to %s", a2dp_sink.get_connected_source_name());
//			display_peer(a2dp_sink.get_connected_source_name());
			break;
	}
}

void dataCB(){

}

void volumeCB(int volume){
//	Serial.printf("volume CB ; new value : %i\n", volume);
	display_volume(volume);
}

void setup() {

	Serial.begin(115200);
//	Serial.println(ESP.getFreeHeap());		// 243824
//	delay(2000);
//	Serial.println(xPortGetCoreID());
//	Serial.printf("setup on core %i\n", xPortGetCoreID());


	pinMode(PIN_LED_STATUS, OUTPUT);

	pinMode(PIN_DAC_FILTER, OUTPUT);
	pinMode(PIN_DAC_DEMP, OUTPUT);
	pinMode(PIN_DAC_SOFT_MUTE, OUTPUT);

	pinMode(PIN_BTN_1, INPUT_PULLUP);
	pinMode(PIN_BTN_BOOT, INPUT_PULLUP);

	bool state = 0;
	for(uint8_t i = 0; i < 12; ++i){
		state ^= 1;
		digitalWrite(PIN_LED_STATUS, state);
		delay(50);
	}

	// The pin config as per the setup
	
	// on esPod, pins 5, 16, 17, resp.

	const i2s_pin_config_t pin_config = {
		.bck_io_num = 16,   					// Serial Clock (SCK)
		.ws_io_num = 5,    						// Word Select (WS)
		.data_out_num = 17,				 		// not used (only for speakers)
		.data_in_num = I2S_PIN_NO_CHANGE,		// Serial Data (SD)
	};


	a2dp_sink.set_pin_config(pin_config);
	a2dp_sink.set_avrc_metadata_attribute_mask(ESP_AVRC_MD_ATTR_TITLE |
												ESP_AVRC_MD_ATTR_ARTIST |
												ESP_AVRC_MD_ATTR_ALBUM |
												ESP_AVRC_MD_ATTR_PLAYING_TIME);
	a2dp_sink.set_avrc_metadata_callback(metadataCB);
	a2dp_sink.set_rssi_callback(rssiCB);
	a2dp_sink.set_on_connection_state_changed(stateChangedCB);
	// This one is for audio in.
//	a2dp_sink.set_on_data_received(dataCB);
	a2dp_sink.set_on_volumechange(volumeCB);
	// Volume is coded from 0 to 127
	a2dp_sink.set_volume(116);
	a2dp_sink.start("fais peter les watts");


	display_init();
	display_setBasic();

	display_update();

	// 143384 for a2dp
	// 144556 for display
	// 129884 for both

//	Serial.println(ESP.getFreeHeap());


	// unmute the DAC.
	digitalWrite(PIN_DAC_SOFT_MUTE, 1);

}

void loop() {
/*	// 128500, 131620 in loop
	Serial.println(ESP.getFreeHeap());
	delay(2000);
*/
//	display_scroll();
//	display_RSSI(getRssi());
//	delay(1000);
	display_peer(a2dp_sink.get_connected_source_name());
	delay(5000);
}
