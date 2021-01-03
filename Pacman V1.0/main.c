
//Using SDL and standard IO
#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL_image.h>


//Declaring structures
struct coords
{
	int x, y;
};

//Screen dimension constants
const int SCREEN_WIDTH = 1024; //
const int SCREEN_HEIGHT = 768; //

//Declaring map
int** map;
//Starts up SDL and creates window
bool init();

//Frees media and shuts down SDL
void close();

//The window renderer will be rendering to
SDL_Window* window = NULL;

//The renderer to rendering to the window
SDL_Renderer* renderer = NULL;

//Pacman Textures
SDL_Texture* pacmanClosedLeft = NULL;
SDL_Texture* pacmanClosedRight = NULL;
SDL_Texture* pacmanClosedUp = NULL;
SDL_Texture* pacmanClosedDown = NULL;
SDL_Texture* pacmanOpenLeft = NULL;
SDL_Texture* pacmanOpenRight = NULL;
SDL_Texture* pacmanOpenUp = NULL;
SDL_Texture* pacmanOpenDown = NULL;

//Pacman postion
SDL_Rect pacmanPositionPixels = {15*32, 11*32, 32, 32};
struct coords pacmanPositionAtLogicMap = { 15,11 };


bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Create window
		window = SDL_CreateWindow("Pacman Copyright @2021 by Adam Barczyk", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (window == NULL)
		{
			printf("Windows could not be created SDL_Error: %s", SDL_GetError());
			success = false;
		}
		else
		{
			//Create renderer for window
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
			if (renderer == NULL)
			{
				printf("Renderer could not be created! SDL_Error: %s", SDL_GetError());
				success = false;
			}
		}
	}
	return success;
}

void close()
{
	//Destroy window
	SDL_DestroyWindow(window);
	window = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

int** generateMap() //TODO utworz oddzielna funkcje renderujaca, moze renderMap()?
{
	/*
	* Dimensions of elements on the screen
	* Each element dimensions: 32x32 px
	* Number of elements: 32x24 elements (32 elements OX, 24 elements OY)
	*
	* Mean of array values
	* 0 - it's a road
	* 1 - it's a wall
	*/

	//Dynamically allocating memory for logic map
	int** logicMap = calloc(24, sizeof(int*));
	for (int i = 0; i < 24; i++)
	{
		logicMap[i] = calloc(32, sizeof(int));
	}

	//int logicMapTmp[y][x] for easy setting of map design
	int logicMapTmp[24][32] = {
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,0,1,0,1,1,1,1,1,0,1,1,0,1,0,1,1,0,1,0,1,1,0,1,1,1,1,1,0,1,0,1},
		{1,0,1,0,1,1,1,1,1,0,1,1,0,0,0,1,1,0,0,0,1,1,0,1,1,1,1,1,0,1,0,1},
		{1,0,1,0,0,0,0,0,0,0,1,1,1,1,0,1,1,0,1,1,1,1,0,0,0,0,0,0,0,1,0,1},
		{1,0,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,0,1},
		{1,0,1,0,0,0,0,0,0,0,1,1,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0,0,0,1,0,1},
		{1,0,1,0,1,1,1,1,1,0,1,1,0,1,0,0,0,0,1,0,1,1,0,1,1,1,1,1,0,1,0,1},
		{1,0,0,0,0,0,1,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,1,0,0,0,0,0,1},
		{1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1},
		{1,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,1,0,1,0,1},
		{1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,0,1,0,1,0,1,0,1,0,1,0,1},
		{1,0,1,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,1},
		{1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,1,0,0,0,0,0,1},
		{1,0,1,1,1,1,1,0,1,1,1,1,0,1,0,0,0,0,1,0,1,0,1,1,1,1,1,1,1,1,0,1},
		{1,0,0,0,1,0,0,0,1,1,1,1,0,0,0,1,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1},
		{1,0,1,0,1,0,1,0,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,1,0,1},
		{1,0,1,0,0,0,1,0,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,1,0,1},
		{1,0,1,1,1,1,1,0,1,1,1,1,0,0,0,1,1,0,0,0,1,1,1,1,0,0,0,1,1,1,0,1},
		{1,0,1,1,1,1,1,0,1,1,1,1,0,1,0,1,1,0,1,0,1,1,1,1,0,1,1,1,1,1,0,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	};

	//Rewrite logicMapTmp to final logicMap allocated dynamically
	for (int x = 0; x < 32; x++) {
		for (int y = 0; y < 24; y++) {
			logicMap[y][x] = logicMapTmp[y][x];
		}
	}

	return logicMap;
}

void renderMap(int** logicMap)
{
	//Clear screen
	SDL_SetRenderDrawColor(renderer, 22, 22, 166, 255);
	SDL_RenderClear(renderer);

	//Set map look for renderer
	SDL_Rect rect;
	SDL_SetRenderDrawColor(renderer, 11, 11, 50, 255);
	for (int x = 0; x < 32; x++) {
		for (int y = 0; y < 24; y++) {
			if (logicMap[y][x] == 0) {
				rect.x = (x + 1) * 32 - 32;
				rect.y = (y + 1) * 32 - 32;
				rect.w = 32;
				rect.h = 32;
				SDL_RenderFillRect(renderer, &rect);
				//printf("[%d][%d][%d][%d]\n", rect.x, rect.y, rect.w, rect.h);
			}
		}
	}
	//Render map on the screen
	//SDL_RenderPresent(renderer);
}

SDL_Texture* loadPacman(char* path)
{
	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path);
	if (loadedSurface == NULL)
	{
		printf("\nImage could not be loaded in loadPacman function! Source: %s SDL_image Error: %s", path, IMG_GetError());
	}
	else
	{
		//Create texture from loadedSurface pixels
		newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("\nTexture could not be created in loadPacman function! SourceFile: %s SDL Error: %s", path, SDL_GetError());
		}

		//free loadedSurface
		SDL_FreeSurface(loadedSurface);
	}

	return newTexture;
}

