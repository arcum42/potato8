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
#include "potato_ops.h"

std::string print_regs()
{
    return std::string(fmt::format("V=(0x{:X} 0x{:X} 0x{:X} 0x{:X} 0x{:X} 0x{:X} 0x{:X} 0x{:X} 0x{:X} 0x{:X} 0x{:X} 0x{:X} 0x{:X} 0x{:X} 0x{:X} 0x{:X});", \
    V[0], V[1], V[2], V[3], V[4], V[5], V[6], V[7], V[8], V[9], V[0xA], V[0xB], V[0xC], V[0xD], V[0xE], V[0xF]));
}

void print_op(const char *str)
{
	spdlog::get("potato")->debug("pc[0x{:X}] {}: {} I=0x{:X}", pc, str, print_regs(), idx);
}

void print_op(std::string str)
{
	spdlog::get("potato")->debug("pc[0x{:X}] {}: {} I=0x{:X}", pc, str, print_regs(), idx);
}

namespace potato_chip
{
    void unknown(uint16_t opcode)
    {
        print_op(fmt::format("Unknown opcode 0x{}", opcode));
    }

    void sys(uint16_t addr)
    {
        print_op(fmt::format("SYS 0x{} - Not implemented", addr));
    }

    void cls()
    {
        print_op("CLS");
        display.fill(0);
    }
    
    void call(uint16_t addr)
    {
        print_op(fmt::format("CALL 0x{}", addr));
		stack[sp] = pc;
		++sp;
		pc = addr;
		pc -= 2; // Don't advance.
    }

    void rts()
    {
        print_op("RTS");
		pc = stack[sp];
		--sp;

		pc -= 2; // Don't advance.
    }

    void jump(uint16_t addr)
    {
        print_op(fmt::format("JUMP 0x{}", addr));
		pc = addr;
		pc -= 2; // Don't advance.
    }

    void jumpi(uint16_t addr)
    {
		print_op(fmt::format("JUMPI V(0), 0x{:X}", addr));
		pc = V[0] + addr;
		pc -= 2; // Don't advance
    }

    void ske(uint8_t reg, uint8_t num)
    {
		print_op(fmt::format("SKE V(0x{:X}), 0x{:X}", reg, num));
		if (V[reg] == num) pc += 2;
    }

    void skne(uint8_t reg, uint8_t num)
    {
		print_op(fmt::format("SKNE V(0x{:X}), 0x{:X}", reg, num));
		if (V[reg] != num) pc += 2;
    }

    void skre(uint8_t x, uint8_t y)
    {
		print_op(fmt::format("SKRE V(0x{:X}), V(0x{:X})", x, y));
		if (V[x] == V[y]) pc += 2;
    }

    void skrne(uint8_t x, uint8_t y)
    {
		print_op(fmt::format("SKRNE V(0x{:X}), V(0x{:X})", x, y));
		if (V[x] != V[y]) pc += 2;
    }

    void load(uint8_t reg, uint8_t num)
    {
        print_op(fmt::format("LOAD V(0x{:X}), 0x{:X}", reg, num));
		V[reg] = num;
    }

    void loadi(uint16_t addr)
    {
        print_op(fmt::format("LOADI 0x{:X}", addr));
		idx = addr;
    }

    void math_add(uint8_t reg, uint8_t num)
    {
        print_op(fmt::format("ADD V(0x{:X}), 0x{:X}", reg, num));
		V[reg] += num;
    }

    void math_move(uint8_t x, uint8_t y)
    {
        print_op(fmt::format("MOVE V(0x{:X}), V(0x{:X})", x, y));
		V[x] = V[y];
    }

    void math_or(uint8_t x, uint8_t y)
    {
        print_op(fmt::format("OR V(0x{:X}), V(0x{:X})", x, y));
		V[x] = V[x] | V[y];
    }

    void math_and(uint8_t x, uint8_t y)
    {
        print_op(fmt::format("AND V(0x{:X}), V(0x{:X})", x, y));
		V[x] = V[x] & V[y];
    }

    void math_xor(uint8_t x, uint8_t y)
    {
        print_op(fmt::format("XOR V(0x{:X}), V(0x{:X})", x, y));
		V[x] = V[x] ^ V[y];
    }

    void math_add_r(uint8_t x, uint8_t y)
    {
        print_op(fmt::format("ADDR V(0x{:X}), V(0x{:X})", x, y));
		V[0xF] = (V[y] > V[x]); // Carry
		V[x] += V[y];
    }

