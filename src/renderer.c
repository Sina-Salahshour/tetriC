

#define SCREEN_WIDTH 350
#define SCREEN_HEIGHT 600

#define BLOCK_SIZE 25

#include "tetris.h"

App init_handler()
{
    App app;
    int rendererFlags, windowFlags;

    rendererFlags = SDL_RENDERER_ACCELERATED;

    windowFlags = 0;

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
    }
    app.window = SDL_CreateWindow("p01",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    app.renderer = SDL_CreateRenderer(app.window, -1, rendererFlags);
    SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 255);
    SDL_RenderClear(app.renderer);
    return app;
}

void event_handler(GameState* game_state)
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            SDL_Quit();
            exit(0);
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_w:
                if (can_go_to(game_state, game_state->piece, 0, 0, 1)) {
                    game_state->piece.rotation = (game_state->piece.rotation + 1) % 4;
                }
                break;
            case SDLK_a:
                if (can_go_to(game_state, game_state->piece, -1, 0, 0)) {
                    game_state->piece.x--;
                }
                break;
            case SDLK_d:
                if (can_go_to(game_state, game_state->piece, 1, 0, 0)) {
                    game_state->piece.x++;
                }
                break;
            case SDLK_s:
                if (can_go_to(game_state, game_state->piece, 0, 1, 0)) {
                    game_state->piece.y++;
                }
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }
}

void draw_rect(App app, u_int8_t x, u_int8_t y, uint32_t color)
{
    const padding_x = BLOCK_SIZE;
    const padding_y = BLOCK_SIZE;
    SDL_Rect r;
    r.x = padding_x + x * BLOCK_SIZE;
    r.y = padding_y + y * BLOCK_SIZE;
    r.w = BLOCK_SIZE;
    r.h = BLOCK_SIZE;
    static SDL_Texture* texture = NULL;
    if (texture == NULL) {

        texture = SDL_CreateTexture(app.renderer,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING,
            BLOCK_SIZE, BLOCK_SIZE);
    }

    static SDL_Surface* surface = NULL;
    if (surface == NULL) {

        surface = SDL_CreateRGBSurfaceFrom(NULL,
            BLOCK_SIZE, BLOCK_SIZE,
            32, 0,
            0x00FF0000,
            0x0000FF00,
            0x000000FF,
            0xFF000000);
    }
    SDL_LockTexture(texture, NULL,
        &surface->pixels,
        &surface->pitch);
    for (u_int32_t i = 0; i < BLOCK_SIZE; i++) {
        for (u_int32_t j = 0; j < BLOCK_SIZE; j++) {
            if (i == 0 || j == 0 || i == BLOCK_SIZE - 1 || j == BLOCK_SIZE - 1) {
                ((uint32_t*)(surface->pixels))[i * BLOCK_SIZE + j] = 0x000000;

            } else {
                ((uint32_t*)(surface->pixels))[i * BLOCK_SIZE + j] = color;
            }
        }
    }

    SDL_UnlockTexture(texture);
    SDL_RenderCopy(app.renderer, texture, NULL, &r);
}
