#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#define true 1
#define false 0
typedef uint8_t bool;
typedef struct {
    SDL_Renderer* renderer;
    SDL_Window* window;
} App;
typedef uint8_t Piece;

typedef enum {
    L = 1,
    S,
    Z,
    T,
    I,
    O,
    J
} Color;

typedef struct piece_state {
    int8_t x;
    int8_t y;
    uint8_t rotation;
    Color color;
} PieceState;

typedef struct game_state {
    Piece field[24][10];
    App app;
    PieceState piece;
    Color piece_list[14];
    uint8_t piece_index;
    bool is_game_over;
} GameState;
App init_handler();
void draw_rect(App app, u_int8_t x, u_int8_t y, uint32_t color, bool fancy);
void event_handler(GameState* game_state);
void draw_shape(Piece field[][10], PieceState piece_state);
void clear_shape(Piece field[][10], PieceState piece_state);
void render_field(GameState* game_state);
bool can_go_to(GameState* game_state, PieceState piece_state, int8_t delta_x, int8_t delta_y, int8_t delta_rotation);
void spawn_shape(GameState* game_state);
void push_shape_down(GameState* game_state);
void update_shape(GameState* game_state);
void rotate_shape(GameState* game_state);
