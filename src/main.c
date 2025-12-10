#include <stdio.h>

#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>

#define WINDOW_TITLE "miv"
#define MAX_WINDOW_WIDTH 1280
#define MAX_WINDOW_HEIGHT 720

static SDL_Window *p_window = NULL;
static SDL_Renderer *p_renderer = NULL;
static SDL_Texture *p_texture = NULL;
static float zoom = 1.0f;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: miv <path>\n");
        return SDL_APP_FAILURE;
    }

    SDL_Surface *p_surface = IMG_Load(argv[1]);
    if (!p_surface) {
        SDL_Log("Couldn't load image: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    };

    int image_width = p_surface->w;
    int image_height = p_surface->h;

    float scale_factor = 1.0f;

    if (image_width > MAX_WINDOW_WIDTH || image_height > MAX_WINDOW_HEIGHT) {
        float scale_factor_x = (float)MAX_WINDOW_WIDTH / image_width;
        float scale_factor_y = (float)MAX_WINDOW_HEIGHT / image_height;
        scale_factor = SDL_min(scale_factor_x, scale_factor_y);
    }

    int window_width = (int)(image_width * scale_factor);
    int window_height = (int)(image_height * scale_factor);


    SDL_WindowFlags window_flags = SDL_WINDOW_HIGH_PIXEL_DENSITY;
    if (!SDL_CreateWindowAndRenderer(
        WINDOW_TITLE, window_width, window_height, window_flags, &p_window, &p_renderer
    )) {
        SDL_Log("Couldn't create window and renderer: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    p_texture = SDL_CreateTextureFromSurface(p_renderer, p_surface);
    if (!p_texture) {
        SDL_Log("Couldn't create texture from surface: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_DestroySurface(p_surface);
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }

    if(event->type == SDL_EVENT_KEY_DOWN) {
        switch (event->key.scancode) {
        case SDL_SCANCODE_ESCAPE:
            return SDL_APP_SUCCESS;
            break;

        case SDL_SCANCODE_RIGHTBRACKET:
            zoom += 0.1f;
            SDL_SetRenderScale(p_renderer, zoom, zoom);
            break;

        case SDL_SCANCODE_LEFTBRACKET:
            zoom = SDL_max(zoom - 0.1f, 1.0f);
            SDL_SetRenderScale(p_renderer, zoom, zoom);
            break;

        default:
            break;
        }
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    int output_width, output_height;
    SDL_GetRenderOutputSize(p_renderer, &output_width, &output_height);
    float offset_x = (output_width * (1.0f - zoom)) / (2.0f * zoom);
    float offset_y = (output_height * (1.0f - zoom)) / (2.0f * zoom);

    SDL_FRect destination = {
        offset_x,
        offset_y,
        output_width,
        output_height
    };

    SDL_RenderClear(p_renderer);
    SDL_RenderTexture(p_renderer, p_texture, NULL, &destination);
    SDL_RenderPresent(p_renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    SDL_DestroyTexture(p_texture);
    SDL_DestroyRenderer(p_renderer);
    SDL_DestroyWindow(p_window);
}
