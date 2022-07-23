#include "tetris.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

const char* shapes[7] = {
    // ----
    "    "
    "#   "
    "### "
    "    ",
    // ----
    "    "
    "##  "
    " ## "
    "    ",
    // ----
    "    "
    " ## "
    "##  "
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
    "  # "
    "### "
    "    ",
    // ----
};
const uint32_t* color_hex[7] = {
    0x0000ff,
    0xff0000,
    0x00ff00,
    0x800080,
    0x00ffff,
    0xffff00,
    0xff7f00,
};

void draw_shape(Piece field[][10], PieceState piece_state)
{
    for (uint8_t i = 0; i < 4; i++) {
        for (uint8_t j = 0; j < 4; j++) {
            int rotations[] = {
                i * 4 + j,
                (4 - j - 1) * 4 + i,
                (4 - i - 1) * 4 + (4 - j - 1),
                j * 4 + (4 - i - 1),
            };
            if (shapes[piece_state.color - 1][rotations[piece_state.rotation]] != ' ') {
                field[piece_state.y + i][piece_state.x + j] = piece_state.color;
            }
        }
    }
}
void clear_shape(Piece field[][10], PieceState piece_state)
{
    for (uint8_t i = 0; i < 4; i++) {
        for (uint8_t j = 0; j < 4; j++) {
            int rotations[] = {
                i * 4 + j,
                (4 - j - 1) * 4 + i,
                (4 - i - 1) * 4 + (4 - j - 1),
                j * 4 + (4 - i - 1),
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
                draw_rect(game_state->app, j + field_padding_x, i + field_padding_y - 4, color_hex[game_state->field[i][j] - 1]);
            } else {
                draw_rect(game_state->app, j + field_padding_x, i + field_padding_y - 4, 0x7f7f7f);
            }
        }
    }
    for (uint8_t i = 0; i < 4; i++) {
        for (uint8_t j = 0; j < 4; j++) {
            int rotations[] = {
                i * 4 + j,
                (4 - j - 1) * 4 + i,
                (4 - i - 1) * 4 + (4 - j - 1),
                j * 4 + (4 - i - 1),
            };
            if (shapes[game_state->piece.color - 1][rotations[game_state->piece.rotation]] != ' ') {
                draw_rect(game_state->app, game_state->piece.x + j + field_padding_x, game_state->piece.y + i + field_padding_y - 4, color_hex[game_state->piece.color - 1]);
            }
        }
    }
    for (uint8_t i = 0; i < 20; i++) {
        draw_rect(game_state->app, field_padding_x - 1, i + field_padding_y, 0x905099);
        draw_rect(game_state->app, field_padding_x + 10, i + field_padding_y, 0x905099);
    }
    for (uint8_t j = 0; j < 12; j++) {
        draw_rect(game_state->app, j + field_padding_x - 1, field_padding_y - 1, 0x905099);
        draw_rect(game_state->app, j + field_padding_x - 1, field_padding_y + 20, 0x905099);
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
    int result = true;
    for (uint8_t i = 0; i < 4; i++) {
        for (uint8_t j = 0; j < 4; j++) {
            int rotations[] = {
                i * 4 + j,
                (4 - j - 1) * 4 + i,
                (4 - i - 1) * 4 + (4 - j - 1),
                j * 4 + (4 - i - 1),
            };
            if (shapes[piece_state.color - 1][rotations[(piece_state.rotation + delta_rotation) % 4]] != ' ') {
                if (piece_state.y + i + delta_y >= 24 || piece_state.y + i + delta_y < 0 || piece_state.x + j + delta_x < 0 || piece_state.x + j + delta_x >= 10 || game_state->field[piece_state.y + i + delta_y][piece_state.x + j + delta_x] != 0) {
                    result = false;
                    return result;
                }
            }
        }
    }

    return result;
}

bool can_go_down(GameState* game_state, PieceState piece_state)
{
    int result = true;
    for (uint8_t i = 0; i < 4; i++) {
        for (uint8_t j = 0; j < 4; j++) {
            int rotations[] = {
                i * 4 + j,
                (4 - j - 1) * 4 + i,
                (4 - i - 1) * 4 + (4 - j - 1),
                j * 4 + (4 - i - 1),
            };
            if (shapes[piece_state.color - 1][rotations[piece_state.rotation]] != ' ') {
                if (piece_state.y + i + 1 >= 24 || game_state->field[piece_state.y + i + 1][piece_state.x + j] != 0) {
                    result = false;
                    return result;
                }
            }
        }
    }

    return result;
}
void spawn_shape(GameState* game_state, Color shape)
{
    PieceState new_piece = { 5, 0, 0, shape };
    game_state->piece = new_piece;
}

uint32_t push_down_timer_handler(uint32_t interval, void* game_state_ptr)
{
    GameState* game_state = game_state_ptr;
    if (can_go_down(game_state, game_state->piece)) {
        game_state->piece.y += 1;
        return 500;
    } else {
        draw_shape(game_state->field, game_state->piece);
        spawn_shape(game_state, rand() % 7 + 1);
        return 500;
    }
}

int main(int argc, char const* argv[])
{
    GameState game_state = { { 0 }, init_handler(), { 0, 0, 0, O } };
    int x = 0;
    spawn_shape(&game_state, rand() % 7 + 1);
    SDL_AddTimer(250, &push_down_timer_handler, (void*)&game_state);
    while (1) {
        event_handler(&game_state);
        check_field(&game_state);
        render_field(&game_state);
    }

    return 0;
}
