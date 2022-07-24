#include "tetris.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SHAPE_FIELD_SIZE 4
#define INITIAL_SPEED 500
#define MAXIMUM_SPEED 100
#define OFFSET_COUNTS 5

const Color initial_shape_list[7] = {
    L, O, J, S, I, T, Z
};

const char* shapes[7]
    = {
          // ----
          "    "
          "  # "
          "### "
          "    ",
          // ----
          "    "
          " ## "
          "##  "
          "    ",
          // ----
          "    "
          "##  "
          " ## "
          "    ",
          // ----
          "    "
          " #  "
          "### "
          "    ",
          // ----
          "    "
          "####"
          "    "
          "    ",
          // ----
          "    "
          " ## "
          " ## "
          "    ",
          // ----
          "    "
          "#   "
          "### "
          "    ",
          // ----
      };
static void shuffle(void* array, size_t n, size_t size)
{
    char tmp[size];
    char* arr = array;
    size_t stride = size * sizeof(char);

    if (n > 1) {
        size_t i;
        for (i = 0; i < n - 1; ++i) {
            size_t rnd = (size_t)rand();
            size_t j = i + rnd / (RAND_MAX / (n - i) + 1);

            memcpy(tmp, arr + j * stride, size);
            memcpy(arr + j * stride, arr + i * stride, size);
            memcpy(arr + i * stride, tmp, size);
        }
    }
}
const uint8_t shape_rotate_offsets[7][5][4][2] = {
    {
        { { 0, 1 }, { -1, 0 }, { 0, -1 }, { 1, 0 } },
        { { 0, 1 }, { 0, 0 }, { 0, -1 }, { 0, 0 } },
        { { 0, 1 }, { 0, 1 }, { 0, -1 }, { 0, 1 } },
        { { 0, 1 }, { -1, -2 }, { 0, -1 }, { 1, -2 } },
        { { 0, 1 }, { 0, -2 }, { 0, -1 }, { 0, -2 } },
    },
    {
        { { 0, 1 }, { -1, 0 }, { 0, -1 }, { 1, 0 } },
        { { 0, 1 }, { 0, 0 }, { 0, -1 }, { 0, 0 } },
        { { 0, 1 }, { 0, 1 }, { 0, -1 }, { 0, 1 } },
        { { 0, 1 }, { -1, -2 }, { 0, -1 }, { 1, -2 } },
        { { 0, 1 }, { 0, -2 }, { 0, -1 }, { 0, -2 } },
    },
    {
        { { 0, 1 }, { -1, 0 }, { 0, -1 }, { 1, 0 } },
        { { 0, 1 }, { 0, 0 }, { 0, -1 }, { 0, 0 } },
        { { 0, 1 }, { 0, 1 }, { 0, -1 }, { 0, 1 } },
        { { 0, 1 }, { -1, -2 }, { 0, -1 }, { 1, -2 } },
        { { 0, 1 }, { 0, -2 }, { 0, -1 }, { 0, -2 } },
    },
    {
        { { 0, 1 }, { -1, 0 }, { 0, -1 }, { 1, 0 } },
        { { 0, 1 }, { 0, 0 }, { 0, -1 }, { 0, 0 } },
        { { 0, 1 }, { 0, 1 }, { 0, -1 }, { 0, 1 } },
        { { 0, 1 }, { -1, -2 }, { 0, -1 }, { 1, -2 } },
        { { 0, 1 }, { 0, -2 }, { 0, -1 }, { 0, -2 } },
    },
    {
        { { -1, 0 }, { 0, -1 }, { 1, 0 }, { 0, 1 } },
        { { -2, 0 }, { 1, -1 }, { 3, 0 }, { 0, 1 } },
        { { 1, 0 }, { 1, -1 }, { 0, 0 }, { 0, 1 } },
        { { -2, 0 }, { 1, -2 }, { 3, 1 }, { 0, 3 } },
        { { 1, 0 }, { 1, 1 }, { 0, 1 }, { 0, 0 } },
    },
    { 0 },
    {
        { { 0, 1 }, { -1, 0 }, { 0, -1 }, { 1, 0 } },
        { { 0, 1 }, { 0, 0 }, { 0, -1 }, { 0, 0 } },
        { { 0, 1 }, { 0, 1 }, { 0, -1 }, { 0, 1 } },
        { { 0, 1 }, { -1, -2 }, { 0, -1 }, { 1, -2 } },
        { { 0, 1 }, { 0, -2 }, { 0, -1 }, { 0, -2 } },
    },
};
const uint32_t* color_hex[7] = {
    0xff7f00,
    0x00ff00,
    0xff0000,
    0x800080,
    0x00ffff,
    0xffff00,
    0x0000ff,
};

