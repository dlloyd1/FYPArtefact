#include "Texture2D.h"

Texture2D::Texture2D(SDL_Renderer* renderer)
{
	mRenderer = renderer;
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

Texture2D::~Texture2D()
{
	Free();
	mRenderer = NULL;
}

bool Texture2D::LoadFromFile(string path)
{
	Free();

	SDL_Texture* pTexture = NULL;

	SDL_Surface* pSurface = IMG_Load(path.c_str());
	if (pSurface)
	{
		//Colour key the image
		SDL_SetColorKey(pSurface, SDL_TRUE, SDL_MapRGB(pSurface->format, 0, 0xFF, 0xFF));

		//create texture
		pTexture = SDL_CreateTextureFromSurface(mRenderer, pSurface);
		if (pTexture != NULL)
		{
			mWidth = pSurface->w;
			mHeight = pSurface->h;
		}
		else
		{
			cout << "Unable to create texture from SDL_Surface. Error: " << IMG_GetError() << endl;
		}

		//SDL_Surface object no longer needed once loaded as a texture
		SDL_FreeSurface(pSurface);
	}
	else
	{
		cout << "SDL_Surface not loaded correctly. Error: " << IMG_GetError() << endl;
	}

	mTexture = pTexture;

	//Return process success
	return mTexture != NULL;
}

void Texture2D::Free()
{
	if (mTexture != NULL)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void Texture2D::Render(double x, double y, SDL_RendererFlip flip, double angle)
{
	//Set where to render the texture.
	SDL_Rect renderLocation = { (int)x, (int)y, mWidth, mHeight };

	//Render to screen.
	SDL_RenderCopyEx(mRenderer, mTexture, NULL, &renderLocation, angle, NULL, flip);
}

void Texture2D::Render(Vector2D newPosition, SDL_RendererFlip flip, double angle)
{
	Render(newPosition.x, newPosition.y, flip, angle);
}

void Texture2D::Render(SDL_Rect srcRect, SDL_Rect destRect, SDL_RendererFlip flip, double angle)
{
	//Render to screen.
	SDL_RenderCopyEx(mRenderer, mTexture, &srcRect, &destRect, angle, NULL, flip);
}
