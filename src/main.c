#include <assert.h>
#include <math.h>
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

#define PADDLE_SPEED 250.0f

#define BALL_SPEED 250.0f

#define BALL_SIZE ((struct Vector2){ .x = 10, .y = 10 })

#define MAX_PARTICLES 100

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

struct paddle_t paddle_init (float x);

void paddle_render (struct paddle_t *paddle);

struct ball_t
{
	struct Color colour;
	struct Vector2 speed;
	struct Vector2 pos;
	struct Vector2 size;
};

struct ball_t ball_init (void);

void ball_render (struct ball_t *ball);

void ball_update (struct ball_t *ball);

enum screen_t
{
	SCREEN_PONG,
	SCREEN_GAME_OVER,
	SCREEN_MENU,
};

enum collision_t
{
	COLLISION_HORIZONTAL_WALL,
	COLLISION_VERTICAL_WALL,
	COLLISION_PLAYER,
	COLLISION_AI,
	COLLISION_NONE,
};

struct particle_t
{
	Vector2 pos;
	Vector2 speed;
	float life;
	bool active;
};

struct game_t
{
	struct Color bg_colour;
	struct paddle_t paddle_player, paddle_ai;
	struct ball_t ball;
	enum screen_t screen;
	struct particle_t particles[MAX_PARTICLES];
};

struct game_t game_init (void);

void game_update_paddle_ai (struct game_t *game);

enum collision_t game_ball_collides_what (struct game_t *game);

void game_game_over_handle_input (struct game_t *game);

void game_game_over_update (struct game_t *game);

void game_game_over_render (struct game_t *game);

void game_pong_handle_input (struct game_t *game);

void game_pong_update (struct game_t *game);

void game_pong_render (struct game_t *game);

void game_spawn_particles (struct game_t *game, Vector2 pos, float direction);

void game_update_particles (struct game_t *game);

void game_menu_handle_input (struct game_t *game);

void game_menu_update (struct game_t *game);

void game_menu_render (struct game_t *game);

void game_handle_input (struct game_t *game);

void game_update (struct game_t *game);

void game_render (struct game_t *game);

/********************/
/* IMPLEMENTATION  */
/********************/

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
ball_init (void)
{
	Vector2 ball_speed = { 0 };

	int sign = (ystar_between (&seed, 0, 2) == 1) ? -1 : 1;

	ball_speed.x = sign * BALL_SPEED;
	ball_speed.y = ball_speed.x * 0.5;

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
	float dt = GetFrameTime ();
	ball->pos.x += ball->speed.x * dt;
	ball->pos.y += ball->speed.y * dt;
}

struct game_t
game_init (void)
{
	return (struct game_t){
		.bg_colour     = COLOUR_BACKGROUND,
		.paddle_player = paddle_init (0),
		.paddle_ai     = paddle_init (WINDOW_WIDTH - PADDLE_SIZE.x),
		.ball          = ball_init (),
		.screen        = SCREEN_MENU,
	};
}

enum collision_t
game_ball_collides_what (struct game_t *game)
{
	struct Rectangle ball_rect = {
		game->ball.pos.x,
		game->ball.pos.y,
		game->ball.size.x,
		game->ball.size.y,
	};

	struct Rectangle ai_rect = {
		game->paddle_ai.pos.x,
		game->paddle_ai.pos.y,
		game->paddle_ai.size.x,
		game->paddle_ai.size.y,
	};

	struct Rectangle player_rect = {
		game->paddle_player.pos.x,
		game->paddle_player.pos.y,
		game->paddle_player.size.x,
		game->paddle_player.size.y,
	};

	if (CheckCollisionRecs (ball_rect, ai_rect))
		{
			return COLLISION_AI;
		}

	if (CheckCollisionRecs (ball_rect, player_rect))
		{
			return COLLISION_PLAYER;
		}

	if (game->ball.pos.y <= 0
	    || (game->ball.pos.y + game->ball.size.y) >= WINDOW_HEIGHT)
		{
			return COLLISION_VERTICAL_WALL;
		}

	if (game->ball.pos.x <= 0
	    || (game->ball.pos.x + game->ball.size.x) >= WINDOW_WIDTH)
		{
			return COLLISION_HORIZONTAL_WALL;
		}

	return COLLISION_NONE;
}

