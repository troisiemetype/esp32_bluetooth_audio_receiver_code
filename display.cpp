#include "display.h"

TFT_eSPI tft = TFT_eSPI();

TFT_eSprite title = TFT_eSprite(&tft);
TFT_eSprite artist = TFT_eSprite(&tft);
TFT_eSprite album = TFT_eSprite(&tft);

enum scrollState_t{
	IDLE = 0,
	SCROLLING,
	END,
};

struct scrolled_t{
	TFT_eSprite* sprite;
	uint8_t index;
	uint16_t size;
	uint16_t pos;
	uint16_t current;
	scrollState_t state;
	bool scroll;
};

scrolled_t scrolled[3];

void display_init(){
	log_d("init TFT");
	tft.init();
//	tft.invertDisplay(1);
	tft.setRotation(3);
	tft.initDMA();


	scrolled[0].sprite = &title;
	scrolled[1].sprite = &artist;
	scrolled[2].sprite = &album;

	for (uint8_t i = 0; i < 3;++i){
		scrolled[i].index = i;
		scrolled[i].size = 0;
		scrolled[i].pos = 0;
		scrolled[i].scroll = false;
	}


	display_initSprites();
}

void display_initSprites(){
	// Use a 4 bits color palette to save space
	log_d("init sprites");
/*
	title.setColorDepth(8);
	void *ptr = nullptr;

	ptr = title.createSprite(320, 40);

	if(ptr){
		log_d("sprite created");
	} else {
		log_d("not enough memory for Sprite");
		return;
	}
	log_d("init sprite");
	title.fillSprite(DISP_BG);
	title.setScrollRect(0, 0, 320, 40);
	title.setTextColor(DISP_FG);
	title.setTextDatum(TL_DATUM);
	title.loadFont(NotoSans28);
	title.setTextWrap(false);
	log_d("sprite inited");
*/

	log_d("sprite title");
	display_createSprite(&title);

	log_d("sprite artist");
	display_createSprite(&artist);
	log_d("sprite album");
	display_createSprite(&album);

}

bool display_createSprite(TFT_eSprite *sprite, uint16_t width){
	sprite->setColorDepth(8);
	void *ptr = nullptr;

	ptr = sprite->createSprite(width, SPRITE_HEIGHT);

	if(ptr){
		log_d("sprite created");
	} else {
		log_d("not enough memory for Sprite");
		return 0;
	}
	log_d("init sprite");
	sprite->fillSprite(DISP_BG);
	sprite->setScrollRect(0, 0, width, SPRITE_HEIGHT, DISP_BG);
	sprite->setTextColor(DISP_FG);
	sprite->setTextDatum(TL_DATUM);
	sprite->loadFont(NotoSans28);
	sprite->setTextWrap(false);
	log_d("sprite initialized");

	return 1;
}

void display_deleteSprite(TFT_eSprite *sprite){
	log_d("delete sprite");
	/*
	for(uint8_t i = 0; i < 3; ++i){
		if(scrolled[i] == sprite) scrolled[i] = nullptr;
	}
	*/
	sprite->deleteSprite();
}

void display_setBasic(){
//	tft.unloadFont();
	tft.setTextSize(1);
	tft.fillScreen(TFT_BLACK);
	tft.setTextColor(TFT_WHITE, TFT_BLACK);
}

void display_update(){

}

void display_updateSprite(const char* data, TFT_eSprite* sprite, uint8_t position){
	log_d("update sprite data");

	sprite->fillSprite(DISP_BG);
	bool scroll = false;
	uint16_t size = sprite->textWidth(data);
	if(size > 320){
		log_d("setting scroll for size %i", size);
		scroll = true;
//		display_deleteSprite(sprite);
//		display_createSprite(sprite, size);
//		sprite->fillSprite(TFT_ORANGE);

		for(uint8_t i = 0; i < 3; ++i){
			if(scrolled[i].sprite == sprite){
				log_d("store sprite in table %i", i);
				scrolled[i].pos = position;
				scrolled[i].size = size;
				scrolled[i].current = 0;
				scrolled[i].state = IDLE;
				scrolled[i].scroll = true;
				break;
			}
		}
	}
	 else {
		for(uint8_t i = 0; i < 3; ++i){
			if(scrolled[i].sprite == sprite){
				log_d("erase sprite in table %i", i);
				scrolled[i].scroll = false;
			}
		}	 	
	 }

//	sprite->drawRect(0, 0, 320, SPRITE_HEIGHT,TFT_BLACK);
	sprite->drawString(data, 1, 6);

	log_d("push sprite");
	sprite->pushSprite(0, position);
}

