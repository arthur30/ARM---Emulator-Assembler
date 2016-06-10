#ifndef PLAYER_H
#define PLAYER_H

#include "entity.h"
#include "color.h"
#include "libarmc/rpi-gpio.h"

#define PLAYER_LIVES 3
#define PLAYER_DIRECTION 90
#define PLAYER_SPEED 50

#define PLAYER_1_RIGHT 2
#define PLAYER_1_LEFT 3

typedef struct {
	entity_t *entity;
	float dir;
	float speed;
	float angular_vel;
	int lives;
	color_t color;
	rpi_gpio_pin_t left_pin;
	rpi_gpio_pin_t right_pin;
} player_t;

player_t *player_new(void);
void player_free(player_t *player);

#endif