void draw_shape(Piece field[][10], PieceState piece_state)
{
    for (uint8_t i = 0; i < SHAPE_FIELD_SIZE; i++) {
        for (uint8_t j = 0; j < SHAPE_FIELD_SIZE; j++) {
            int rotations[] = {
                i * SHAPE_FIELD_SIZE + j,
                (SHAPE_FIELD_SIZE - j - 1) * SHAPE_FIELD_SIZE + i,
                (SHAPE_FIELD_SIZE - i - 1) * SHAPE_FIELD_SIZE + (SHAPE_FIELD_SIZE - j - 1),
                j * SHAPE_FIELD_SIZE + (SHAPE_FIELD_SIZE - i - 1),
            };
            if (shapes[piece_state.color - 1][rotations[piece_state.rotation]] != ' ') {
                field[piece_state.y + i][piece_state.x + j] = piece_state.color;
            }
        }
    }
}
void clear_shape(Piece field[][10], PieceState piece_state)
{
    for (uint8_t i = 0; i < SHAPE_FIELD_SIZE; i++) {
        for (uint8_t j = 0; j < SHAPE_FIELD_SIZE; j++) {
            int rotations[] = {
                i * SHAPE_FIELD_SIZE + j,
                (SHAPE_FIELD_SIZE - j - 1) * SHAPE_FIELD_SIZE + i,
                (SHAPE_FIELD_SIZE - i - 1) * SHAPE_FIELD_SIZE + (SHAPE_FIELD_SIZE - j - 1),
                j * SHAPE_FIELD_SIZE + (SHAPE_FIELD_SIZE - i - 1),
            };

            if (shapes[piece_state.color - 1][rotations[piece_state.rotation]] != ' ') {
                field[piece_state.y + i][piece_state.x + j] = 0;
            }
        }
    }
}

void render_field(GameState* game_state)
{
    uint32_t field_padding_x
        = 1;
    uint32_t field_padding_y = 1;
    for (uint8_t i = 4; i < 24; i++) {
        for (uint8_t j = 0; j < 10; j++) {
            if (game_state->field[i][j] != 0) {
                draw_rect(game_state->app, j + field_padding_x, i + field_padding_y - 4, color_hex[game_state->field[i][j] - 1], false);
            } else {
                draw_rect(game_state->app, j + field_padding_x, i + field_padding_y - 4, 0x101010, true);
            }
        }
    }
    for (uint8_t i = 0; i < SHAPE_FIELD_SIZE; i++) {
        for (uint8_t j = 0; j < SHAPE_FIELD_SIZE; j++) {
            int rotations[] = {
                i * SHAPE_FIELD_SIZE + j,
                (SHAPE_FIELD_SIZE - j - 1) * SHAPE_FIELD_SIZE + i,
                (SHAPE_FIELD_SIZE - i - 1) * SHAPE_FIELD_SIZE + (SHAPE_FIELD_SIZE - j - 1),
                j * SHAPE_FIELD_SIZE + (SHAPE_FIELD_SIZE - i - 1),
            };
            if (shapes[game_state->piece.color - 1][rotations[game_state->piece.rotation]] != ' ') {
                draw_rect(game_state->app, game_state->piece.x + j + field_padding_x, game_state->piece.y + i + field_padding_y - 4, color_hex[game_state->piece.color - 1], false);
            }
        }
    }
    for (uint8_t i = 0; i < 20; i++) {
        draw_rect(game_state->app, field_padding_x - 1, i + field_padding_y, 0x505050, true);
        draw_rect(game_state->app, field_padding_x + 10, i + field_padding_y, 0x505050, true);
    }
    for (uint8_t j = 0; j < 12; j++) {
        draw_rect(game_state->app, j + field_padding_x - 1, field_padding_y - 1, 0x505050, true);
        draw_rect(game_state->app, j + field_padding_x - 1, field_padding_y + 20, 0x505050, true);
    }
    SDL_RenderPresent(game_state->app.renderer);
}
bool is_row_full(Piece row[10])
{
    for (uint8_t i = 0; i < 10; i++) {
        if (row[i] == 0) {
            return false;
        }
    }
    return true;
}
void delete_row(GameState* game_state, uint8_t row_index)
{
    for (uint8_t i = 0; i < 10; i++) {
        game_state->field[row_index][i] = 0;
    }
    for (uint8_t i = row_index; i != 1; i--) {
        memcpy(game_state->field[i], game_state->field[i - 1], sizeof(Piece) * 10);
    }
}
void check_field(GameState* game_state)
{
    for (uint8_t i = 23; i != 0; i--) {
        if (is_row_full(game_state->field[i])) {
            delete_row(game_state, i);
        }
    }
}

