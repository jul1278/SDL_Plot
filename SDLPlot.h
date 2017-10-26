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
    // Name: SDLPlot
    // Desc:
    //------------------------------------------------------------------------------------------------------------------
    SDLPlot(SDL_Renderer* renderer, SDL_Texture* texture, const SDLPlotConfiguration& configuration) 
        : renderer(renderer), texture(texture), plotConfiguration(configuration) 
    {
        auto font = TTF_OpenFont("OxygenMono-Regular.ttf", 30); 

        SDL_Color color = {0xff, 0xff, 0xff, 0xff};
        auto textSurface = TTF_RenderText_Solid(font, "Plot Title", color); 
        this->titleTextTexture = SDL_CreateTextureFromSurface(this->renderer, textSurface); 
        
        SDL_FreeSurface(textSurface);
        TTF_CloseFont(font);
        
        font = TTF_OpenFont("OxygenMono-Regular.ttf", 40);

        textSurface = TTF_RenderText_Solid(font, "Y Axis", color); 
        this->leftYAxisTextTexture = SDL_CreateTextureFromSurface(this->renderer, textSurface);
        SDL_FreeSurface(textSurface);

        textSurface = TTF_RenderText_Solid(font, "X Axis", color); 
        this->xAxisTextTexture = SDL_CreateTextureFromSurface(this->renderer, textSurface);
        
        TTF_CloseFont(font); 
        SDL_FreeSurface(textSurface);
    }

    //------------------------------------------------------------------------------------------------------------------
    // Name: ~SDLPlot
    // Desc:
    //------------------------------------------------------------------------------------------------------------------
    ~SDLPlot() 
    {
        SDL_DestroyTexture(this->titleTextTexture);
        SDL_DestroyTexture(this->leftYAxisTextTexture);
        SDL_DestroyTexture(this->rightYAxisTextTexture);
        SDL_DestroyTexture(this->texture);
    }

    //------------------------------------------------------------------------------------------------------------------
    // Name: Draw
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

        // draw titles
        this->DrawTitles();
        
        // TODO: draw annotation/captions etc

        SDL_RenderCopy(this->renderer, this->texture, nullptr, nullptr); 
    }

    //-------------------------------------------------------------------------------------------------------------------
    // Name: Plot
    // Desc:
    //-------------------------------------------------------------------------------------------------------------------
    void Plot(const std::vector<int>& xData, const std::vector<int>& yData) {

    }

private:
    //------------------------------------------------------------------------------------------------------------------
    // Name: DrawTitles
    // Desc:
    //------------------------------------------------------------------------------------------------------------------
    bool DrawTitles() {
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

        // x axis
        SDL_QueryTexture(this->xAxisTextTexture, NULL, NULL, &tw, &th); 

        textHeight = 0.4f * this->plotConfiguration.bottomMargin; 
        change = 1.0f - ((float) (th - textHeight) / th); 

        textWidth = change * tw; //(this->plotConfiguration.plotWidth < 200) ? 0.33f * this->plotConfiguration.plotWidth : 200; 

        textRect; 
        textRect.x = (this->plotConfiguration.plotWidth - textWidth) / 2;
        textRect.y = this->plotConfiguration.plotHeight - this->plotConfiguration.bottomMargin + (textHeight / 2); 
        textRect.w = textWidth; 
        textRect.h = textHeight; 

        // draw titles 
        SDL_RenderCopy(this->renderer, this->xAxisTextTexture, nullptr, &textRect); 

        // left y axis
        SDL_QueryTexture(this->leftYAxisTextTexture, NULL, NULL, &tw, &th); 
        
        textHeight = 0.4f * this->plotConfiguration.leftMargin; 
        change = 1.0f - ((float) (th - textHeight) / th); 

        textWidth = change * tw; //(this->plotConfiguration.plotWidth < 200) ? 0.33f * this->plotConfiguration.plotWidth : 200; 

        textRect; 
        textRect.x = (this->plotConfiguration.leftMargin - textWidth) / 2;
        textRect.y = (this->plotConfiguration.plotHeight - textHeight) / 2; 
        textRect.w = textWidth; 
        textRect.h = textHeight; 

        SDL_Point center = {this->plotConfiguration.leftMargin / 2, this->plotConfiguration.plotHeight / 2}; 

        // draw titles 
        SDL_RenderCopyEx(this->renderer, this->leftYAxisTextTexture, nullptr, &textRect, -90.0f, nullptr, SDL_FLIP_NONE); 
    }

    //------------------------------------------------------------------------------------------------------------------
    // Name: ValidateConfig
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
    }
};


#endif // SDLPLOT_H