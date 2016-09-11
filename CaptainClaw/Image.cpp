#include <assert.h>
#include "Image.h"

Image::Image(WapPid* pid, SDL_Renderer* renderer)
{
    assert(pid != NULL);
    assert(renderer != NULL);

    _width = pid->width;
    _height = pid->height;
    _offsetX = pid->offsetX;
    _offsetY = pid->offsetY;

    _texture = GetTextureFromPid(pid, renderer);
    assert(_texture != NULL);
}

Image::~Image()
{
    SDL_DestroyTexture(_texture);
    _texture = NULL;
}

SDL_Rect Image::GetPositonRect(int32_t x, int32_t y)
{
    int32_t positionX = x - _width / 2 + _offsetX;
    int32_t positionY = y - _height / 2 + _offsetY;

    SDL_Rect rect = { positionX, positionY, _width, _height };

    return rect;
}

static void PutPixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        }
        else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}

SDL_Texture* Image::GetTextureFromPid(WapPid* pid, SDL_Renderer* renderer)
{
    assert(pid != NULL);
    assert(renderer != NULL);
    
    uint32_t rmask, gmask, bmask, amask;
    uint32_t width = pid->width;
    uint32_t height = pid->height;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif

    SDL_Surface* surface = SDL_CreateRGBSurface(0, width, height, 32, rmask, gmask, bmask, amask);
    assert(surface != NULL);

    uint32_t colorIdx;
    uint32_t colorsCount = pid->colorsCount;
    for (colorIdx = 0; colorIdx < colorsCount; colorIdx++)
    {
        WAP_ColorRGBA color = pid->colors[colorIdx];
        uint32_t x = colorIdx % width;
        uint32_t y = colorIdx / width;

        PutPixel(surface, x, y, SDL_MapRGBA(surface->format, color.r, color.g, color.b, color.a));
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    assert(texture != NULL);

    SDL_FreeSurface(surface);

    return texture;
}