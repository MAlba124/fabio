#include <string>
#include <thread>

#include <boost/asio.hpp>

#include <SDL.h>

#include "./include/game.hpp"

#include "./imgui-docking/imgui.h"
#include "./imgui-docking/backends/imgui_impl_sdl2.h"
#include "./imgui-docking/backends/imgui_impl_sdlrenderer.h"

#include "../net/client/include/client.hpp"

const int WINDOW_HEIGHT = 720;
const int WINDOW_WIDTH = 1280;
const int GRID_CELL_SIZE = 15;
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
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForSDLRenderer(this->win, this->renderer);
    ImGui_ImplSDLRenderer_Init(this->renderer);

    boost::asio::io_context io_context;
    net::client::Client client(io_context);

	char addr[20] = "";
	int port = 1233;
	std::thread iocThread;
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

		SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x,
							io.DisplayFramebufferScale.y);

		/* START */

        SDL_SetRenderDrawColor(this->renderer, 0xFF, 0xFF, 0xFF, 0xFF); // ?
		SDL_RenderClear(this->renderer);


		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags =
			ImGuiWindowFlags_MenuBar
			| ImGuiWindowFlags_NoDocking;
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags
			|= ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoMove;
		window_flags
			|= ImGuiWindowFlags_NoBringToFrontOnFocus
			| ImGuiWindowFlags_NoNavFocus;

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Fabio trading", (bool *)true, window_flags);
        ImGui::PopStyleVar();
        ImGui::PopStyleVar(2);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MainDockspace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Options"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows,
				// which we can't undo at the moment without finer window depth/z control.
				//ImGui::Separator();
				//ImGui::Separator();

				if (ImGui::MenuItem("Close", NULL, false, true))
					done = true;

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
		ImGui::End();

		/* MainFrame */
        ImGui::Begin("MainFrame");

        ImGui::End();

		/* ClientConnect */
		if (!client.isConnected())
		{
			ImGui::Begin("Connect to server");

			ImGui::Text("Connect to a remote server");

			ImGui::InputText("Address", (char *)&addr, 19);
			ImGui::InputInt("Port", &port);

			if (ImGui::Button("Connect"))
			{
				if (strlen(addr) > 0 && port > -1)
				{
					client.connect(std::string(addr), port);
					if (client.isConnected())
					{
						iocThread = std::thread([&io_context]
						{ io_context.run(); });
					}
				}
			}

			ImGui::End();
		}
		else {
			ImGui::Begin("Server");

			ImGui::Text("Connected to server");

			// Why not work???
			if (ImGui::Button("Disconnect"))
			{
				client.disconnect();
				if (iocThread.joinable())
					iocThread.join();
			}

			ImGui::End();
		}

		/* END */

		ImGui::Render();
		ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());

		SDL_RenderPresent(this->renderer);
	}
}
