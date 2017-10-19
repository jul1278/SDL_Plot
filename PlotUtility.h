// PlotUtility.h
#ifndef PLOTUTILITY_H
#define PLOTUTILITY_H

#include <iostream>
#include <exception>
#include <cstring>

#include "SDL.h"

/* SDL interprets each pixel as a 32-bit number, so our masks must depend
    on the endianness (byte order) of the machine */
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
    const uint32_t rmask = 0xff000000;
    const uint32_t gmask = 0x00ff0000;
    const uint32_t bmask = 0x0000ff00;
    const uint32_t amask = 0x000000ff;
#else
    const uint32_t rmask = 0x000000ff;
    const uint32_t gmask = 0x0000ff00;
    const uint32_t bmask = 0x00ff0000;
    const uint32_t amask = 0xff000000;
#endif


// DrawGridInfo
struct DrawGridInfo {
    uint32_t color;
    
    unsigned int x; 
    unsigned int y; 

    unsigned int height; 
    unsigned int width; 
    
    unsigned int xCount; 
    unsigned int yCount;
    
    bool dotted;
};

// DrawCandleInfo
struct DrawCandleInfo {

    uint32_t color; 

    unsigned int x; 
    unsigned int y; 

    unsigned int candleTop;
    unsigned int candleBottom;
    
    unsigned int width; 

    unsigned int topWickHeight; 
    unsigned int bottomWickHeight;   
}; 

//---------------------------------------------------------------------------------------------------------------------------------------------------
// Name: DrawCandle
// Desc:
//---------------------------------------------------------------------------------------------------------------------------------------------------
void DrawCandle(SDL_Renderer* renderer, const DrawCandleInfo& candleInfo) {

}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// Name: DrawDottedLine
// Desc:
//---------------------------------------------------------------------------------------------------------------------------------------------------
void DrawDottedLine(
    SDL_Renderer* renderer, 
    const unsigned int color, 
    int x1, 
    int y1, 
    int x2, 
    int y2, 
    const unsigned int period, 
    const unsigned int cycle) {

    // cant draw this
    if (x1 == x2 && y1 == y2) {
        return; 
    }

    // Always draw left to right/top to bottom so swap x's and y's if we need to
    if (x1 > x2) {
        std::swap(x1, x2); 
    }

    if (y1 > y2) {
        std::swap(y1, y2); 
    }

    auto xDiff = (x2 - x1) > 0 ? (x2 - x1) : 1; 
    auto yDiff = (y2 - y1) > 0 ? (y2 - y1) : 1; 

    auto surface = SDL_CreateRGBSurface(0, xDiff, yDiff, 32, rmask, gmask, bmask, amask); 
        
    if (surface == nullptr) {
        std::cout << "SDL_CreateRGBSurface failed\n"; 
        return; 
    }

    SDL_LockSurface(surface);

    auto arrSize = xDiff * yDiff;
    auto pixelData = static_cast<uint32_t*>(surface->pixels); 

    memset(pixelData, 0x00, arrSize * sizeof(uint32_t) ); 
    unsigned int counter = 0; 

    if (x1 == x2 || y1 == y2) {

        // perfectly vertical line
        auto indexTop = (y1 == y2) ? x2 - x1 : y2 - y1; 

        for (auto index = 0; index < indexTop; index++) {
            
            if (index < arrSize && counter < cycle) {
                pixelData[index] = color; 
            }

            counter++;
            if (counter > period) {
                counter = 0; 
            }
        }

    } else {
        
        // sorta bresenhams line algorithm 
        auto m = (float) yDiff / xDiff;
        unsigned int yCurr = 0; 

        for (auto x = 0; x < xDiff; x++) {

            // calculate predicted value - 
            auto error = m * x - (float) yCurr;

            if (m == 0) {
                error = y2 - yCurr; // ????? 
            }

            if (error >= 1.0f) {
                while (error >= 1.0f) {
                    
                    auto index = (yCurr * xDiff) + x;
                
                    // draw pixel
                    if (index < arrSize && counter < cycle) {
                        pixelData[index] = color; 
                    }
        
                    yCurr++;
                    counter++;
                    
                    if (counter > period) {
                        counter = 0;
                    }

                    error -= 1.0f; 
                }
            } else {

                auto index = (yCurr * xDiff) + x;
                
                // draw pixel
                if (index < arrSize && counter < cycle) {
                    pixelData[index] = color; 
                }

                counter++; 

                if (counter > period) {
                    counter = 0;
                }
            }
        }
    }

    SDL_UnlockSurface(surface);

    auto texture = SDL_CreateTextureFromSurface(renderer, surface); 

    SDL_Rect rect = {x1, y1, xDiff, yDiff}; 
    SDL_RenderCopy(renderer, texture, NULL, &rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture); 

    return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// Name: DrawGrid
//
//---------------------------------------------------------------------------------------------------------------------------------------------------
void DrawGrid(SDL_Renderer* renderer, const DrawGridInfo& drawGridInfo) {

    SDL_SetRenderDrawColor(renderer, drawGridInfo.color >> 24, (drawGridInfo.color & 0x00ff0000) >> 16, (drawGridInfo.color & 0x0000ff00) >> 8, (drawGridInfo.color & 0x000000ff)); 

    // TODO: fix division by zero below 

    unsigned int xSpacing = (drawGridInfo.width - drawGridInfo.x) / (drawGridInfo.xCount - 1); 
    unsigned int ySpacing = (drawGridInfo.height - drawGridInfo.y) / (drawGridInfo.yCount - 1);

    for (auto x = drawGridInfo.x; x <= drawGridInfo.width; x += xSpacing) {
        
        if (drawGridInfo.dotted) {
            DrawDottedLine(renderer, drawGridInfo.color, x, drawGridInfo.y, x, drawGridInfo.height, 10, 5);
        } else {
            SDL_RenderDrawLine(renderer, x, drawGridInfo.y, x, drawGridInfo.height); 
        }
    }

    for (auto y = drawGridInfo.y; y <= (drawGridInfo.height + drawGridInfo.y); y += ySpacing) {
        
        if (drawGridInfo.dotted) {
            DrawDottedLine(renderer, drawGridInfo.color, drawGridInfo.x, y, drawGridInfo.width, y, 10, 5);
        } else {
            SDL_RenderDrawLine(renderer, drawGridInfo.x, y, drawGridInfo.width, y);
        }
    }
}

#endif // PLOTUTILITY_H