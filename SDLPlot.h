#ifndef SDLPLOT_H
#define SDLPLOT_H

// ISDLPlot
#include <memory>
#include <vector>
#include <list>
#include <tuple>

#include "PlotUtility.h"
#include "SDL.h"

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
        : renderer(renderer), texture(texture), plotConfiguration(configuration) {}

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
        gridInfo.xCount = 3; 
        gridInfo.yCount = 3;
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