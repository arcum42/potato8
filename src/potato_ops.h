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
    extern void init_ops();
    extern void sys();
    extern void cls();
    extern void call();
    extern void jump();
    extern void jumpi();
    extern void rts();
    extern void unknown();
    extern void ske();
    extern void skne();
    extern void skre();
    extern void skrne();
    extern void load();
    extern void loadi();
    extern void math_add();
    extern void math_move();
    extern void math_or();
    extern void math_and();
    extern void math_xor();
    extern void math_add_r();
    extern void math_sub_r();
    extern void math_subn_r();
    extern void math_shr();
    extern void math_shl();
    extern void rand();
    extern void skpr();
    extern void sknp();
    extern void moved();
    extern void keyd();
    extern void loadd();
    extern void loads();
    extern void addi();
    extern void ldspr();
    extern void bcd();
    extern void stor();
    extern void read();
    extern void draw();
    extern void handle_special_ops();
    extern void handle_math_ops();
    extern void handle_misc_e_ops();
    extern void handle_misc_f_ops();
    extern void handle_op();
}