bool loadMedia()
{
	//Flag depending on success or failure to load
	bool success = true;

	//Loading pacman with open mouth turned to right
	pacmanClosedLeft = loadPacman("pacman_closed_left.png");
	pacmanClosedRight = loadPacman("pacman_closed_right.png");
	pacmanClosedUp = loadPacman("pacman_closed_up.png");
	pacmanClosedDown = loadPacman("pacman_closed_down.png");
	pacmanOpenLeft = loadPacman("pacman_open_left.png");
	pacmanOpenRight = loadPacman("pacman_open_right.png");
	pacmanOpenUp = loadPacman("pacman_open_up.png");
	pacmanOpenDown = loadPacman("pacman_open_down.png");

	SDL_Texture* pacmanTextures[] = 
	{
		pacmanClosedLeft,
		pacmanClosedRight,
		pacmanClosedUp,
		pacmanClosedDown,
		pacmanOpenLeft,
		pacmanOpenRight,
		pacmanOpenUp,
		pacmanOpenDown
	};

	for (int i = 0; i < 8; i++)
	{
		if (pacmanTextures[i] == NULL)
		{
			printf("\npacman texture could not be loaded in loadMedia function! SDL_Error: %s", SDL_GetError());
			success = false;
		}
	}

	return success;
}

void freeMap(int** map)
{
	for (int i = 0; i < 24; i++)
	{
		free(map[i]);
	}
	free(map);
}

void moveLeft() //TODO utworz oddzielna funkcje renderujaca, ¿eby mozna bylo renderowac bez zmiany polozenia, moze renderPacman()?
{
	for (int i = 0; i < 8; i++)
	{
		pacmanPositionPixels.x = pacmanPositionPixels.x - 4;
		pacmanPositionPixels.y = pacmanPositionPixels.y;
		pacmanPositionPixels.w = pacmanPositionPixels.w;
		pacmanPositionPixels.h = pacmanPositionPixels.h;

		int** x = generateMap();
		renderMap(x);
		freeMap(x);
		
		SDL_RenderCopy(renderer, pacmanClosedLeft, NULL, &pacmanPositionPixels);
		SDL_RenderPresent(renderer);
		SDL_Delay(15); //potrzebne?
	}
}

