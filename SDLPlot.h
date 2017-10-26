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
        
        font = TTF_OpenFont("OxygenMono-Regular.ttf", 15);

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

        // draw spikes on axis
        this->DrawAxisIncrements(gridInfo);

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

        // TODO: factor out some functions here

        int tw, th; 
        SDL_QueryTexture(this->titleTextTexture, NULL, NULL, &tw, &th); 

        auto textHeight = 0.8f * this->plotConfiguration.topMargin; 
        auto change = (th != 0) ? 1.0f - ((float) (th - textHeight) / th) : 1.0f; 

        auto textWidth = change * tw; 

        SDL_Rect textRect; 
        textRect.x = (this->plotConfiguration.plotWidth - textWidth) / 2;
        textRect.y = (this->plotConfiguration.topMargin - textHeight) / 2; 
        textRect.w = tw;//textWidth; 
        textRect.h = th;//textHeight;  
        
        // draw titles 
        SDL_RenderCopy(this->renderer, this->titleTextTexture, nullptr, &textRect); 

        // x axis
        SDL_QueryTexture(this->xAxisTextTexture, NULL, NULL, &tw, &th); 

        textHeight = 0.4f * this->plotConfiguration.bottomMargin; 
        change = (th != 0) ? 1.0f - ((float) (th - textHeight) / th) : 1.0f; 

        textWidth = change * tw; 
 
        textRect.x = (this->plotConfiguration.plotWidth - textWidth) / 2;
        textRect.y = this->plotConfiguration.plotHeight - this->plotConfiguration.bottomMargin + (textHeight / 2); 
        textRect.w = textWidth; 
        textRect.h = textHeight; 

        // draw titles 
        SDL_RenderCopy(this->renderer, this->xAxisTextTexture, nullptr, &textRect); 

        // left y axis
        SDL_QueryTexture(this->leftYAxisTextTexture, NULL, NULL, &tw, &th); 
        
        textHeight = 0.4f * this->plotConfiguration.leftMargin; 
        change = (th != 0) ? 1.0f - ((float) (th - textHeight) / th) : 1.0f; 

        textWidth = change * tw;

        textRect.x = (this->plotConfiguration.leftMargin - textWidth) / 2;
        textRect.y = (this->plotConfiguration.plotHeight - textHeight) / 2; 
        textRect.w = textWidth; 
        textRect.h = textHeight; 

        SDL_Point center = {this->plotConfiguration.leftMargin / 2, this->plotConfiguration.plotHeight / 2}; 

        // draw titles 
        SDL_RenderCopyEx(this->renderer, this->leftYAxisTextTexture, nullptr, &textRect, -90.0f, nullptr, SDL_FLIP_NONE); 
    }

    //------------------------------------------------------------------------------------------------------------------
    // Name: DrawAxisIncrements
    // Desc:
    //------------------------------------------------------------------------------------------------------------------
    void DrawAxisIncrements(const DrawGridInfo& drawGridInfo) {
        
        SDL_SetRenderDrawColor(this->renderer, 0xFF, 0xFF, 0xFF, 0xFF);         
        
        auto fy = [] (const DrawIntervalInfo& info) 
        {
            auto x1 = info.x;
            auto y1 = info.y; 

            SDL_RenderDrawLine(info.renderer, x1, y1, x1 + 5, y1);
        };

        auto fx = [] (const DrawIntervalInfo& info) 
        {
            auto x1 = info.x;
            auto y1 = info.y; 

            SDL_RenderDrawLine(info.renderer, x1, y1, x1, y1 - 5);
        };

        DrawOnRepeatingInterval(this->renderer, 
            this->plotConfiguration.leftMargin, this->plotConfiguration.topMargin, 
            this->plotConfiguration.leftMargin, this->plotConfiguration.plotHeight - this->plotConfiguration.bottomMargin,
            drawGridInfo.yCount + 1, 0.0, false, true, fy); 

        DrawOnRepeatingInterval(this->renderer, 
            this->plotConfiguration.leftMargin,  // x1
            this->plotConfiguration.plotHeight - this->plotConfiguration.bottomMargin,  // y1
            this->plotConfiguration.plotWidth - this->plotConfiguration.rightMargin,  // x2
            this->plotConfiguration.plotHeight - this->plotConfiguration.bottomMargin, // y2
            
            drawGridInfo.xCount + 1, 0.0, false, true, fx); 
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