bool can_go_to(GameState* game_state, PieceState piece_state, int8_t delta_x, int8_t delta_y, int8_t delta_rotation)
{
    for (uint8_t i = 0; i < SHAPE_FIELD_SIZE; i++) {
        for (uint8_t j = 0; j < SHAPE_FIELD_SIZE; j++) {
            int rotations[] = {
                i * SHAPE_FIELD_SIZE + j,
                (SHAPE_FIELD_SIZE - j - 1) * SHAPE_FIELD_SIZE + i,
                (SHAPE_FIELD_SIZE - i - 1) * SHAPE_FIELD_SIZE + (SHAPE_FIELD_SIZE - j - 1),
                j * SHAPE_FIELD_SIZE + (SHAPE_FIELD_SIZE - i - 1),
            };
            if (shapes[piece_state.color - 1][rotations[(piece_state.rotation + delta_rotation) % 4]] != ' ') {
                if (piece_state.y + i + delta_y >= 24 || piece_state.y + i + delta_y < 0 || piece_state.x + j + delta_x < 0 || piece_state.x + j + delta_x >= 10 || game_state->field[piece_state.y + i + delta_y][piece_state.x + j + delta_x] != 0) {
                    return false;
                }
            }
        }
    }
    return true;
}

void spawn_shape(GameState* game_state)
{
    if (game_state->piece_index >= 14) {
        memcpy(game_state->piece_list, initial_shape_list, sizeof(Color) * 7);
        memcpy(&game_state->piece_list[7], initial_shape_list, sizeof(Color) * 7);
        shuffle(game_state->piece_list, 14, sizeof(Color));
    }
    game_state->piece_index = (game_state->piece_index + 1) % 14;
    PieceState new_piece = { 5, 0, 0, game_state->piece_list[game_state->piece_index] };
    if (!can_go_to(game_state, new_piece, 0, 0, 0)) {
        game_state->is_game_over = true;
    } else {
        game_state->piece = new_piece;
    }
}

void rotate_shape(GameState* game_state)
{
    for (uint8_t i = 0; i < OFFSET_COUNTS; i++) {
        if (
            can_go_to(game_state, game_state->piece,
                shape_rotate_offsets[game_state->piece.color - 1][i][game_state->piece.rotation][0],
                shape_rotate_offsets[game_state->piece.color - 1][i][game_state->piece.rotation][1],
                1)) {
            game_state->piece.x += shape_rotate_offsets[game_state->piece.color - 1][i][game_state->piece.rotation][0];
            game_state->piece.y += shape_rotate_offsets[game_state->piece.color - 1][i][game_state->piece.rotation][1];
            game_state->piece.rotation = (game_state->piece.rotation + 1) % 4;
            break;
        }
    }
}

uint32_t push_down_timer_handler(uint32_t interval, void* game_state_ptr)
{
    GameState* game_state = game_state_ptr;
    if (game_state->is_game_over) {
        puts("game over!");
        return 0;
    }
    if (can_go_to(game_state, game_state->piece, 0, 1, 0)) {
        game_state->piece.y += 1;
    } else {
        draw_shape(game_state->field, game_state->piece);
        spawn_shape(game_state);
    }
    if (interval > MAXIMUM_SPEED) {
        --interval;
    }
    return interval;
}

int main(int argc, char const* argv[])
{
    srand(time(0));
    GameState game_state = { { 0 }, init_handler(), { 0, 0, 0, 0 }, { 0 }, -1 };
    spawn_shape(&game_state);
    SDL_AddTimer(INITIAL_SPEED, &push_down_timer_handler, (void*)&game_state);
    while (1) {
        event_handler(&game_state);
        check_field(&game_state);
        render_field(&game_state);
    }

    return 0;
}
