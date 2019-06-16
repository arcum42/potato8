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

#include "potato8.h"
#include "potato_ui.h"
#include "potato_ops.h"

// 0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
// 0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
// 0x200-0xFFF - Program ROM and work RAM

std::array<uint8_t, 0x1000> mem;
std::array<uint8_t, 0x10> V = {0};
std::array<uint8_t, 0x40 * 0x30> display = {0};
std::array<uint8_t, 0x10> keys = {0};
std::array<uint8_t, 0x10> stack = {0};

op_field op = 0;
uint8_t sp = 0;
uint16_t idx = 0;
uint16_t pc = 0;

uint8_t delay_timer = 0;
uint8_t sound_timer = 0;

std::mt19937 mt_rand;

bool display_changed = true;

auto potato_logger = spdlog::basic_logger_mt("potato-log", "logs/potato.txt");
auto console = spdlog::stdout_color_mt("potato"); 

bool load_file(const char *filename)
{
	std::ifstream f;
	char *buffer;

    spdlog::get("potato")->debug("Opening file {}.", filename);
	f.open(filename, std::ios::binary | std::ios::ate);

	if (f.is_open())
	{
        spdlog::get("potato")->debug("Open!\n");

		std::streamsize size = f.tellg();
        f.seekg(0, std::ios::beg);

		buffer = new char[size];
		f.read(buffer, size);
		f.close();

		for (int i = 0; i < size; i++)
		{
			mem[0x200 + i] = buffer[i];
		}
		return true;
	}

	return false;
}

void random_gfx()
{
    for(int i= 0; i < 0x40 * 0x30; i++)
    {
        display[i] = mt_rand() % 2;
    }
}

bool main_init(const char* filename)
{
	bool ret = true;
	auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();

	console->debug("Initialize.");

	std::mt19937 mt_rand(seed);

	pc = 0x200;
	op = 0;
	idx = 0;
	sp = 0;

	ret = load_file(filename);

	for (int i = 0; i < 80; i++)
	{
		mem[i] = potato_font[i];
	}

	potato_chip::init_ops();
    //random_gfx();

	if (ret) ret = init_potato_ui();

	return ret;
}

void main_loop()
{
	bool quit = false;

	spdlog::get("potato")->debug("Enter main loop.");
	while (!quit)
	{
		op = mem[pc] << 8 | mem[pc + 1];

		potato_chip::handle_op();
		pc += 2;

		if (delay_timer > 0) delay_timer--;
		if (sound_timer > 0)
		{
			sound_timer--;
			spdlog::get("potato")->debug("Beep!");
		}

		if (display_changed)
		{
			potato_ui_update();
			display_changed = false;
		}

		quit = potato_events();
        if (pc > 0x1000)
        {
            spdlog::get("potato")->debug("Out of bounds! Closing.");
            quit = true;
        }
        //std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

int main(int argc, char *args[])
{
	spdlog::set_level(spdlog::level::debug);

    if (argc > 1) 
    {
        if (!main_init(args[1])) return 1;
    }
    else
    {
        if (!main_init("maze.ch8")) return 1;
    }
    
	

	main_loop();

	potato_ui_cleanup();

	spdlog::get("potato")->debug("Exit.");

	return 0;
}
