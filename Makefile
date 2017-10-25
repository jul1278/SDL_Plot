#OBJS specifies which files to compile as part of the project
OBJS = SDLPlotMain.cpp

#CC specifies which compiler we're using
CC = g++

#INCLUDE_PATHS specifies the additional include paths we'll need
INCLUDE_PATHS = -I../lib/SDL2-2.0.6/i686-w64-mingw32/include/SDL2/ -I../lib/SDL2_ttf-2.0.14/i686-w64-mingw32/include/SDL2/

#LIBRARY_PATHS specifies the additional library paths we'll need
LIBRARY_PATHS = -L../lib/SDL2-2.0.6/i686-w64-mingw32/lib/ -L../lib/SDL2_ttf-2.0.14/i686-w64-mingw32/bin/

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
# -Wl,-subsystem,windows gets rid of the console window
#COMPILER_FLAGS = -w -g -Wl,-subsystem,windows
COMPILER_FLAGS = -w -g -mwindows

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = SDLPlot

#This is the target that compiles our executable
all : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)