    void math_sub_r(uint8_t x, uint8_t y)
    {
        print_op(fmt::format("SUBR V(0x{:X}), V0x{:X})", x, y));
		V[0xF] = (V[x] > V[y]); // borrow
		V[x] -= V[y];
    }

    void math_subn_r(uint8_t x, uint8_t y)
    {
        print_op(fmt::format("SUBN V(0x{:X}), V(0x{:X})", x, y));
		V[0xF] = (V[y] > V[x]); // borrow
		V[x] = V[y] - V[x];
    }

    void math_shr(uint8_t x)
    {
        print_op(fmt::format("SHR V(0x{:X})", x));
		V[0xF] = V[x] & 0x1;
		V[x] >>= 1;
    }

    void math_shl(uint8_t x)
    {
        print_op(fmt::format("SHL V(0x{:X})", x));
		V[0xF] = (V[x] >> 7) & 0x1;
		V[x] <<= 1;
    }

    void rand(uint8_t x, uint16_t kk)
    {
        print_op(fmt::format("RND V(0x{:X}), 0x{:X}", x, kk));
		V[x] = ((mt_rand() % 0xff) & kk);
		spdlog::get("potato")->debug("random: 0x{:X}", V[x]);
    }

    void skpr(uint8_t x)
    {
        print_op(fmt::format("SKPR V(0x{:X})", x));
		if (keys[x] != 0) pc += 2;

    }

    void sknp(uint8_t x)
    {
        print_op(fmt::format("SKNP V(0x{:X})", x));
		if (keys[x] == 0) pc += 2;
    }

    void moved(uint8_t x)
    {
        print_op(fmt::format("MOVED V(0x{:X})", x));
		V[x] = delay_timer;
    }

    void keyd(uint8_t x)
    {
        print_op(fmt::format("KEYD V(0x{:X})", x));
		bool key_pressed = false;

		for (int i = 0; i < 0xF; i++)
		{
			if (keys[i] = 1) 
            {
                key_pressed = true;
                V[x] = i;
            }
		}

		if (!key_pressed) pc -= 2;
    }

    void loadd(uint8_t x)
    {
        print_op(fmt::format("LOADD V(0x{:X})", x));
		delay_timer = V[x];
    }

    void loads(uint8_t x)
    {
        print_op(fmt::format("LOADS V(0x{:X})", x));
		sound_timer = V[x];
    }

    void addi(uint8_t x)
    {
        print_op(fmt::format("ADDI V(0x{:X})", x));
		idx += V[x];
    }

    void ldspr(uint8_t x)
    {
        print_op(fmt::format("LDSPR V(0x{:X})", x));
        idx = 5 * (V[x] & 0xF);
    }

    void bcd(uint8_t x)
    {
        print_op(fmt::format("BCD V(0x{:X})", x));
		mem[idx] = V[x] / 100;
		mem[idx + 1] = (V[x] / 10) % 10;
		mem[idx + 2] = (V[x] % 100) % 10;
    }

    void stor(uint8_t x)
    {
        print_op(fmt::format("STOR V(0x{:X})", x));
		for (uint16_t i = 0; i < x; i++)
		{
			mem[idx + i] = V[i];
		}
        idx += x + 1;
    }

    void read(uint8_t x)
    {
        print_op(fmt::format("READ V(0x{:X})", x));
		for (uint16_t i = 0; i < x; i++)
		{
			V[i] = mem[idx + i];
		}
        idx += x + 1;
    }

    void draw(uint8_t x, uint8_t y, uint8_t n)
    {
        print_op(fmt::format("DRAW V(0x{:X}) V(0x{:X}), 0x{:X}", x, y, n));
		uint16_t tst = 0;

		V[0xF] = 0;

		for (int y2 = 0; y2 < n; y2++)
		{
			tst = mem[idx + y2];

			for (int x2 = 0; x2 < 8; x2++)
			{
				if ((tst & (0x80 >> x2)) != 0)
				{
                    uint32_t loc = (V[x] + x2 + ((V[y] + y2) * 0x40));
                    //spdlog::get("potato")->debug("l -> 0x{:X}", loc);

					if (display[loc] == 1) V[0xF] = 1;

					display[loc] ^= 1;
				}
			}
		}
		display_changed = true;
    }
}