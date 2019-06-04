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

bool load_file(const char *filename)
{
	std::ifstream f;
	char *buffer;

    printf("Opening file %s.\n", filename);
	f.open(filename, std::ios::binary | std::ios::ate);

	if (f.is_open())
	{
        printf("Open!\n");

		std::streamsize size = f.tellg();
        f.seekg(0, std::ios::beg);

		buffer = new char[size];
		f.read(buffer, size);
		f.close();

		for (int i = 0; i < size; i++)
		{
			mem[0x200 + i] = buffer[i];
            //printf("%c", buffer[i]);
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

	printf("Initialize.\n");

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

void print_op(const char *str)
{
	printf("pc[0x%X] %s: op=0x%X mem(0x%X).\n", pc, str, op, mem[pc]);
}

void handle_op()
{
	uint8_t x = op & 0x0f00 >> 8;
	uint8_t y = op & 0x00f0 >> 4;
	uint16_t nnn = op & 0x0fff;
	uint8_t kk = op & 0x00ff;

    /*printf("[0x%X] op: 0x%x x: 0x%x y: 0x%x nnn: 0x%x kk: 0x%x ", pc, op, x, y, nnn, kk);
    for(int i = 0; i < 0xF; i++)
    {
        printf("v[0x%x]=0x%x ", i, V[i]);
    }
    printf("\n");*/

	switch (op & 0xf000)
	{
		case 0x0000:
			switch (op & 0xff)
			{
				case 0x00:
					print_op("SYS nnn: Not yet implemented");
					break;

				case 0xE0:
				{
					print_op("CLS");
					display.fill(0);
				}
				break;

				case 0xEE:
				{
					print_op("RET");
					pc = stack[sp];
					--sp;

					pc -= 2; // Don't advance.
				}
				break;

				default:
				{
					print_op("Unknown opcode");
				}
				break;
			}
			break;

		case 0x1000:
		{
			print_op("JP nnn");

			//printf("Jumping to 0x%x.\n", nnn);
			pc = nnn;
			pc -= 2; // Don't advance.
		}
		break;

		case 0x2000:
		{
			print_op("CALL addr");
			//printf("Calling 0x%x.\n", nnn);

			stack[sp] = pc;
			++sp;
			pc = nnn;
			pc -= 2; // Don't advance.
		}
		break;

		case 0x3000:
		{
			print_op("SE Vx, kk");

			if (V[x] == kk) pc += 2;
		}
		break;

		case 0x4000:
		{
			print_op("SNE Vx, kk");

			if (V[x] != kk) pc += 2;
		}
		break;

		case 0x5000:
		{
			print_op("SE Vx, Vy");

			if (V[x] == V[y]) pc += 2;
		}
		break;

		case 0x6000:
		{
			print_op("LD Vx, kk");

			V[x] = kk;
		}
		break;

		case 0x7000:
		{
			print_op("ADD Vx, kk");

			V[x] += kk;
		}
		break;

		case 0x8000:
		{
			switch (op & 0xf)
			{
				case 0x0:
				{
					print_op("LD Vx, Vy");

					V[x] = V[y];
				}
				break;

				case 0x1:
				{
					print_op("OR Vx, Vy");

					V[x] = V[x] | V[y];
				}
				break;

				case 0x2:
				{
					print_op("AND Vx, Vy");

					V[x] = V[x] & V[y];
				}
				break;

				case 0x3:
				{
					print_op("XOR Vx, Vy");

					V[x] = V[x] ^ V[y];
				}
				break;

				case 0x4:
				{
					print_op("ADD Vx, Vy");

					V[0xF] = (V[y] > V[x]); // Carry
					V[x] += V[y];
				}
				break;

				case 0x5:
				{
					print_op("SUB Vx, Vy");

					V[0xF] = (V[x] > V[y]); // borrow
					V[x] -= V[y];
				}
				break;

				case 0x6: // Shift right
				{
					print_op("SHR V");

					V[0xF] = V[x] & 0x1;
					V[x] >>= 1;
				}
				break;

				case 0x7:
				{
					print_op("SUBN Vx, Vy");

					V[0xF] = (V[y] > V[x]); // borrow
					V[x] = V[y] - V[x];
				}
				break;

				case 0xE: // Shift left
				{
					print_op("SHL Vx");

					V[0xF] = (V[x] >> 7) & 0x1;
					V[x] <<= 1;
				}
				break;

				default:
				{
					print_op("Unknown opcode");
				}
				break;
			}
		}
		break;

		case 0x9000:
		{
			print_op("SNE Vx, Vy");

			if (V[x] != V[y]) pc += 2;
		}
		break;

		case 0xA000:
		{
			print_op("LD I, addr");
			idx = nnn;
		}
		break;

		case 0xB000:
		{
			print_op("JP V0, addr");
			pc = V[0] + nnn;
			pc -= 2; // Don't advance
		}
		break;

		case 0xC000:
		{
			print_op("RND Vx, byte");

			V[x] = ((mt_rand() % 0xff) & kk);
			printf("random: %x\n", V[x]);
		}
		break;

		case 0xD000:
		{
			print_op("DRW Vx, Vy, n");
			uint16_t h = op & 0x000f;
			uint16_t tst = 0;

			V[0xF] = 0;

			for (int y2 = 0; y2 < h; y2++)
			{
				tst = mem[idx + y2];

				for (int x2 = 0; x2 < 8; x2++)
				{
					if ((tst & (0x80 >> x2)) != 0)
					{
                        uint32_t loc = (x + x2 + ((y + y2) * 0x40));

						if (display[loc] == 1) V[0xF] = 1;

						display[loc] ^= 1;
                        printf("*");
					}
				}
			}
            printf("\n");
			display_changed = true;
		}
		break;

		case 0xE000:
		{
			switch (op & 0xff)
			{
				case 0x9E:
				{
					print_op("SKP Vx");

					if (keys[x] != 0) pc += 2;
				}
				break;

				case 0xA1:
				{
					print_op("SKNP Vx");

					if (keys[x] == 0) pc += 2;
				}
				break;

				default:
				{
					print_op("Unknown opcode");
				}
				break;
			}
		}
		break;

		case 0xF000:
		{
			switch (op & 0xff)
			{
				case 0x07:
				{
					print_op("LD Vx, DT");

					V[x] = delay_timer;
				}
				break;

				case 0xA:
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
				break;

				case 0x15:
				{
					print_op("LD DT, Vx");

					delay_timer = V[x];
				}
				break;

				case 0x18:
				{
					print_op("LD ST, Vx");

					sound_timer = V[x];
				}
				break;

				case 0x1E:
				{
					print_op("ADD I, Vx");

					idx += V[x];
				}
				break;

				case 0x29:
				{
					print_op("LD F, Vx - idx = loc of sprite");
                    idx = 5 * (V[x] & 0xF);
				}
				break;

				case 0x33:
				{
					print_op("BCD (LD B, Vx)");

					mem[idx] = V[x] / 100;
					mem[idx + 1] = (V[x] / 10) % 10;
					mem[idx + 2] = (V[x] % 100) % 10;
				}
				break;

				case 0x55:
				{
					print_op("LD [I], Vx");

					for (uint16_t i = 0; i < x; i++)
					{
						mem[idx + i] = V[i];
					}
                    idx += x + 1;
				}
				break;

				case 0x65:
				{
					print_op("LD Vx, [I]");

					for (uint16_t i = 0; i < x; i++)
					{
						V[i] = mem[idx + i];
					}
                    idx += x + 1;
				}
				break;

				default:
				{
					print_op("Unknown opcode");
				}
				break;
			}
			break;

			default:
			{
				print_op("Unknown opcode");
			}
			break;
		}
	}
}

void main_loop()
{
	bool quit = false;

	printf("Enter main loop.\n");

	while (!quit)
	{
		op = mem[pc] << 8 | mem[pc + 1];

		handle_op();
		pc += 2;

		if (delay_timer > 0) delay_timer--;
		if (sound_timer > 0)
		{
			sound_timer--;
			printf("Beep!\n");
		}

		if (display_changed)
		{
			potato_ui_update();
			display_changed = false;
		}

		quit = potato_events();
        if (pc > 0x1000)
        {
            printf("Out of bounds! Closing.\n");
            quit = true;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

int main(int argc, char *args[])
{
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

	printf("Exit.\n");

	return 0;
}