void
game_update_paddle_ai (struct game_t *game)
{
	float dt = GetFrameTime ();

	float ball_center_y   = game->ball.pos.y + (game->ball.size.y / 2);
	float paddle_center_y = game->paddle_ai.pos.y + (game->paddle_ai.size.y / 2);
	float distance        = ball_center_y - paddle_center_y;

	if (fabsf (distance) > game->paddle_ai.speed)
		{
			if (distance > 0)
				{
					game->paddle_ai.pos.y += game->paddle_ai.speed * dt;
				}
			else
				{
					game->paddle_ai.pos.y -= game->paddle_ai.speed * dt;
				}
		}
	else
		{
			game->paddle_ai.pos.y += distance;
		}

	if (game->paddle_ai.pos.y < 0)
		{
			game->paddle_ai.pos.y = 0;
		}
	if (game->paddle_ai.pos.y + game->paddle_ai.size.y > WINDOW_HEIGHT)
		{
			game->paddle_ai.pos.y = WINDOW_HEIGHT - game->paddle_ai.size.y;
		}
}

void
game_pong_handle_input (struct game_t *game)
{
	float dt = GetFrameTime ();

	if (IsKeyDown (KEY_W) || IsKeyDown (KEY_UP))
		{
			game->paddle_player.pos.y -= game->paddle_player.speed * dt;
		}
	if (IsKeyDown (KEY_S) || IsKeyDown (KEY_DOWN))
		{
			game->paddle_player.pos.y += game->paddle_player.speed * dt;
		}

	if (game->paddle_player.pos.y < 0)
		{
			game->paddle_player.pos.y = 0;
		}
	if (game->paddle_player.pos.y + game->paddle_player.size.y > WINDOW_HEIGHT)
		{
			game->paddle_player.pos.y = WINDOW_HEIGHT - game->paddle_player.size.y;
		}
}

void
game_pong_update (struct game_t *game)
{
	ball_update (&game->ball);
	game_update_particles (game);

	enum collision_t collision_entity = game_ball_collides_what (game);

	switch (collision_entity)
		{
		case COLLISION_AI:
		case COLLISION_PLAYER:
			{
				struct paddle_t *paddle = (collision_entity == COLLISION_PLAYER)
				                              ? &game->paddle_player
				                              : &game->paddle_ai;

				if (collision_entity == COLLISION_PLAYER)
					{
						game->ball.pos.x = paddle->pos.x + paddle->size.x;
					}
				else
					{
						game->ball.pos.x = paddle->pos.x - game->ball.size.x;
					}

				float hit_factor = (game->ball.pos.y + (game->ball.size.y / 2))
				                   - (paddle->pos.y + (paddle->size.y / 2));
				float hit_normalised = hit_factor / (paddle->size.y / 2);

				game->ball.speed.y = hit_normalised * BALL_SPEED * 0.8f;
				game->ball.speed.x *= -1.05f;

				float p_dir = (collision_entity == COLLISION_PLAYER) ? 1.0f : -1.0f;
				game_spawn_particles (game, game->ball.pos, p_dir);
			}
			break;

		case COLLISION_HORIZONTAL_WALL:
			game->screen = SCREEN_GAME_OVER;
			break;

		case COLLISION_VERTICAL_WALL:
			if (game->ball.pos.y <= 0)
				{
					game->ball.pos.y = 0;
				}
			else
				{
					game->ball.pos.y = WINDOW_HEIGHT - game->ball.size.y;
				}

			game->ball.speed.y *= -1.0f;
			break;

		case COLLISION_NONE:
			break;
		}

	game_update_paddle_ai (game);
}

void
game_pong_render (struct game_t *game)
{
	ClearBackground (COLOUR_BACKGROUND);
	for (int i = 0; i < MAX_PARTICLES; i++)
		{
			if (game->particles[i].active)
				{
					DrawRectangleV (game->particles[i].pos,
					                (Vector2){ 4, 4 },
					                Fade (COLOUR_BALL, game->particles[i].life));
				}
		}
	paddle_render (&game->paddle_player);
	paddle_render (&game->paddle_ai);
	ball_render (&game->ball);
}

void
game_game_over_handle_input (struct game_t *game)
{
	if (IsKeyPressed (KEY_R))
		{
			game->ball   = ball_init ();
			game->screen = SCREEN_PONG;
		}
}

void
game_game_over_update (struct game_t *game)
{
}

