#ifndef BAR_DISPLAY_H
#define BAR_DISPLAY_H

#include "TFT_eSPI.h"
#include "fonts.h"

#define DISP_BG			TFT_BLACK
#define DISP_FG			TFT_ORANGE
#define DISP_FG_FAINT	TFT_BROWN

#define DISP_WIDTH  TFT_HEIGHT
#define DISP_HEIGHT TFT_WIDTH

#define SPRITE_HEIGHT	35
#define HEADER_HEIGHT	40

void display_init();
void display_initSprites();

bool display_createSprite(TFT_eSprite *sprite, uint16_t width = DISP_WIDTH);
void display_deleteSprite(TFT_eSprite *sprite);

void display_setBasic();

void display_update();
void display_updateSprite(const char* data, TFT_eSprite* sprite, uint8_t position);

void display_title(const char* data);
void display_artist(const char* data);
void display_album(const char* data);
void display_playingTime(const char* data);
void display_volume(uint8_t volume);

void display_peer(const char* name);

void display_RSSI(uint16_t signal);

void display_scroll();

#endif