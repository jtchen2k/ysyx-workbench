/*
 * gpu.cc
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-04-24 15:14:38
 * @modified: 2025-04-28 11:23:23
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#include "SDL_pixels.h"
#include "common.h"
#include "device/map.h"
#include "macro.h"
#include <SDL2/SDL.h>

#define SCREEN_W (MUXDEF(CONFIG_VGA_SIZE_800x600, 800, 400))
#define SCREEN_H (MUXDEF(CONFIG_VGA_SIZE_800x600, 600, 300))

static uint32_t screen_size() { return SCREEN_W * SCREEN_H * sizeof(uint32_t); }

static void       *vmem = nullptr;
static mmio_map_t *vga_mmio_map [[maybe_unused]] = nullptr;

static SDL_Renderer *renderer = NULL;
static SDL_Texture  *texture = NULL;
static SDL_Window   *window = nullptr;

[[maybe_unused]]
static void init_screen() {
    char title[] = "npc";
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(
        SCREEN_W * MUXDEF(CONFIG_VGA_SIZE_800x600, 1, 2),
        SCREEN_H * MUXDEF(CONFIG_VGA_SIZE_800x600, 1, 2), // double size for small res
        0, &window, &renderer);
    SDL_SetWindowTitle(window, title);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC,
                                SCREEN_W, SCREEN_H);
    SDL_RenderPresent(renderer);
}

[[maybe_unused]]
static void init_vmem() {
    vmem = (uint32_t *)malloc(screen_size());
}

void init_gpu() {
    IFDEF(CONFIG_VGA_SHOW_SCREEN, init_screen());
    LogInfo("gpu device initialized");
}