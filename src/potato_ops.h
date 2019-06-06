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

extern void print_op(const char *str);

namespace potato_chip
{
    extern void sys(uint16_t nnn);
    extern void cls();
    extern void call(uint16_t addr);
    extern void jump(uint16_t addr);
    extern void jumpi(uint16_t addr);
    extern void rts();
    extern void unknown(uint16_t opcode);
    extern void ske(uint8_t reg, uint8_t num);
    extern void skne(uint8_t reg, uint8_t num);
    extern void skre(uint8_t x, uint8_t y);
    extern void skrne(uint8_t x, uint8_t y);
    extern void load(uint8_t reg, uint8_t num);
    extern void loadi(uint16_t addr);
    extern void math_add(uint8_t reg, uint8_t num);
    extern void math_move(uint8_t x, uint8_t y);
    extern void math_or(uint8_t x, uint8_t y);
    extern void math_and(uint8_t x, uint8_t y);
    extern void math_xor(uint8_t x, uint8_t y);
    extern void math_add_r(uint8_t x, uint8_t y);
    extern void math_sub_r(uint8_t x, uint8_t y);
    extern void math_subn_r(uint8_t x, uint8_t y);
    extern void math_shr(uint8_t x);
    extern void math_shl(uint8_t x);
    extern void rand(uint8_t x, uint16_t kk);
    extern void skpr(uint8_t x);
    extern void sknp(uint8_t x);
    extern void moved(uint8_t x);
    extern void keyd(uint8_t x);
    extern void loadd(uint8_t x);
    extern void loads(uint8_t x);
    extern void addi(uint8_t x);
    extern void ldspr(uint8_t x);
    extern void bcd(uint8_t x);
    extern void stor(uint8_t x);
    extern void read(uint8_t x);
    extern void draw(uint8_t x, uint8_t y, uint8_t n);
}