void display_title(const char* data){
	log_v("song title : %s", data);
	display_updateSprite(data, &title, HEADER_HEIGHT);
}

void display_artist(const char* data){
	log_v("artist : %s", data);
	display_updateSprite(data, &artist, SPRITE_HEIGHT + HEADER_HEIGHT);
}

void display_album(const char* data){
	log_v("album : %s", data);
	display_updateSprite(data, &album, 2 * SPRITE_HEIGHT + HEADER_HEIGHT);
}

void display_playingTime(const char* data){
	log_v("playing time : %s", data);
}

void display_volume(uint8_t volume){
	if(volume > 127) volume = 127;
	if(volume == 0) volume = 1;

	uint8_t startAngle = 30;

	uint16_t angle = startAngle + (360 - 2 * startAngle) * volume / 127;
	uint8_t radius = HEADER_HEIGHT / 2;
	uint16_t start = DISP_WIDTH - radius;

	tft.fillRect(start - radius, 0, 2 * radius, HEADER_HEIGHT, DISP_BG);
	tft.drawSmoothArc(start, radius, radius - 2,radius - 3, startAngle, 360 - startAngle, DISP_FG_FAINT, DISP_BG, true);
	tft.drawSmoothArc(start, radius, radius - 2,radius - 3, startAngle, angle, DISP_FG, DISP_FG_FAINT, true);

/*	
	uint16_t width = 150;
	uint16_t height = 18;

	uint16_t start = (DISP_WIDTH - width) / 2;
	uint16_t end = (DISP_WIDTH + width) / 2;

	uint16_t volHeight = height * volume / 127;

	width = width * volume / 127;

	tft.fillRect(0, 0, DISP_WIDTH, HEADER_HEIGHT, DISP_BG);

	tft.drawTriangle(start, height + 1, end, height + 1, end, 1, DISP_FG_FAINT);
	tft.fillTriangle(start, height + 1, start + width, height - volHeight + 1, starHEADER_HEIGHT / 2HEADER_HEIGHT / 2t + width, height + 1, DISP_FG_FAINT);
	*/
}

void display_peer(const char* name){
	if(strlen(name) > 0){
		log_d("peer name : %s", name);
	} else {
		log_d("disconnected");
	}
	tft.fillRect(0,0, DISP_WIDTH - HEADER_HEIGHT, HEADER_HEIGHT, DISP_BG);
	tft.setTextColor(DISP_FG);
	tft.setTextDatum(TL_DATUM);
	tft.loadFont(NotoSans16);
	tft.drawString(name, 10, 12);
}

void display_RSSI(uint16_t signal){
	if(signal > 127) signal = 127;
	if(signal == 0) signal = 1;

	uint8_t startAngle = 30;

	uint16_t angle = startAngle + (360 - 2 * startAngle) * signal / 127;
	uint8_t radius = HEADER_HEIGHT / 2;
	tft.fillRect(0, 0, DISP_WIDTH, HEADER_HEIGHT, DISP_BG);
	tft.drawSmoothArc(DISP_WIDTH / 4, radius, radius - 1,radius - 2, startAngle, 360 - startAngle, DISP_FG_FAINT, DISP_BG, true);
	tft.drawSmoothArc(DISP_WIDTH / 4, radius, radius - 1,radius - 2, startAngle, angle, DISP_FG, DISP_FG_FAINT, true);
}

void display_scroll(){
	for(uint8_t i = 0; i < 3; ++i){
		scrolled_t *s = &scrolled[i];
		if(s->scroll){
//			log_d("scrolling sprite at pos %i : 0x%x8", i, (uint32_t)s->sprite);
			if(s->state == IDLE){
				delay(500);
//				log_d("scrolling state : IDLE");
				s->state = SCROLLING;
			} else if(s->state == SCROLLING){
//				log_d("scrolling state : SCROLLING");
				if((++s->current + DISP_WIDTH) >= s->size){
					log_d("scrolling reached end at %i; %i", s->current, s->size);
					s->state = END;
					break;
				}
				s->sprite->scroll(-1, 0);
				s->sprite->pushSprite(0, s->pos);

			} else if(s->state == END){
//				log_d("scrolling state : END");

			}
		}
	}
}
