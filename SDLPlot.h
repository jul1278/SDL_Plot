#ifndef SDLPLOT_H
#define SDLPLOT_H

// ISDLPlot
#include <memory>
#include <vector>
#include <list>
#include <tuple>

#include "PlotUtility.h"
#include "SDL.h"
#include "SDL_ttf.h"


//----------------------------------------------------------------------------------------------------------------------
// Name: SDLPlotConfiguration
// Desc:
//----------------------------------------------------------------------------------------------------------------------
struct SDLPlotConfiguration {
    bool grid; 

    float gridSpacingX; 
    float gridSpacingY; 

    int xMinLimitLeft; 
    int xMaxLimitLeft;
    
    int yMinLimitLeft;
    int yMaxLimitLeft; 
    
    int xMinLimitRight; 
    int xMaxLimitRight;
    
    int yMinLimitRight;
    int yMaxLimitRight; 

    int plotWidth;
    int plotHeight; 

    int axisThiccness; 

    float topMargin;
    float bottomMargin; 

    float leftMargin;
    float rightMargin; 
    
    std::string plotTitle; 

    std::string xTitleLeft;
    std::string xTitleRight;

    std::string yTitle;

    std::shared_ptr<std::vector<int>> xData; 
    std::shared_ptr<std::vector<int>> yData;

    SDLPlotConfiguration() {
        // TODO: set defaults here
    }
}; 

//----------------------------------------------------------------------------------------------------------------------
// Name: SDLPlotConfiguration
// Desc:
//----------------------------------------------------------------------------------------------------------------------
class SDLPlot {

    SDL_Renderer* renderer; 
    SDL_Texture* texture;

    TTF_Font* font; 

    SDL_Texture* titleTextTexture;
    SDL_Texture* xAxisTextTexture;
    SDL_Texture* leftYAxisTextTexture;
    SDL_Texture* rightYAxisTextTexture;


    struct Series {
        std::vector<int> xData;  
        std::vector<int> yData;

        uint32_t color; 
    }; 

    std::list<Series> dataSeries;
     
    SDLPlotConfiguration plotConfiguration; 

public:

    //------------------------------------------------------------------------------------------------------------------
    // Name:
    // Desc:
    //------------------------------------------------------------------------------------------------------------------
    SDLPlot(SDL_Renderer* renderer, SDL_Texture* texture, const SDLPlotConfiguration& configuration) 
        : renderer(renderer), texture(texture), plotConfiguration(configuration) 
    {
        this->font = TTF_OpenFont("OxygenMono-Regular.ttf", 30); 

        SDL_Color color = {0xff, 0xff, 0xff, 0xff};
        SDL_Surface* textSurface = TTF_RenderText_Solid(this->font, "Plot Title", color); 
        this->titleTextTexture = SDL_CreateTextureFromSurface(this->renderer, textSurface); 
    }

    ~SDLPlot() {}

    //------------------------------------------------------------------------------------------------------------------
    // Name:
    // Desc:
    //------------------------------------------------------------------------------------------------------------------
    void Draw() {

        if (!this->ValidateConfig()) {
            return;
        }

        SDL_SetRenderTarget(this->renderer, this->texture); 
        SDL_RenderClear(this->renderer); 

        // draw grid
        DrawGridInfo gridInfo;
        gridInfo.color = 0x4f4f4fff;  
        gridInfo.x = this->plotConfiguration.leftMargin;
        gridInfo.y = this->plotConfiguration.topMargin; 
        gridInfo.width = this->plotConfiguration.plotWidth - this->plotConfiguration.rightMargin; 
        gridInfo.height = this->plotConfiguration.plotHeight - this->plotConfiguration.bottomMargin;
        gridInfo.xCount = 12; 
        gridInfo.yCount = 12;
        gridInfo.dotted = false; 

        DrawGrid(this->renderer, gridInfo); 

        SDL_SetRenderDrawColor(this->renderer, 0xFF, 0xFF, 0xFF, 0xFF); 

        // draw y axis
        auto y2 = this->plotConfiguration.plotHeight - this->plotConfiguration.topMargin; 
        SDL_RenderDrawLine(this->renderer, this->plotConfiguration.leftMargin, this->plotConfiguration.topMargin, this->plotConfiguration.leftMargin, y2); 

        // draw x axis 
        auto x2 = this->plotConfiguration.plotWidth - this->plotConfiguration.rightMargin; 
        auto y = this->plotConfiguration.plotHeight - this->plotConfiguration.bottomMargin; 
        SDL_RenderDrawLine(this->renderer, this->plotConfiguration.leftMargin, y, x2, y); 

        int tw, th; 
        SDL_QueryTexture(this->titleTextTexture, NULL, NULL, &tw, &th); 

        auto textHeight = 0.8f * this->plotConfiguration.topMargin; 
        auto change = 1.0f - ((float) (th - textHeight) / th); 

        auto textWidth = change * tw; //(this->plotConfiguration.plotWidth < 200) ? 0.33f * this->plotConfiguration.plotWidth : 200; 

        SDL_Rect textRect; 
        textRect.x = (this->plotConfiguration.plotWidth - textWidth) / 2;
        textRect.y = (this->plotConfiguration.topMargin - textHeight) / 2; 
        textRect.w = textWidth; 
        textRect.h = textHeight;  
        
        // draw titles 
        SDL_RenderCopy(this->renderer, this->titleTextTexture, nullptr, &textRect); 

        // draw data onto plot

        // TODO: draw annotation/captions etc

        SDL_RenderCopy(this->renderer, this->texture, nullptr, nullptr); 
    }

    //-------------------------------------------------------------------------------------------------------------------
    // Name:
    // Desc:
    //-------------------------------------------------------------------------------------------------------------------
    void Plot(const std::vector<int>& xData, const std::vector<int>& yData) {

    }

private:

    //------------------------------------------------------------------------------------------------------------------
    // Name:
    // Desc:
    //------------------------------------------------------------------------------------------------------------------
    bool ValidateConfig() {
        
        // TODO: validate config info
        if (this->texture == nullptr) {
            return false; 
        }

        if (this->renderer == nullptr) {
            return false; 
        } 

        return true; 
    }

    //------------------------------------------------------------------------------------------------------------------
    // Name:
    // Desc:
    //------------------------------------------------------------------------------------------------------------------
    void PlotData() {
        for (auto series : this->dataSeries) {

        }
    }
};


#endif // SDLPLOT_H