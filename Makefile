SDL_PATH = C:/Software/SDL2/x86_64-w64-mingw32
INCLUDE_PATHS = -I include -I $(SDL_PATH)/include
LIB_PATHS = -L $(SDL_PATH)/lib
LIBS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lsqlite3
C_FLAGS = -Wall

all:
	gcc src/*.c $(C_FLAGS) $(INCLUDE_PATHS) $(LIB_PATHS) -o bin/main $(LIBS)