#include "raylib.h"
#include <stdio.h>

/********************/
/*  COLOUR PALETTE  */
/********************/

#define COLOUR_BACKGROUND                                                      \
	((struct Color){ .r = 33, .b = 33, .g = 33, .a = 255 })

struct game_t
{
	Color bg_colour;
};

struct game_t
game_init (void)
{
	return (struct game_t){
		.bg_colour = COLOUR_BACKGROUND,
	};
}

void
game_render (struct game_t *game)
{
	ClearBackground (COLOUR_BACKGROUND);
}

int
main (void)
{
	InitWindow (800, 800, "Pong -- vs-123");
	struct game_t game = game_init ();

	while (!WindowShouldClose ())
		{
			BeginDrawing ();
			game_render (&game);
			EndDrawing ();
		}

	return 0;
}
