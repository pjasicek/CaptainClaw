#ifndef UTIL_H_
#define UTIL_H_

#include <memory>
#include <vector>
#include <string>
#include <libwap.h>
#include <SDL2/SDL.h>
#include "../SharedDefines.h"

struct TileCollisionPrototype;
struct TileDescription;

namespace Util
{
    std::string ConvertToThreeDigitsString(int32_t num);

    SDL_Rect WwdRectToSDLRect(WwdRect& rect);

    void ParseCollisionRectanglesFromTile(TileCollisionPrototype* tilePrototype, TileDescription* tileDesc);

    void SplitStringIntoVector(std::string str, std::vector<std::string>& vec);

    void PrintRect(SDL_Rect rect, std::string comment);

    int GetRandomNumber(int fromRange, int toRange);
}

#endif