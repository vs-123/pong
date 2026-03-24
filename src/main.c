#include "raylib.h"
#include <stdio.h>

/********************/
/*  COLOUR PALETTE  */
/********************/

#define COLOUR_BACKGROUND                                                      \
	((struct Color){ .r = 33, .b = 33, .g = 33, .a = 255 })
#define COLOUR_PADDLE ((struct Color){ .r = 221, .b = 221, .g = 221, .a = 255 })

#define WINDOW_WIDTH 800
#define PADDLE_SIZE ((struct dimensions_t){ .width = 10, .height = 50 })

struct dimensions_t
{
	int width;
	int height;
};

struct paddle_t
{
	int x;
	int y;
	int speed;
	struct Color colour;
	struct dimensions_t size;
};

struct paddle_t
paddle_init (int x)
{
	return (struct paddle_t){
		.x      = x,
		.y      = 0,
		.speed  = 1,
		.colour = COLOUR_PADDLE,
		.size   = PADDLE_SIZE,
	};
}

void
paddle_render (struct paddle_t *paddle)
{
	DrawRectangle (paddle->x,
	               paddle->y,
	               paddle->size.width,
	               paddle->size.height,
	               paddle->colour);
}

struct game_t
{
	struct Color bg_colour;
	struct paddle_t paddle_player, paddle_ai;
};

struct game_t
game_init (void)
{
	return (struct game_t){
		.bg_colour     = COLOUR_BACKGROUND,
		.paddle_player = paddle_init (0),
		.paddle_ai     = paddle_init (WINDOW_WIDTH - PADDLE_SIZE.width),
	};
}

void
game_render (struct game_t *game)
{
	ClearBackground (COLOUR_BACKGROUND);
	paddle_render (&game->paddle_player);
	paddle_render (&game->paddle_ai);
}

int
main (void)
{
	InitWindow (WINDOW_WIDTH, WINDOW_WIDTH, "Pong -- vs-123");
	struct game_t game = game_init ();

	while (!WindowShouldClose ())
		{
			BeginDrawing ();
			game_render (&game);
			EndDrawing ();
		}

	return 0;
}
