#include "./include/game.hpp"

#include "./imgui-docking/imgui.h"
#include "./imgui-docking/backends/imgui_impl_sdl2.h"
#include "./imgui-docking/backends/imgui_impl_sdlrenderer.h"

const int WINDOW_HEIGHT = 720;
const int WINDOW_WIDTH = 1280;
const int GRID_CELL_SIZE = 50;
const int GRID_COLOR = 0x808080FF;

fabio::Game::Game()
	: ok(true)
{
    if (SDL_Init(SDL_INIT_VIDEO
                 | SDL_INIT_TIMER
                 | SDL_INIT_GAMECONTROLLER) != 0)
	{
		this->ok = false;
		return;
	}

    this->wfs = (SDL_WindowFlags)
        (SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    this->win = SDL_CreateWindow(WINDOW_NAME,
                                 SDL_WINDOWPOS_CENTERED,
                                 SDL_WINDOWPOS_CENTERED,
                                 WINDOW_WIDTH, WINDOW_HEIGHT,
                                 this->wfs);
    this->renderer = SDL_CreateRenderer(this->win, -1,
                                        SDL_RENDERER_PRESENTVSYNC
                                        | SDL_RENDERER_ACCELERATED);
	if (renderer == NULL)
		this->ok = false;
}

bool
fabio::Game::isOk()
{
	return this->ok;
}

void
fabio::Game::cleanup()
{
	this->ui.cleanup();
    SDL_DestroyRenderer(this->renderer);
    SDL_DestroyWindow(this->win);
    SDL_Quit();
}

void
fabio::Game::run()
{
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
	(void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForSDLRenderer(this->win, this->renderer);
    ImGui_ImplSDLRenderer_Init(this->renderer);

	bool done = false;
	while (!done)
	{
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
			ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT
                && event.window.event == SDL_WINDOWEVENT_CLOSE
                && event.window.windowID == SDL_GetWindowID(this->win))
                done = true;
        }

		ImGui_ImplSDLRenderer_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		/* START */

        ImGui::Begin("MainFrame");

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                    1000.0f / ImGui::GetIO().Framerate,
                    ImGui::GetIO().Framerate);

        ImGui::End();

		/* END */

		ImGui::Render();
		// ImGui::GetIO();
		SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x,
							io.DisplayFramebufferScale.y);

        SDL_SetRenderDrawColor(this->renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(this->renderer);

        SDL_SetRenderDrawColor(this->renderer, (GRID_COLOR >> 16) & 0xFF,
							   (GRID_COLOR >> 8) & 0xFF,
							   GRID_COLOR & 0xFF,
							   GRID_COLOR >> 24);
        for (int x = GRID_CELL_SIZE; x < WINDOW_WIDTH; x += GRID_CELL_SIZE) {
            SDL_RenderDrawLine(this->renderer, x, 0, x, WINDOW_HEIGHT);
        }
        for (int y = GRID_CELL_SIZE; y < WINDOW_HEIGHT; y += GRID_CELL_SIZE) {
            SDL_RenderDrawLine(this->renderer, 0, y, WINDOW_WIDTH, y);
        }

		ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());

		SDL_RenderPresent(this->renderer);
	}
}
