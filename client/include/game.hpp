#ifndef __GAME_HPP
#define __GAME_HPP 1

#include <SDL.h>

#include "ui.hpp"

#define WINDOW_NAME "Fabio - Client"

namespace fabio
{
	void drawGrid(SDL_Surface *surface);

	class Game
	{
		private:
			bool ok;
			SDL_WindowFlags wfs;
			SDL_Window *win;
			SDL_Renderer *renderer;
			SDL_Surface *s;
			SDL_Surface *gridS;
			UI ui;
		public:
			Game();
			bool isOk();
			void cleanup();
			void run();
	};
}

#endif // __GAME_HPP
