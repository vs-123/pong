#include <stdio.h>
#include <time.h>

#define YSTAR_IMPLEMENTATION
#include "raylib.h"
#include "ystar.h"

/***************/
/*  CONSTANTS  */
/***************/

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

#define PADDLE_SIZE ((struct Vector2){ .x = 10, .y = 50 })
#define PADDLE_SPEED 0.15

#define BALL_SIZE ((struct Vector2){ .x = 10, .y = 10 })

/********************/
/*  COLOUR PALETTE  */
/********************/

#define COLOUR_BACKGROUND                                                      \
	((struct Color){ .r = 33, .b = 33, .g = 33, .a = 255 })
#define COLOUR_PADDLE ((struct Color){ .r = 221, .b = 221, .g = 221, .a = 255 })
#define COLOUR_BALL ((struct Color){ .r = 221, .b = 221, .g = 221, .a = 255 })

/*************/
/*  GLOBALS  */
/*************/

uint64_t seed = 0;

struct dimensions_t
{
	int width;
	int height;
};

struct paddle_t
{
	struct Color colour;
	float speed;
	struct Vector2 pos;
	struct Vector2 size;
};

struct paddle_t
paddle_init (float x)
{
	return (struct paddle_t){
		.pos    = (struct Vector2){ .x = x, .y = WINDOW_HEIGHT / 2 - PADDLE_SIZE.y },
		.speed  = PADDLE_SPEED,
		.colour = COLOUR_PADDLE,
		.size   = PADDLE_SIZE,
	};
}

void
paddle_render (struct paddle_t *paddle)
{
	DrawRectangleV (paddle->pos,
	                (struct Vector2){ .x = paddle->size.x, .y = paddle->size.y },
	                paddle->colour);
}

struct ball_t
{
	struct Color colour;
	struct Vector2 speed;
	struct Vector2 pos;
	struct Vector2 size;
};

struct ball_t
ball_init (void)
{
	Vector2 ball_speed = { 0 };

	uint32_t d1 = ystar_between (&seed, 10, 100);
	uint32_t d2 = ystar_between (&seed, 10, 100);

	ball_speed.x = 1.0f / d1;
	ball_speed.y = 1.0f / d2;

	return (struct ball_t){
		.pos    = (struct Vector2){ .x = WINDOW_WIDTH / 2 - BALL_SIZE.x,
		                            .y = WINDOW_HEIGHT / 2 - BALL_SIZE.y },
		.speed  = ball_speed,
		.size   = BALL_SIZE,
		.colour = COLOUR_BALL,
	};
}

void
ball_render (struct ball_t *ball)
{
	DrawRectangleV (ball->pos,
	                (struct Vector2){ .x = ball->size.x, .y = ball->size.y },
	                ball->colour);
}

void
ball_update (struct ball_t *ball)
{
	ball->pos.x += ball->speed.x;
	ball->pos.y += ball->speed.y;
}

struct game_t
{
	struct Color bg_colour;
	struct paddle_t paddle_player, paddle_ai;
	struct ball_t ball;
};

struct game_t
game_init (void)
{
	return (struct game_t){
		.bg_colour     = COLOUR_BACKGROUND,
		.paddle_player = paddle_init (0),
		.paddle_ai     = paddle_init (WINDOW_WIDTH - PADDLE_SIZE.x),
		.ball          = ball_init (),
	};
}

void
game_handle_input (struct game_t *game)
{
	if (IsKeyDown (KEY_W) || IsKeyDown (KEY_UP))
		{
			game->paddle_player.pos.y -= game->paddle_player.speed;
		}
	if (IsKeyDown (KEY_S) || IsKeyDown (KEY_DOWN))
		{
			game->paddle_player.pos.y += game->paddle_player.speed;
		}
}

void
game_update (struct game_t *game)
{
	ball_update (&game->ball);
	if (game->ball.pos.y != game->paddle_ai.pos.y)
		{
			if (game->ball.pos.y > game->paddle_ai.pos.y)
				{
					game->paddle_ai.pos.y += game->paddle_ai.speed;
				}
			else
				{

					game->paddle_ai.pos.y -= game->paddle_ai.speed;
				}
		}
}

void
game_render (struct game_t *game)
{
	ClearBackground (COLOUR_BACKGROUND);
	paddle_render (&game->paddle_player);
	paddle_render (&game->paddle_ai);
	ball_render (&game->ball);
}

int
main (void)
{
	InitWindow (WINDOW_WIDTH, WINDOW_HEIGHT, "Pong -- vs-123");
	seed               = time (NULL);
	struct game_t game = game_init ();

	while (!WindowShouldClose ())
		{
			BeginDrawing ();
			game_handle_input (&game);
			game_update (&game);
			game_render (&game);
			EndDrawing ();
		}

	return 0;
}
