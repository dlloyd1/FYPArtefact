#ifndef _TEXTURE2D_H
#define _TEXTURE2D_H

#include <SDL.h>
#include <string>
#include <iostream>
#include <SDL_image.h>

#include "Structures.h"

using namespace std;

class Texture2D
{
public:
	Texture2D(SDL_Renderer* renderer);
	~Texture2D();
	
	bool LoadFromFile(string path);
	void Free();

	void Render(double x, double y, SDL_RendererFlip flip = SDL_FLIP_NONE, double angle = 0.0f);					
	void Render(Vector2D newPosition, SDL_RendererFlip flip = SDL_FLIP_NONE, double angle = 0.0f);
	void Render(SDL_Rect srcRect, SDL_Rect destRect, SDL_RendererFlip flip = SDL_FLIP_NONE, double angle = 0.0f);
	
	int GetWidth() { return mWidth; };
	int GetHeight() { return mHeight; };

private:
	SDL_Renderer* mRenderer;
	SDL_Texture* mTexture;

	int mWidth;
	int mHeight;
};
#endif // ! _TEXTURE2D_H
