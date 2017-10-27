// SDLPlotMain.cpp
#include <iostream>
#include <exception>
#include <cstring>

#include "PlotUtility.h"

#include "SDLPlot.h"
#include "SDL.h"
#include "SDL_ttf.h"

#undef main

const int backColor = 0xffafafaf; 

//---------------------------------------------------------------------------------------------------------------------------------------------------
//  SetupSDL
//---------------------------------------------------------------------------------------------------------------------------------------------------
struct SDLInfo {
    SDL_Renderer* renderer; 
    SDL_Window* window; 
    bool isError; 

    void CleanUp() {
        SDL_DestroyWindow(this->window);
        SDL_DestroyRenderer(this->renderer);

        TTF_Quit(); 

        SDL_Quit(); 
    }

    ~SDLInfo() {
        this->CleanUp(); 
    }
};

//---------------------------------------------------------------------------------------------------------------------------------------------------
// Name: SetupSDL
// Desc: 
//---------------------------------------------------------------------------------------------------------------------------------------------------
SDLInfo SetupSDL(int windowWidth, int windowHeight) {
    
    SDLInfo info; 
    
    // fix for when debugging in gdb causes app to crash
    SDL_SetHint(SDL_HINT_WINDOWS_DISABLE_THREAD_NAMING, "1");
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        
        info.isError = true; 
        return info;
    }

    info.window = SDL_CreateWindow("SDL Plot", 100, 100, windowWidth, windowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if (info.window == nullptr) {
        SDL_Quit();
        
        info.isError = true; 
        return info;
    }

    info.renderer = SDL_CreateRenderer(info.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);

    if (info.renderer == nullptr) {
        SDL_DestroyWindow(info.window);
        SDL_Quit();
        
        info.isError = true; 
        return info;
    } 

    // init SDL_ttf
	if (TTF_Init() == -1) {
		auto error = TTF_GetError(); 
		SDL_DestroyWindow(info.window); 
		SDL_DestroyRenderer(info.renderer); 
        SDL_Quit(); 
        
        info.isError = true; 
		return info; 
	}

    info.isError = false; 
    return info; 
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// Name: Update
// Desc:
//---------------------------------------------------------------------------------------------------------------------------------------------------
void Update(const SDLInfo& sdlInfo, std::vector<double>& plotData) {
    
    int windowWidth;
    int windowHeight; 
    
    SDL_SetRenderDrawColor(sdlInfo.renderer, 0x2f, 0x2f, 0x2f, 0xff);
    SDL_RenderClear(sdlInfo.renderer);

    SDL_GetWindowSize(sdlInfo.window, &windowWidth, &windowHeight); 

    auto texture = SDL_CreateTexture(sdlInfo.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, windowWidth, windowHeight);

    SDLPlotConfiguration config; 
    config.leftMargin = 50; 
    config.rightMargin = 50;
    config.topMargin = 50;
    config.bottomMargin = 50; 
    config.plotWidth = windowWidth;
    config.plotHeight = windowHeight; 

    SDLPlot plot(sdlInfo.renderer, texture, config); 
    plot.Draw(); 

    // detach render target
    SDL_SetRenderTarget(sdlInfo.renderer, nullptr); 

    // now render texture to the screen
    SDL_RenderCopyEx(sdlInfo.renderer, texture, nullptr, nullptr, 0, nullptr, SDL_FLIP_NONE); 

    SDL_Color color = {0x00, 0xff, 0x00, 0xff};
    plot.Plot(plotData, color); 

    SDL_RenderPresent(sdlInfo.renderer);
    SDL_DestroyTexture(texture);  
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// Name: main
// Desc:
//---------------------------------------------------------------------------------------------------------------------------------------------------
int main(int argc, char* argv[]) {

    int windowWidth = 640;
    int windowHeight = 480; 

    auto sdlInfo = SetupSDL(windowWidth, windowHeight);

    if (sdlInfo.isError) {
        return 0; 
    }

    auto plotData = GenerateRandomWalk(600, 0.8, 0.05, 0.1); 
    Update(sdlInfo, plotData); 

    // Main loop
    while(1) {

        SDL_Event event;
        
        if(SDL_PollEvent(&event)) {

            if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                Update(sdlInfo, plotData); 
            }

            if (event.type == SDL_QUIT) {
                break; 
            }
        }
    }
    
    return 0; 
}


