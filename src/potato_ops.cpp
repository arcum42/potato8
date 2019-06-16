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

using opFcn = void(*)(); 
std::array<opFcn, 0x10>op_table;
std::array<opFcn, 0x10>math_table;
std::array<opFcn, 0x100>special_table;
std::array<opFcn, 0x100>misc_e_table;
std::array<opFcn, 0x100>misc_f_table;

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
    void unknown()
    {
        print_op(fmt::format("Unknown opcode 0x{}", op.op_code()));
    }

    void sys()
    {
        print_op(fmt::format("SYS 0x{} - Not implemented", op.nnn()));
    }

    void cls()
    {
        print_op("CLS");
        display.fill(0);
    }
    
    void call()
    {
        print_op(fmt::format("CALL 0x{}", op.nnn()));
		stack[sp] = pc;
		++sp;
		pc = op.nnn();
		pc -= 2; // Don't advance.
    }

    void rts()
    {
        print_op("RTS");
		pc = stack[sp];
		--sp;

		pc -= 2; // Don't advance.
    }

    void jump()
    {
        print_op(fmt::format("JUMP 0x{}", op.nnn()));
		pc = op.nnn();
		pc -= 2; // Don't advance.
    }

    void jumpi()
    {
		print_op(fmt::format("JUMPI V(0), 0x{:X}", op.nnn()));
		pc = V[0] + op.nnn();
		pc -= 2; // Don't advance
    }

    void ske()
    {
		print_op(fmt::format("SKE V(0x{:X}), 0x{:X}", op.x(), op.kk()));
		if (V[op.x()] == op.kk()) pc += 2;
    }

    void skne()
    {
		print_op(fmt::format("SKNE V(0x{:X}), 0x{:X}", op.x(), op.kk()));
		if (V[op.x()] != op.kk()) pc += 2;
    }

    void skre()
    {
		print_op(fmt::format("SKRE V(0x{:X}), V(0x{:X})", op.x(), op.y()));
		if (V[op.x()] == V[op.y()]) pc += 2;
    }

    void skrne()
    {
		print_op(fmt::format("SKRNE V(0x{:X}), V(0x{:X})", op.x(), op.y()));
		if (V[op.x()] != V[op.y()]) pc += 2;
    }

    void load()
    {
        print_op(fmt::format("LOAD V(0x{:X}), 0x{:X}", op.x(), op.kk()));
		V[op.x()] = op.kk();
    }

    void loadi()
    {
        print_op(fmt::format("LOADI 0x{:X}", op.nnn()));
		idx = op.nnn();
    }

    void math_add()
    {
        print_op(fmt::format("ADD V(0x{:X}), 0x{:X}", op.x(), op.kk()));
		V[op.x()] += op.kk();
    }

    void math_move()
    {
        print_op(fmt::format("MOVE V(0x{:X}), V(0x{:X})", op.x(), op.y()));
		V[op.x()] = V[op.y()];
    }

    void math_or()
    {
        print_op(fmt::format("OR V(0x{:X}), V(0x{:X})", op.x(), op.y()));
		V[op.x()] = V[op.x()] | V[op.y()];
    }

    void math_and()
    {
        print_op(fmt::format("AND V(0x{:X}), V(0x{:X})", op.x(), op.y()));
		V[op.x()] = V[op.x()] & V[op.y()];
    }

    void math_xor()
    {
        print_op(fmt::format("XOR V(0x{:X}), V(0x{:X})", op.x(), op.y()));
		V[op.x()] = V[op.x()] ^ V[op.y()];
    }

    void math_add_r()
    {
        print_op(fmt::format("ADDR V(0x{:X}), V(0x{:X})", op.x(), op.y()));
		V[0xF] = (V[op.y()] > V[op.x()]); // Carry
		V[op.x()] += V[op.y()];
    }

    void math_sub_r()
    {
        print_op(fmt::format("SUBR V(0x{:X}), V0x{:X})", op.x(), op.y()));
		V[0xF] = (V[op.x()] > V[op.y()]); // borrow
		V[op.x()] -= V[op.y()];
    }

    void math_subn_r()
    {
        print_op(fmt::format("SUBN V(0x{:X}), V(0x{:X})", op.x(), op.y()));
		V[0xF] = (V[op.y()] > V[op.x()]); // borrow
		V[op.x()] = V[op.y()] - V[op.x()];
    }

    void math_shr()
    {
        print_op(fmt::format("SHR V(0x{:X})", op.x()));
		V[0xF] = V[op.x()] & 0x1;
		V[op.x()] >>= 1;
    }

    void math_shl()
    {
        print_op(fmt::format("SHL V(0x{:X})", op.x()));
		V[0xF] = (V[op.x()] >> 7) & 0x1;
		V[op.x()] <<= 1;
    }

    void rand()
    {
        print_op(fmt::format("RND V(0x{:X}), 0x{:X}", op.x(), op.kk()));
		V[op.x()] = ((mt_rand() % 0xff) & op.kk());
		spdlog::get("potato")->debug("random: 0x{:X}", V[op.x()]);
    }

    void skpr()
    {
        print_op(fmt::format("SKPR V(0x{:X})", op.x()));
		if (keys[V[op.x()]] != 0) pc += 2;

    }

    void sknp()
    {
        print_op(fmt::format("SKNP V(0x{:X})", op.x()));
		if (keys[V[op.x()]] == 0) pc += 2;
    }

    void moved()
    {
        print_op(fmt::format("MOVED V(0x{:X})", op.x()));
		V[op.x()] = delay_timer;
    }

    void keyd()
    {
        print_op(fmt::format("KEYD V(0x{:X})", op.x()));
		bool key_pressed = false;

		for (int i = 0; i < 0xF; i++)
		{
			if (keys[i] = 1) 
            {
                key_pressed = true;
                V[op.x()] = i;
            }
		}

		if (!key_pressed) pc -= 2;
    }

    void loadd()
    {
        print_op(fmt::format("LOADD V(0x{:X})", op.x()));
		delay_timer = V[op.x()];
    }

    void loads()
    {
        print_op(fmt::format("LOADS V(0x{:X})", op.x()));
		sound_timer = V[op.x()];
    }

    void addi()
    {
        print_op(fmt::format("ADDI V(0x{:X})", op.x()));
		idx += V[op.x()];
    }

    void ldspr()
    {
        print_op(fmt::format("LDSPR V(0x{:X})", op.x()));
        idx = 5 * (V[op.x()] & 0xF);
    }

    void bcd()
    {
        print_op(fmt::format("BCD V(0x{:X})", op.x()));
		mem[idx] = V[op.x()] / 100;
		mem[idx + 1] = (V[op.x()] / 10) % 10;
		mem[idx + 2] = (V[op.x()] % 100) % 10;
    }

    void stor()
    {
        print_op(fmt::format("STOR V(0x{:X})", op.x()));
		for (uint16_t i = 0; i < op.x(); i++)
		{
			mem[idx + i] = V[i];
		}
        idx += op.x() + 1;
    }

    void read()
    {
        print_op(fmt::format("READ V(0x{:X})", op.x()));
		for (uint16_t i = 0; i < op.x(); i++)
		{
			V[i] = mem[idx + i];
		}
        idx += op.x() + 1;
    }

    void draw()
    {
        print_op(fmt::format("DRAW V(0x{:X}) V(0x{:X}), 0x{:X}", op.x(), op.y(), op.n()));
		uint16_t tst = 0;

		V[0xF] = 0;

		for (int y2 = 0; y2 < op.n(); y2++)
		{
			tst = mem[idx + y2];

			for (int x2 = 0; x2 < 8; x2++)
			{
				if ((tst & (0x80 >> x2)) != 0)
				{
                    uint32_t loc = (V[op.x()] + x2 + ((V[op.y()] + y2) * 0x40));
                    //spdlog::get("potato")->debug("l -> 0x{:X}", loc);

					if (display[loc] == 1) V[0xF] = 1;

					display[loc] ^= 1;
				}
			}
		}
		display_changed = true;
    }

