/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <array>
#include <random>
#include <chrono>
#include <thread>

#include "SDL.h"

#include "potato_font.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

extern std::array<uint8_t,0x1000> mem;
extern std::array<uint8_t, 0x10>V;
extern std::array<uint8_t, 0x40 * 0x30> display;
extern std::array<uint8_t, 0x10>keys;

extern uint8_t delay_timer;
extern uint8_t sound_timer;

extern uint16_t op;
extern uint8_t sp;
extern uint16_t idx;
extern uint16_t pc;

extern std::array<uint8_t, 0x10>stack;
extern uint8_t stack_ptr;

extern std::mt19937 mt_rand;
extern bool display_changed;
