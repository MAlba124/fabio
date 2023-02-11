#ifndef __GAME_HPP
#define __GAME_HPP 1

#include <SDL.h>

#include "ui.hpp"

#define WINDOW_NAME "Fabio - Client"

namespace fabio
{
	class Game
	{
		private:
			bool ok;
//			net::client::Client& client;
			SDL_WindowFlags wfs;
			SDL_Window *win;
			SDL_Renderer *renderer;
			UI ui;
		public:
			Game();
			bool isOk();
			void cleanup();
			void run();
	};
}

#endif // __GAME_HPP