void handle_special_ops()
{
    special_table[op.op_code()]();
}

void handle_math_ops()
{
    math_table[op.op_code()]();
}

void handle_misc_e_ops()
{
    misc_e_table[op.op_code()]();
}

void handle_misc_f_ops()
{
    misc_f_table[op.op_code()]();
}

void handle_op()
{
	spdlog::get("potato")->debug("opcode = 0x{:X}, x:0x{:X}, y:0x{:X}, nnn:0x{:X}, kk:{:X}, n:0x{:X}", op.op_code(), op.x(), op.y(), op.nnn(), op.kk(), op.n());

    op_table[op.op_code()]();
}

void init_ops()
{
    op_table = { handle_special_ops, jump, call, ske, skne, skre, load, math_add, handle_math_ops, skrne, loadi, jumpi, rand, draw, handle_misc_e_ops, handle_misc_f_ops };
    math_table = { math_move, math_or, math_and, math_add_r, math_sub_r, math_shr, math_subn_r, unknown, unknown, unknown, unknown, unknown, unknown, math_shr, unknown };
    special_table = { sys, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
                      unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
                      unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
                      unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
                      unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
                      unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
                      unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
                      unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
                      unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
                      unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
                      unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
                      unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
                      unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
                      unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
                      cls, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, rts, unknown,
                      unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown
                    };
    misc_e_table = { unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
                     unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
                     unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
                     unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
                     unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
                     unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
                     unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
                     unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
                     unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
                     unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, skpr, unknown,
                     unknown, sknp, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
                     unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
                     unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
                     unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
                     unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
                     unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown
                    };
    misc_f_table = { unknown, unknown, unknown, unknown, unknown, unknown, unknown, moved, unknown, unknown, keyd, unknown, unknown, unknown, unknown, unknown,
                     unknown, unknown, unknown, unknown, unknown, loadd, unknown, unknown, loads, unknown, unknown, unknown, unknown, unknown, addi, unknown,
                     unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, ldspr, unknown, unknown, unknown, unknown, unknown, unknown,
                     unknown, unknown, unknown, bcd, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
                     unknown, unknown, unknown, unknown, unknown, stor, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
                     unknown, unknown, unknown, unknown, unknown, read, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
                     unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
                     unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
                     unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
                     unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
                     unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
                     unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
                     unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
                     unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
                     unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown,
                     unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown, unknown
                    };
}
}


