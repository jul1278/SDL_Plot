#ifndef SDLPLOT_H
#define SDLPLOT_H

// ISDLPlot
#include <memory>
#include <vector>
#include <list>
#include <tuple>
#include <unordered_map>
#include <algorithm>

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

    typedef std::unique_ptr<SDL_Texture, std::function<void(SDL_Texture*)>> sdl_texture_ptr; 

    sdl_texture_ptr titleTextTexture;
    sdl_texture_ptr xAxisTextTexture;
    sdl_texture_ptr leftYAxisTextTexture;
    sdl_texture_ptr rightYAxisTextTexture;

    DrawGridInfo gridInfo; 

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
        SDL_Color color = {0xff, 0xff, 0xff, 0xff};
        this->titleTextTexture = RenderTextToTexture(this->renderer, "OxygenMono-Regular.ttf", 30, "Plot Title", color); 
        this->leftYAxisTextTexture = RenderTextToTexture(this->renderer, "OxygenMono-Regular.ttf", 15, "Left Y Axis", color); 
        this->xAxisTextTexture = RenderTextToTexture(this->renderer, "OxygenMono-Regular.ttf", 15, "X Axis", color); 
        
        this->gridInfo.color = 0x4f4f4fff;  
        this->gridInfo.x = this->plotConfiguration.leftMargin;
        this->gridInfo.y = this->plotConfiguration.topMargin; 
        this->gridInfo.width = this->plotConfiguration.plotWidth - this->plotConfiguration.rightMargin; 
        this->gridInfo.height = this->plotConfiguration.plotHeight - this->plotConfiguration.bottomMargin;
        this->gridInfo.xCount = 12; 
        this->gridInfo.yCount = 12;
        this->gridInfo.dotted = false; 
    }

    //------------------------------------------------------------------------------------------------------------------
    // Name: ~SDLPlot
    // Desc:
    //------------------------------------------------------------------------------------------------------------------
    ~SDLPlot() 
    {
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

        DrawGrid(this->renderer, this->gridInfo); 

        SDL_SetRenderDrawColor(this->renderer, 0xFF, 0xFF, 0xFF, 0xFF); 

        // draw y axis
        auto y2 = this->plotConfiguration.plotHeight - this->plotConfiguration.topMargin; 
        SDL_RenderDrawLine(this->renderer, this->plotConfiguration.leftMargin, this->plotConfiguration.topMargin, this->plotConfiguration.leftMargin, y2); 

        // draw x axis 
        auto x2 = this->plotConfiguration.plotWidth - this->plotConfiguration.rightMargin; 
        auto y = this->plotConfiguration.plotHeight - this->plotConfiguration.bottomMargin; 
        SDL_RenderDrawLine(this->renderer, this->plotConfiguration.leftMargin, y, x2, y); 

        // draw spikes on axis
        this->DrawAxisIncrements(this->gridInfo);

        // draw titles
        this->DrawTitles();
        
        // TODO: draw annotation/captions etc

        SDL_RenderCopy(this->renderer, this->texture, nullptr, nullptr); 
    }

    //-------------------------------------------------------------------------------------------------------------------
    // Name: Plot
    // Desc:
    //-------------------------------------------------------------------------------------------------------------------
    void Plot(const std::vector<double>& yData, SDL_Color color) {
        auto min = std::min_element(yData.begin(), yData.end());
        auto max = std::max_element(yData.begin(), yData.end());
        
        auto diff = *max - *min; 
        
        auto plotAreaHeight = this->plotConfiguration.plotHeight - this->plotConfiguration.bottomMargin - this->plotConfiguration.topMargin; 
        auto plotAreaWidth = this->plotConfiguration.plotWidth - this->plotConfiguration.leftMargin - this->plotConfiguration.rightMargin; 

        auto yDataScaled = yData; 

        std::transform(yDataScaled.begin(), yDataScaled.end(), yDataScaled.begin(), 
            [plotAreaHeight, min, max] 
            (float y) 
            { 
                return (double) plotAreaHeight * ((y - *min) / (*max - *min) ); 
            } 
        ); 

        // so far assuming that there are more pixels than data points; 
        // how to handle opposite case? it actually seems to work okay when you scale it really small so idk
        auto xSpace = plotAreaWidth / yDataScaled.size(); 

        SDL_SetRenderDrawColor(this->renderer, color.r, color.g, color.b, color.a); 

        auto yFlipTransform = this->plotConfiguration.plotHeight - this->plotConfiguration.bottomMargin;

        for (auto i = 1; i < yDataScaled.size(); i++) {

            auto x1 = (i-1) * xSpace; 
            auto y1 = yFlipTransform - (int) yDataScaled[i - 1];

            auto x2 = i * xSpace; 
            auto y2 = yFlipTransform - (int) yDataScaled[i];

            SDL_RenderDrawLine(this->renderer, x1 + this->plotConfiguration.leftMargin, y1, x2 + this->plotConfiguration.leftMargin, y2); 
        }
    }

private:

    //------------------------------------------------------------------------------------------------------------------
    // Name: DrawTitles
    // Desc:
    //------------------------------------------------------------------------------------------------------------------
    bool DrawTitles() {

        // TODO: factor out some functions here

        int tw, th; 
        SDL_QueryTexture(this->titleTextTexture.get(), NULL, NULL, &tw, &th); 

        auto textHeight = 0.8f * this->plotConfiguration.topMargin; 
        auto change = (th != 0) ? 1.0f - ((float) (th - textHeight) / th) : 1.0f; 

        auto textWidth = change * tw; 

        SDL_Rect textRect; 
        textRect.x = (this->plotConfiguration.plotWidth - textWidth) / 2;
        textRect.y = (this->plotConfiguration.topMargin - textHeight) / 2; 
        textRect.w = tw; 
        textRect.h = th;  
        
        // draw titles 
        SDL_RenderCopy(this->renderer, this->titleTextTexture.get(), nullptr, &textRect); 

        // x axis
        SDL_QueryTexture(this->xAxisTextTexture.get(), NULL, NULL, &tw, &th); 

        textHeight = 0.4f * this->plotConfiguration.bottomMargin; 
        change = (th != 0) ? 1.0f - ((float) (th - textHeight) / th) : 1.0f; 

        textWidth = change * tw; 
 
        textRect.x = (this->plotConfiguration.plotWidth - textWidth) / 2;
        textRect.y = this->plotConfiguration.plotHeight - this->plotConfiguration.bottomMargin + (textHeight / 2); 
        textRect.w = textWidth; 
        textRect.h = textHeight; 

        // draw titles 
        SDL_RenderCopy(this->renderer, this->xAxisTextTexture.get(), nullptr, &textRect); 

        // left y axis
        SDL_QueryTexture(this->leftYAxisTextTexture.get(), NULL, NULL, &tw, &th); 
        
        textHeight = 0.4f * this->plotConfiguration.leftMargin; 
        change = (th != 0) ? 1.0f - ((float) (th - textHeight) / th) : 1.0f; 

        textWidth = change * tw;

        textRect.x = (this->plotConfiguration.leftMargin - textWidth) / 2;
        textRect.y = (this->plotConfiguration.plotHeight - textHeight) / 2; 
        textRect.w = textWidth; 
        textRect.h = textHeight; 

        SDL_Point center = {this->plotConfiguration.leftMargin / 2, this->plotConfiguration.plotHeight / 2}; 

        // draw titles 
        SDL_RenderCopyEx(this->renderer, this->leftYAxisTextTexture.get(), nullptr, &textRect, -90.0f, nullptr, SDL_FLIP_NONE); 
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
};


#endif // SDLPLOT_H