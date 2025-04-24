/*
 * keyboard.cc
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-04-24 14:46:16
 * @modified: 2025-04-24 16:06:33
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#include "core.h"
#include "device/map.h"
#include <SDL2/SDL.h>

#define KEYDOWN_MASK 0x8000

static mmio_map_t *keyboard_mmio_map;
static uint32_t   *i8042_space;

/// sdl keycode to am scancode
static uint32_t sdl_keymap[512] = {};

// clang-format off
#define NPC_KEYS(f) \
  f(ESCAPE) f(F1) f(F2) f(F3) f(F4) f(F5) f(F6) f(F7) f(F8) f(F9) f(F10) f(F11) f(F12) \
  f(GRAVE) f(1) f(2) f(3) f(4) f(5) f(6) f(7) f(8) f(9) f(0) f(MINUS) f(EQUALS) f(BACKSPACE) \
  f(TAB) f(Q) f(W) f(E) f(R) f(T) f(Y) f(U) f(I) f(O) f(P) f(LEFTBRACKET) f(RIGHTBRACKET) f(BACKSLASH) \
  f(CAPSLOCK) f(A) f(S) f(D) f(F) f(G) f(H) f(J) f(K) f(L) f(SEMICOLON) f(APOSTROPHE) f(RETURN) \
  f(LSHIFT) f(Z) f(X) f(C) f(V) f(B) f(N) f(M) f(COMMA) f(PERIOD) f(SLASH) f(RSHIFT) \
  f(LCTRL) f(APPLICATION) f(LALT) f(SPACE) f(RALT) f(RCTRL) \
  f(UP) f(DOWN) f(LEFT) f(RIGHT) f(INSERT) f(DELETE) f(HOME) f(END) f(PAGEUP) f(PAGEDOWN)
// clang-format on

#define NPC_KEYS_NAME(x) NPC_KEY_##x,
#define SDL_KEYMAP(k) sdl_keymap[SDL_SCANCODE_##k] = NPC_KEY_##k;

enum npc_key { NPC_KEY_NONE = 0, MAP(NPC_KEYS, NPC_KEYS_NAME) };

static void init_sdl_keymap() { MAP(NPC_KEYS, SDL_KEYMAP); }

#define KEY_QUEUE_LEN 1024
static uint32_t key_queue[KEY_QUEUE_LEN] = {};
// front and rear
static uint16_t key_f = 0, key_r = 0;

static void key_enqueue(uint32_t am_scancode) {
    key_queue[key_r] = am_scancode;
    key_r = (key_r + 1) % KEY_QUEUE_LEN;
    Assert(key_r != key_f, "key queue overflow. max length: %d", KEY_QUEUE_LEN);
}

static uint32_t key_dequeue() {
    uint32_t key = NPC_KEY_NONE;
    if (key_f != key_r) {
        key = key_queue[key_f];
        key_f = (key_f + 1) % KEY_QUEUE_LEN;
    }
    return key;
}

void send_key(SDL_Scancode scancode, bool is_down) {
    LogTrace("send_key: %d, %d", scancode, is_down);
    if (g_core_context->state == CORE_STATE_RUNNING && sdl_keymap[scancode] != NPC_KEY_NONE) {
        uint32_t am_scancode = sdl_keymap[scancode] | (is_down ? KEYDOWN_MASK : 0);
        key_enqueue(am_scancode);
    }
}

void i8042_io_handler(uint32_t offset, int len, bool is_write) {
    LogTrace("i8042_io_handler: %d, %d, %d", offset, len, is_write);
    Assert(offset == 0, "invalid offset: %d", offset);
    Assert(len == 4, "invalid len: %d", len);
    if (!is_write) {
        i8042_space[0] = key_dequeue();
    }
}

void init_keyboard() {
    keyboard_mmio_map = add_mmio_map("keyboard", CONFIG_I8042_DATA_MMIO, /* paddr */
                                     8, i8042_io_handler);
    i8042_space = (uint32_t *)keyboard_mmio_map->space;
    init_sdl_keymap();
    LogInfo("keyboard device initialized");
}
