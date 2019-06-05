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

#include "potato_ui.h"
#include "potato8.h"

const int dot_size = 8;
constexpr int WINDOW_WIDTH = dot_size * 0x40;
constexpr int WINDOW_HEIGHT = dot_size * 0x30;

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_Event e;

const int key_values[0x10] =
	{
		SDLK_x, SDLK_1, SDLK_2, SDLK_3,
		SDLK_q, SDLK_w, SDLK_e, SDLK_a,
		SDLK_s, SDLK_d, SDLK_z, SDLK_c,
		SDLK_4, SDLK_r, SDLK_f, SDLK_v};

void clear_window()
{
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF); // black

	SDL_RenderClear(renderer);
}

bool init_potato_ui()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		spdlog::get("potato")->debug("SDL could not initialize! SDL_Error: {}", SDL_GetError());
		return false;
	}

	// Create an application window with the following settings:
	window = SDL_CreateWindow(
		"Potato 8",				 // window title
		SDL_WINDOWPOS_UNDEFINED, // initial x position
		SDL_WINDOWPOS_UNDEFINED, // initial y position
		WINDOW_WIDTH,			 // width, in pixels
		WINDOW_HEIGHT,			 // height, in pixels
		SDL_WINDOW_SHOWN);

	// Check that the window was successfully created
	if (window == NULL)
	{
		// In the case that the window could not be made...
		spdlog::get("potato")->debug("Could not create window: {}.", SDL_GetError());
		return false;
	}

	// Setup renderer
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	// Check that the window was successfully created
	if (renderer == NULL)
	{
		// In the case that the window could not be made...
		spdlog::get("potato")->debug("Could not create renderer: {}.", SDL_GetError());
		return false;
	}

	clear_window();
	return true;
}

void potato_ui_cleanup()
{
	// Close and destroy the window
	SDL_DestroyWindow(window);

	// Clean up
	SDL_Quit();
}

void potato_ui_update()
{
	spdlog::get("potato")->debug("Drawing the screen!");

    for (int i = 0; i < 0x40 * 0x30; i++)
    {
        int x = i % 0x40 * dot_size;
        int y = i / 0x40 * dot_size;
        //printf("(0x%x 0x%x) ", x, y);
        //if (x == 0) printf("\n");
        
        SDL_Rect r = {x, y, x + dot_size, y + dot_size};
        if (display[i])
			{
				SDL_SetRenderDrawColor(renderer, 0x33, 0xFF, 0x33, 0xFF);
                //printf("*");
			}
			else
			{
				SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
                //printf(" ");
			}
        //if (x == 0) printf("\n");

		SDL_RenderFillRect(renderer, &r);
    }

	//Update the surface
	SDL_RenderPresent(renderer);
}

bool potato_events()
{
	bool quit = false;

	//Handle events on queue
	while (SDL_PollEvent(&e) != 0)
	{
		switch (e.type)
		{
			//User requests to quit.
			case SDL_QUIT:
			{
				quit = true;
				break;
			}

			case SDL_KEYDOWN:
			{
				for (uint8_t i = 0; i < 0x10; i++)
				{
					if (e.key.keysym.sym == key_values[i]) keys[i] = 1;
				}
				break;
			}

			case SDL_KEYUP:
			{
				for (uint8_t i = 0; i < 0x10; i++)
				{
					if (e.key.keysym.sym == key_values[i]) keys[i] = 0;
				}
				break;
			}
		}
	}

	return quit;
}