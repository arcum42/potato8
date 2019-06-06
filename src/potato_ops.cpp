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


void print_op(const char *str)
{
	spdlog::get("potato")->debug("pc[0x{:X}] {}: op=0x{:X} mem(0x{:X}).", pc, str, op, mem[pc]);
}

namespace potato_chip
{
    void unknown(uint16_t opcode)
    {
        print_op("Unknown opcode");
    }

    void sys(uint16_t nnn)
    {
        print_op("SYS nnn: Not yet implemented");
    }

    void cls()
    {
        print_op("CLS");
        display.fill(0);
    }
    
    void call(uint16_t addr)
    {
        print_op("CALL addr");
		//printf("Calling 0x%x.\n", nnn);

		stack[sp] = pc;
		++sp;
		pc = addr;
		pc -= 2; // Don't advance.
    }

    void rts()
    {
        print_op("RET");
		pc = stack[sp];
		--sp;

		pc -= 2; // Don't advance.
    }

    void jump(uint16_t addr)
    {
        print_op("JP addr");

		//printf("Jumping to 0x%x.\n", nnn);
		pc = addr;
		pc -= 2; // Don't advance.
    }

    void jumpi(uint16_t addr)
    {
		print_op("JP V0, addr");

		pc = V[0] + addr;
		pc -= 2; // Don't advance
    }

    void ske(uint8_t reg, uint8_t num)
    {
		print_op("SE Vx, kk");

		if (V[reg] == num) pc += 2;
    }

    void skne(uint8_t reg, uint8_t num)
    {
		print_op("SNE Vx, kk");

		if (V[reg] != num) pc += 2;
    }

    void skre(uint8_t x, uint8_t y)
    {
		print_op("SE Vx, Vy");

		if (V[x] == V[y]) pc += 2;
    }

    void skrne(uint8_t x, uint8_t y)
    {
		print_op("SKRNE Vx, Vy");

		if (V[x] != V[y]) pc += 2;
    }

    void load(uint8_t reg, uint8_t num)
    {
	    print_op("LD Vx, kk");

		V[reg] = num;
    }

    void loadi(uint16_t addr)
    {
		print_op("LD I, addr");
		idx = addr;
    }

    void math_add(uint8_t reg, uint8_t num)
    {
        print_op("ADD Vx, kk");

		V[reg] += num;
    }

    void math_move(uint8_t x, uint8_t y)
    {
        print_op("LD Vx, Vy");

		V[x] = V[y];
    }

    void math_or(uint8_t x, uint8_t y)
    {
        print_op("OR Vx, Vy");

		V[x] = V[x] | V[y];
    }

    void math_and(uint8_t x, uint8_t y)
    {
        print_op("AND Vx, Vy");

		V[x] = V[x] & V[y];
    }

    void math_xor(uint8_t x, uint8_t y)
    {
        print_op("XOR Vx, Vy");

		V[x] = V[x] ^ V[y];
    }

    void math_add_r(uint8_t x, uint8_t y)
    {
		print_op("ADD Vx, Vy");

		V[0xF] = (V[y] > V[x]); // Carry
		V[x] += V[y];
    }

    void math_sub_r(uint8_t x, uint8_t y)
    {
		print_op("SUB Vx, Vy");

		V[0xF] = (V[x] > V[y]); // borrow
		V[x] -= V[y];
    }

    void math_subn_r(uint8_t x, uint8_t y)
    {
		print_op("SUBN Vx, Vy");

		V[0xF] = (V[y] > V[x]); // borrow
		V[x] = V[y] - V[x];
    }

    void math_shr(uint8_t x)
    {
		print_op("SHR V");

		V[0xF] = V[x] & 0x1;
		V[x] >>= 1;
    }

    void math_shl(uint8_t x)
    {
		print_op("SHL Vx");

		V[0xF] = (V[x] >> 7) & 0x1;
		V[x] <<= 1;
    }

    void rand(uint8_t x, uint16_t kk)
    {
		print_op("RND Vx, byte");

		V[x] = ((mt_rand() % 0xff) & kk);
		spdlog::get("potato")->debug("random: {:x}", V[x]);
    }

    void skpr(uint8_t x)
    {
		print_op("SKP Vx");

		if (keys[x] != 0) pc += 2;

    }

    void sknp(uint8_t x)
    {
		print_op("SKP Vx");

		if (keys[x] == 0) pc += 2;
    }

    void moved(uint8_t x)
    {
		print_op("LD Vx, DT");

		V[x] = delay_timer;
    }

    void keyd(uint8_t x)
    {
        print_op("LD Vx, K: wait for key press");
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
		print_op("LD DT, Vx");

		delay_timer = V[x];
    }

    void loads(uint8_t x)
    {
		print_op("LD ST, Vx");

		sound_timer = V[x];
    }

    void addi(uint8_t x)
    {
        print_op("ADD I, Vx");

		idx += V[x];
    }

    void ldspr(uint8_t x)
    {
		print_op("LD F, Vx - idx = loc of sprite");
        idx = 5 * (V[x] & 0xF);
    }

    void bcd(uint8_t x)
    {
		print_op("BCD (LD B, Vx)");

		mem[idx] = V[x] / 100;
		mem[idx + 1] = (V[x] / 10) % 10;
		mem[idx + 2] = (V[x] % 100) % 10;
    }

    void stor(uint8_t x)
    {
		print_op("LD [I], Vx");

		for (uint16_t i = 0; i < x; i++)
		{
			mem[idx + i] = V[i];
		}
        idx += x + 1;
    }

    void read(uint8_t x)
    {
		print_op("LD Vx, [I]");

		for (uint16_t i = 0; i < x; i++)
		{
			V[i] = mem[idx + i];
		}
        idx += x + 1;
    }

    void draw(uint8_t x, uint8_t y, uint8_t n)
    {
			print_op("DRW Vx, Vy, n");
			uint16_t tst = 0;

			V[0xF] = 0;

			for (int y2 = 0; y2 < n; y2++)
			{
				tst = mem[idx + y2];

				for (int x2 = 0; x2 < 8; x2++)
				{
					if ((tst & (0x80 >> x2)) != 0)
					{
                        uint32_t loc = (x + x2 + ((y + y2) * 0x40));

						if (display[loc] == 1) V[0xF] = 1;

						display[loc] ^= 1;
					}
				}
			}
			display_changed = true;

    }
}