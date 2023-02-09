#include "./include/game.hpp"

#include "./imgui-docking/imgui.h"
#include "./imgui-docking/backends/imgui_impl_sdl2.h"
#include "./imgui-docking/backends/imgui_impl_sdlrenderer.h"

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
                                 1280, 720,
                                 this->wfs);
    this->renderer = SDL_CreateRenderer(this->win, -1,
                                        SDL_RENDERER_PRESENTVSYNC
                                        | SDL_RENDERER_ACCELERATED);
	if (renderer == NULL)
		this->ok = false;

	this-> s = SDL_GetWindowSurface(this->win);
    if (!this->s) {
        SDL_Log("Failed to get window surface: %s", SDL_GetError());
        SDL_DestroyWindow(this->win);
        SDL_Quit();
        return;
    }

    this->gridS =
		SDL_CreateRGBSurface(0, 1280, 720, 32, 0, 0, 0, 0);
    if (!this->gridS) {
        SDL_Log("Failed to create grid surface: %s", SDL_GetError());
        SDL_FreeSurface(this->s);
        SDL_DestroyWindow(this->win);
        SDL_Quit();
        return;
    }

    drawGrid(this->gridS);
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
    SDL_FreeSurface(this->gridS);
    SDL_FreeSurface(this->s);
    SDL_DestroyRenderer(this->renderer);
    SDL_DestroyWindow(this->win);
    SDL_Quit();
}

void
fabio::drawGrid(SDL_Surface *surface)
{
    // Draw vertical lines
    for (int x = 0; x < surface->w; x += GRID_CELL_SIZE) {
        SDL_Rect rect = { x, 0, 1, surface->h };
        SDL_FillRect(surface, &rect, GRID_COLOR);
    }

    // Draw horizontal lines
    for (int y = 0; y < surface->h; y += GRID_CELL_SIZE) {
        SDL_Rect rect = { 0, y, surface->w, 1 };
        SDL_FillRect(surface, &rect, GRID_COLOR);
    }
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

        SDL_BlitSurface(this->gridS, NULL, this->s, NULL);
        SDL_UpdateWindowSurface(this->win);

		/* END */

		ImGui::Render();
		// ImGui::GetIO();
		SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x,
							io.DisplayFramebufferScale.y);
		SDL_SetRenderDrawColor(this->renderer, (Uint8)(1 * 255),
								(Uint8)(1 * 255),
								(Uint8)(0.3f * 255),
								(Uint8)(1 * 255));
		SDL_RenderClear(this->renderer);
		ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
		SDL_RenderPresent(this->renderer);
	}
}
