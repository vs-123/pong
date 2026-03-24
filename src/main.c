#include "raylib.h"
#include <stdio.h>

/********************/
/*  COLOUR PALETTE  */
/********************/

#define COLOUR_BACKGROUND ((struct Color){.r = 33, .b = 33, .g = 33, .a = 255})

int
main (void)
{
	InitWindow (800, 800, "Pong -- vs-123");

   while (!WindowShouldClose()) {
      BeginDrawing();
      ClearBackground(COLOUR_BACKGROUND);
      EndDrawing();
   }

	return 0;
}
