#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#include <stdio.h>
#include <iostream>
#include <string>

#include "Texture2D.h"
#include "PokerTable.h"
#include "Constants.h"
#include "Structures.h"

using namespace std;

//Globals
SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
TTF_Font* gFont = NULL;
PokerTable* pPokerTable = NULL;
Uint32 gOldTime;

bool InitSDL()
{
	//SDL Setup
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		cout << "SDL failed to initialise correctly, ERROR: " << SDL_GetError() << endl;
		return false;
	}
	else
	{
		//Linear texture filtering
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			cout << "Warning: Linear texture filtering unavailable" << endl;
		}
		
		//Create the window
		gWindow = SDL_CreateWindow("HoldemHorace", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, kScreenWidth, kScreenHeight, SDL_WINDOW_SHOWN);

		if (gWindow)
		{
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);

			if (gRenderer)
			{
				//int imageFlags = IMG_INIT_PNG | IMG_INIT_J PG | IMG_INIT_TIF;
				//init loading of PNG's
				if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
				{
					cout << "SDL_Image failed to initialise correctly, ERROR: " << IMG_GetError() << endl;
					return false;
				}

				//Text rendering setup
				TTF_Init();
				gFont = TTF_OpenFont("Fonts/arial.ttf", 20);
				if (!gFont)
				{
					std::cout << TTF_GetError() << std::endl;
				}
			}
			else
			{
				cout << "SDL_Renderer failed to initialise correctly, ERROR: " << SDL_GetError() << endl;
				return false;
			}
		}
		else
		{
			cout << "SDL_Window failed to be correctly created, Error: " << SDL_GetError() << endl;
		}
	}

	//successful SDL Init
	return true;
}

void CloseSDL()
{
	//release globals
	gRenderer = NULL;
	gWindow = NULL;
	TTF_CloseFont(gFont);
	gFont = NULL;

	//close SDL Subsystems.
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();
}

void Render()
{
	//Clear current SDL screen
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(gRenderer);

	pPokerTable->Render();
	
	//Update the SDL_Renderer
	SDL_RenderPresent(gRenderer);
}

bool Update()
{	
	SDL_Event event;
	SDL_PollEvent(&event);

	Uint32 currentTime = SDL_GetTicks();
	double elapsedTime = currentTime - gOldTime;

	gOldTime = currentTime;
	double msElapsed = elapsedTime / 1000.0f;
	
	switch (event.type)
	{	
	case SDL_QUIT: //click 'X' to exit
		return true;
		break;
	}

	//fixed time step
	while (msElapsed <= kMsPerUpdate)
	{
		//time elapsed since last update
		currentTime = SDL_GetTicks();
		elapsedTime = currentTime - gOldTime;

		//add ms gone by to the counter
		msElapsed += elapsedTime / 1000.0f;
	}

	pPokerTable->Update((float)elapsedTime, event);

	return false;
}

int main(int argc, char* args[])
{
	if (InitSDL())
	{
		//setup instance of Poker Table
		pPokerTable = new PokerTable(gRenderer, gFont);

		bool quit = false;
		gOldTime = SDL_GetTicks();

		cout << "Press Spacebar key to deal" << endl;
		
		while (!quit)
		{
			Render();
			quit = Update();
		}
	}

	if (pPokerTable != NULL)
	{
		delete pPokerTable;
	}
	CloseSDL();
	return 0;
}