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

uint16_t op = 0;
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

    //random_gfx();

	if (ret) ret = init_potato_ui();

	return ret;
}

void handle_op()
{
	uint8_t x = (op & 0x0f00) >> 8;
	uint8_t y = (op & 0x00f0) >> 4;
	uint16_t nnn = (op & 0x0fff);
	uint8_t kk = (op & 0x00ff);
	uint8_t n = (op & 0x000F);

	spdlog::get("potato")->debug("opcode = 0x{:X}, x:0x{:X}, y:0x{:X}, nnn:0x{:X}, kk:{:X}, n:0x{:X}", op, x, y, nnn, kk, n);
    /*spdlog::get("potato")->debug("[0x{:X}] op: 0x{:X} x: 0x{:X} y: 0x{;X} nnn: 0x{:X} kk: 0x{:X} ", pc, op, x, y, nnn, kk);
    for(int i = 0; i < 0xF; i++)
    {
        spdlog::get("potato")->debug("v[0x{:X}]=0x{:X} ", i, V[i]);
    }*/

	switch (op & 0xf000)
	{
		case 0x0000:
			switch (op & 0xff)
			{
				case 0x00: potato_chip::sys(nnn); break;
				case 0xE0: potato_chip::cls(); break;
				case 0xEE: potato_chip::rts(); break;
				default: potato_chip::unknown(op); break;
			}
			break;

		case 0x1000: potato_chip::jump(nnn); break;
		case 0x2000: potato_chip::call(nnn); break;
		case 0x3000: potato_chip::ske(x, kk); break;
		case 0x4000: potato_chip::skne(x, kk); break;
		case 0x5000: potato_chip::skre(x, y); break;

		case 0x6000: potato_chip::load(x, kk); break;
		case 0x7000: potato_chip::math_add(x, kk); break;

		case 0x8000:
		{
			switch (op & 0xf)
			{
				case 0x0: potato_chip::math_move(x, y); break;
				case 0x1: potato_chip::math_or(x, y); break;
				case 0x2: potato_chip::math_and(x, y); break;
				case 0x3: potato_chip::math_xor(x, y); break;
				case 0x4: potato_chip::math_add_r(x, y); break;
				case 0x5: potato_chip::math_sub_r(x, y); break;
				case 0x6: potato_chip::math_shr(x); break;
				case 0x7: potato_chip::math_subn_r(x, y); break;
				case 0xE: potato_chip::math_shr(x); break;
				default:  potato_chip::unknown(op); break;
			}
		}
		break;

		case 0x9000: potato_chip::skrne(x,y); break;
		case 0xA000: potato_chip::loadi(nnn); break;
		case 0xB000: potato_chip::jumpi(nnn); break;
		case 0xC000: potato_chip::rand(x, kk); break;
		case 0xD000: potato_chip::draw(x, y, n); break;

		case 0xE000:
		{
			switch (op & 0xff)
			{
				case 0x9E: potato_chip::skpr(x); break;
				case 0xA1: potato_chip::sknp(x); break;
				default: potato_chip::unknown(op); break;
			}
		}
		break;

		case 0xF000:
		{
			switch (op & 0xff)
			{
				case 0x07: potato_chip::moved(x); break;
				case 0xA: potato_chip::keyd(x); break;
				case 0x15: potato_chip::loadd(x); break;
				case 0x18: potato_chip::loadd(x); break;
				case 0x1E: potato_chip::addi(x); break;
				case 0x29: potato_chip::ldspr(x); break;
				case 0x33: potato_chip::bcd(x); break;
				case 0x55: potato_chip::stor(x); break;
				case 0x65: potato_chip::read(x); break;

				default: potato_chip::unknown(op); break;
			}
		}

		default: potato_chip::unknown(op); break;
	}
}

void main_loop()
{
	bool quit = false;

	spdlog::get("potato")->debug("Enter main loop.");
	while (!quit)
	{
		op = mem[pc] << 8 | mem[pc + 1];

		handle_op();
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
        std::this_thread::sleep_for(std::chrono::seconds(1));
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