void
game_game_over_render (struct game_t *game)
{
	ClearBackground (COLOUR_BACKGROUND);

	int title_width = MeasureText ("GAME OVER", 80);
	int sub_width   = MeasureText ("Press R to Restart", 20);

	DrawText ("GAME OVER",
	          (WINDOW_WIDTH / 2) - (title_width / 2),
	          (WINDOW_HEIGHT / 2) - 50,
	          80,
	          RED);

	DrawText ("Press R to Restart",
	          (WINDOW_WIDTH / 2) - (sub_width / 2),
	          (WINDOW_HEIGHT / 2) + 40,
	          20,
	          RAYWHITE);
}

void
game_menu_handle_input (struct game_t *game)
{
	if (IsKeyPressed (KEY_Y))
		{
			game->screen = SCREEN_PONG;
		}
}

void
game_menu_update (struct game_t *game)
{
}

void
game_menu_render (struct game_t *game)
{
	ClearBackground (COLOUR_BACKGROUND);

	const char *title = "PONG -- vs-123";
	const char *sub   = "Press Y to begin";

	int title_font_size = 60;
	int sub_font_size   = 20;

	int title_width = MeasureText (title, title_font_size);
	int sub_width   = MeasureText (sub, sub_font_size);

	DrawText (title,
	          (WINDOW_WIDTH / 2) - (title_width / 2),
	          (WINDOW_HEIGHT / 2) - 40,
	          title_font_size,
	          COLOUR_PADDLE);

	DrawText (sub,
	          (WINDOW_WIDTH / 2) - (sub_width / 2),
	          (WINDOW_HEIGHT / 2) + 40,
	          sub_font_size,
	          GREEN);
}

#define SCREEN_LIST(X)                                                         \
	X (SCREEN_MENU, menu)                                                         \
	X (SCREEN_PONG, pong)                                                         \
	X (SCREEN_GAME_OVER, game_over)

void
game_update (struct game_t *game)
{
#define AS_UPDATE_CASE(name, prefix)                                           \
	case name:                                                                    \
		game_##prefix##_update (game);                                               \
		break;
	switch (game->screen)
		{
			SCREEN_LIST (AS_UPDATE_CASE)
		default:
			assert (0 && "INVALID SCREEN");
			break;
		}
#undef AS_UPDATE_CASE
}

void
game_handle_input (struct game_t *game)
{
#define AS_INPUT_CASE(name, prefix)                                            \
	case name:                                                                    \
		game_##prefix##_handle_input (game);                                         \
		break;
	switch (game->screen)
		{
			SCREEN_LIST (AS_INPUT_CASE)
		default:
			assert (0 && "INVALID SCREEN");
			break;
		}
#undef AS_INPUT_CASE
}

void
game_render (struct game_t *game)
{
#define AS_RENDER_CASE(name, prefix)                                           \
	case name:                                                                    \
		game_##prefix##_render (game);                                               \
		break;
	switch (game->screen)
		{
			SCREEN_LIST (AS_RENDER_CASE)
		default:
			assert (0 && "INVALID SCREEN");
			break;
		}
#undef AS_RENDER_CASE
}

void
game_spawn_particles (struct game_t *game, Vector2 position, float direction)
{
	for (int i = 0; i < 42; i++)
		{
			for (int j = 0; j < MAX_PARTICLES; j++)
				{
					if (!game->particles[j].active)
						{
							game->particles[j].active = true;
							game->particles[j].pos    = position;
							game->particles[j].life   = 1.0f;

							float speed_x              = (float)ystar_between (&seed, 50, 400);
							game->particles[j].speed.x = speed_x * direction;
							game->particles[j].speed.y = (float)ystar_between (&seed, -200, 200);
							break;
						}
				}
		}
}

void
game_update_particles (struct game_t *game)
{
	float dt = GetFrameTime ();
	for (int i = 0; i < MAX_PARTICLES; i++)
		{
			if (game->particles[i].active)
				{
					game->particles[i].pos.x += game->particles[i].speed.x * dt;
					game->particles[i].pos.y += game->particles[i].speed.y * dt;
					game->particles[i].life -= 2.0f * dt;

					if (game->particles[i].life <= 0)
						{
							game->particles[i].active = false;
						}
				}
		}
}

int
main (void)
{
	seed               = time (NULL);
	struct game_t game = game_init ();
	game.screen        = SCREEN_PONG;

	InitWindow (WINDOW_WIDTH, WINDOW_HEIGHT, "Pong");
	SetTargetFPS (60);

	while (!WindowShouldClose ())
		{
			game_update (&game);
			game_handle_input (&game);
			BeginDrawing ();
			game_render (&game);
			EndDrawing ();
		}

	return 0;
}