void moveRight() //TODO utworz oddzielna funkcje renderujaca, ¿eby mozna bylo renderowac bez zmiany polozenia, moze renderPacman()?
{
	for (int i = 0; i < 8; i++)
	{
		pacmanPositionPixels.x = pacmanPositionPixels.x + 4;
		pacmanPositionPixels.y = pacmanPositionPixels.y;
		pacmanPositionPixels.w = pacmanPositionPixels.w;
		pacmanPositionPixels.h = pacmanPositionPixels.h;

		int** x = generateMap();
		renderMap(x);
		freeMap(x);

		SDL_RenderCopy(renderer, pacmanClosedRight, NULL, &pacmanPositionPixels);
		SDL_RenderPresent(renderer);
	}
}

int main(int argc, char* args[])
{
	if (!init())
	{
		printf("\nFailed to initialize!");
	}
	else
	{

		bool moveLeftFlag = true;
		bool moveRightFlag = false;
		//Load actor's textures
		loadMedia();

		bool running = true;
		SDL_Event e;

		//Set pacman position on the screen
		SDL_Rect pacman1Position = { 100, 100, 32, 32 };
		SDL_Rect pacman2Position = { 150, 100, 32, 32 };
		SDL_Rect pacman3Position = { 200, 100, 32, 32 };
		SDL_Rect pacman4Position = { 250, 100, 32, 32 };
		SDL_Rect pacman5Position = { 300, 100, 32, 32 };
		SDL_Rect pacman6Position = { 350, 100, 32, 32 };
		SDL_Rect pacman7Position = { 400, 100, 32, 32 };
		SDL_Rect pacman8Position = { 450, 100, 32, 32 };

		//While application is running
		while (running)
		{
			while (SDL_PollEvent(&e) != 0)
			{
				//User requests quit the application
				if (e.type == SDL_QUIT)
				{
					running = false;
				}
			}
			map = generateMap();
			
			if (moveLeftFlag)
			{
				if (map[pacmanPositionAtLogicMap.y][pacmanPositionAtLogicMap.x - 1] == 0) //TODO cialo if'a
				{
					moveLeft();
					//pacmanPositionAtLogicMap.x = pacmanPositionAtLogicMap.x - 1;
					//pacmanPositionAtLogicMap.y = pacmanPositionAtLogicMap.y;
					printf("\n%d | %d --> %d", pacmanPositionAtLogicMap.x, pacmanPositionAtLogicMap.y, map[pacmanPositionAtLogicMap.y][pacmanPositionAtLogicMap.x]);
				}
				if (pacmanPositionPixels.x < 4) { moveLeftFlag = false; moveRightFlag = true; }
			}
			else if (moveRightFlag)
			{
				moveRight();
				if (pacmanPositionPixels.x > 1020) { moveLeftFlag = true; moveRightFlag = false; }
			}

			//rendering pacman on the screen
			//SDL_RenderCopy(renderer, pacmanClosedLeft, NULL, &pacmanPosition);
			/*SDL_RenderCopy(renderer, pacmanClosedRight, NULL, &pacman2Position);
			SDL_RenderCopy(renderer, pacmanClosedUp, NULL, &pacman3Position);
			SDL_RenderCopy(renderer, pacmanClosedDown, NULL, &pacman4Position);
			SDL_RenderCopy(renderer, pacmanOpenLeft, NULL, &pacman5Position);
			SDL_RenderCopy(renderer, pacmanOpenRight, NULL, &pacman6Position);
			SDL_RenderCopy(renderer, pacmanOpenUp, NULL, &pacman7Position);
			SDL_RenderCopy(renderer, pacmanOpenDown, NULL, &pacman8Position);*/

			//SDL_RenderCopy(renderer, pacman, NULL, NULL);
			SDL_RenderPresent(renderer);

			freeMap(map);
		}
	}

	//Close SDL
	close();
	return 0;
}