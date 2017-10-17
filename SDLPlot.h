// ISDLPlot

#include "SDL.h"

struct SDLPlotConfiguration {
    bool grid; 

    unsigned int gridSpacingX; 
    unsigned int gridSpacingY; 

    std::shared_ptr<std::vector<int>> xData; 
    std::shared_ptr<std::vector<int>> yData;

    int xMinLimit; 
    int xMaxLimit;
    
    int yMinLimit;
    int yMaxLimit; 
}; 

// ISDLPlot
class ISDLPlot {

    SDL_Renderer* renderer; 
    SDLPlotConfiguration plotConfiguration; 

public:

    ISDLPlot(SDL_Renderer* renderer, const SDLPlotConfiguration& configuration) 
        : renderer(renderer), plotConfiguration(configuration)
    {

    }

    ~ISDLPlot() {}

    void Draw() {}
};


