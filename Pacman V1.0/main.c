//Using SDL and standard IO
#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL_image.h>
#include <string.h>


//Declaring structures
struct coords
{
	int x, y;
};

//Screen dimension constants
const int SCREEN_WIDTH = 1024; //
const int SCREEN_HEIGHT = 768; //

//Declaring map
int** logicMap;
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

//Ghosts textures
SDL_Texture* purpleGhost = NULL;
SDL_Texture* brownGhost = NULL;
SDL_Texture* greenGhost = NULL;
SDL_Texture* yellowGhost = NULL;

//Pacman position
SDL_Rect pacmanPositionPixels = {15*32, 11*32, 32, 32};
struct coords pacmanPositionAtLogicMap = { 15,11 };

//Pacman movement direction
char pacmanMovementDirection[6] = "RIGHT";

//Ghosts position
SDL_Rect purpleGhostPositionPixels = {1*32, 2*32, 32, 32};
struct coords purpleGhostPositionAtLogicMap = {1, 2};
SDL_Rect brownGhostPositionPixels = {30*32, 2*32, 32, 32};
struct coords brownGhostPositionAtLogicMap = { 30, 2 };
SDL_Rect greenGhostPositionPixels = {1*32, 22*32, 32, 32};
struct coords greenGhostPositionAtLogicMap = { 1, 22 };
SDL_Rect yellowGhostPositionPixels = {30*32, 22*32, 32, 32};
struct coords yellowGhostPositionAtLogicMap = { 30, 22 };


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

int** generateMap()
{
	/*
	* Dimensions of elements on the screen
	* Each element dimensions: 32x32 px
	* Number of elements: 32x24 elements (32 elements OX, 24 elements OY)
	*
	* Mean of array values
	* 0 - it's a road
	* 1 - it's a wall
	* 2 - purple ghost
	* 3 - brown ghost
	* 4 - green ghost
	* 5 - yellow ghost
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
		{1,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,1},
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
		{1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,1},
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
			if (logicMap[y][x] != 1) {
				rect.x = (x + 1) * 32 - 32;
				rect.y = (y + 1) * 32 - 32;
				rect.w = 32;                                                                    //Dlaczego trzeba za kazdym razem ustawiac wartosc w i h, skoro sie nie zmieniaja. Rect to struktura(?)
				rect.h = 32;
				SDL_RenderFillRect(renderer, &rect);
			}
		}
	}
	//Render map on the screen
	//SDL_RenderPresent(renderer);
}

SDL_Texture* loadTexture(char* path)
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

	//Loading pacman textures
	pacmanClosedLeft = loadTexture("pacman_closed_left.png");
	pacmanClosedRight = loadTexture("pacman_closed_right.png");
	pacmanClosedUp = loadTexture("pacman_closed_up.png");
	pacmanClosedDown = loadTexture("pacman_closed_down.png");
	pacmanOpenLeft = loadTexture("pacman_open_left.png");
	pacmanOpenRight = loadTexture("pacman_open_right.png");
	pacmanOpenUp = loadTexture("pacman_open_up.png");
	pacmanOpenDown = loadTexture("pacman_open_down.png");

	//Loading ghosts textures
	purpleGhost = loadTexture("purple_ghost.png");
	brownGhost = loadTexture("brown_ghost.png");
	greenGhost = loadTexture("green_ghost.png");
	yellowGhost = loadTexture("yellow_ghost.png");

	SDL_Texture* allTextures[] = 
	{
		pacmanClosedLeft,
		pacmanClosedRight,
		pacmanClosedUp,
		pacmanClosedDown,
		pacmanOpenLeft,
		pacmanOpenRight,
		pacmanOpenUp,
		pacmanOpenDown,
		purpleGhost,
		brownGhost,
		greenGhost,
		yellowGhost
	};

	for (int i = 0; i < 8; i++)
	{
		if (allTextures[i] == NULL)
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

void moveLeft(int** logicMap, SDL_Texture* texture, SDL_Rect* arrayPositionPixels)
{
	for (int i = 0; i < 8; i++)
	{
		arrayPositionPixels->x = arrayPositionPixels->x - 4;
		arrayPositionPixels->y = arrayPositionPixels->y;
		arrayPositionPixels->w = arrayPositionPixels->w;
		arrayPositionPixels->h = arrayPositionPixels->h;

		//Render map based on logic map
		renderMap(logicMap);

		//Render texture on the map at position specified in arrayPositionPixels
		SDL_RenderCopy(renderer, texture, NULL, arrayPositionPixels);
		SDL_RenderPresent(renderer);
	}
}

//void moveRight(int** logicMap, SDL_Texture* texture, SDL_Rect* arrayPositionPixels)
//{
//	for (int i = 0; i < 8; i++)
//	{
//		arrayPositionPixels->x = arrayPositionPixels->x + 4;
//		arrayPositionPixels->y = arrayPositionPixels->y;
//		arrayPositionPixels->w = arrayPositionPixels->w;
//		arrayPositionPixels->h = arrayPositionPixels->h;
//
//		//Render map based on logic map
//		renderMap(logicMap);
//
//		//Render texture on the map at position specified in arrayPositionPixels
//		SDL_RenderCopy(renderer, texture, NULL, arrayPositionPixels);
//		SDL_RenderPresent(renderer);
//	}
//}

SDL_Texture* getPacmanDirection()
{

}

void moveRight(int** logicMap, SDL_Texture* texture, SDL_Rect* arrayPositionPixels)
{
	SDL_Texture* pacmanTmp = pacmanOpenRight;
	if (strcmp(pacmanMovementDirection, "RIGHT") == 0) { pacmanTmp = pacmanOpenRight; }
	else if (strcmp(pacmanMovementDirection, "LEFT") == 0) { pacmanTmp = pacmanOpenLeft; }
	else if (strcmp(pacmanMovementDirection, "UP") == 0) { pacmanTmp = pacmanOpenUp; }
	else if (strcmp(pacmanMovementDirection, "DOWN") == 0) { pacmanTmp = pacmanOpenDown; }

	SDL_Texture* allTextures[] = { pacmanTmp, purpleGhost, brownGhost, greenGhost, yellowGhost };
	SDL_Rect* allTexturesPosition[] = { &pacmanPositionPixels, &purpleGhostPositionPixels, &brownGhostPositionPixels, &greenGhostPositionPixels, &yellowGhostPositionPixels };
	for (int i = 0; i < 8; i++)
	{
		arrayPositionPixels->x = arrayPositionPixels->x + 4;
		arrayPositionPixels->y = arrayPositionPixels->y;
		arrayPositionPixels->w = arrayPositionPixels->w;
		arrayPositionPixels->h = arrayPositionPixels->h;

		//Render map based on logic map
		renderMap(logicMap);

		//Render texture on the map at position specified in arrayPositionPixels
		SDL_RenderCopy(renderer, texture, NULL, arrayPositionPixels);

		//rendering on the same frame all other textures, which aren't moving right now, because otherwise these textures will disappear on this frame
		for (int j = 0; j < 5; j++)
		{
			if (allTextures[j] != texture)
			{
				SDL_RenderCopy(renderer, allTextures[j], NULL, allTexturesPosition[j]);
			}
		}
		//SDL_RenderCopy(renderer, purpleGhost, NULL, &purpleGhostPositionPixels);
		SDL_RenderPresent(renderer);
	}
}

void moveUp(int** logicMap, SDL_Texture* texture, SDL_Rect* arrayPositionPixels)
{
	for (int i = 0; i < 8; i++)
	{
		arrayPositionPixels->x = arrayPositionPixels->x;
		arrayPositionPixels->y = arrayPositionPixels->y - 4;
		arrayPositionPixels->w = arrayPositionPixels->w;
		arrayPositionPixels->h = arrayPositionPixels->h;

		//Render map based on logic map
		renderMap(logicMap);

		//Render texture on the map at position specified in arrayPositionPixels
		SDL_RenderCopy(renderer, texture, NULL, arrayPositionPixels);
		SDL_RenderPresent(renderer);
	}
}

void moveDown(int** logicMap, SDL_Texture* texture, SDL_Rect* arrayPositionPixels)
{
	for (int i = 0; i < 8; i++)
	{
		arrayPositionPixels->x = arrayPositionPixels->x;
		arrayPositionPixels->y = arrayPositionPixels->y + 4;
		arrayPositionPixels->w = arrayPositionPixels->w;
		arrayPositionPixels->h = arrayPositionPixels->h;

		//Render map based on logic map
		renderMap(logicMap);

		//Render texture on the map at position specified in arrayPositionPixels
		SDL_RenderCopy(renderer, texture, NULL, arrayPositionPixels);
		SDL_RenderPresent(renderer);
	}
}

void notMove(int** logicMap, SDL_Texture* texture, SDL_Rect* arrayPositionPixels)
{
	for (int i = 0; i < 8; i++)
	{
		//Render map based on logic map
		renderMap(logicMap);

		//Render texture on the map at position specified in arrayPositionPixels
		SDL_RenderCopy(renderer, texture, NULL, arrayPositionPixels);
		SDL_RenderPresent(renderer);
	}
}

void initializePacmanEngine()
{
	//Handling user input from keyboard
	const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
	if (currentKeyStates[SDL_SCANCODE_LEFT])
	{
		if (logicMap[pacmanPositionAtLogicMap.y][pacmanPositionAtLogicMap.x - 1] == 0)
		{
			//Move pacman to the left and render it on screen
			moveLeft(logicMap, pacmanOpenLeft, &pacmanPositionPixels);

			//Update pacman position on logic map
			pacmanPositionAtLogicMap.x = pacmanPositionAtLogicMap.x - 1;
			pacmanPositionAtLogicMap.y = pacmanPositionAtLogicMap.y;
		}
		else
		{
			//Ustaw pacmana w lewo
		}

		//Update pacman movement direction
		strcpy_s(pacmanMovementDirection, 6 * sizeof(char), "LEFT");
	}
	else if (currentKeyStates[SDL_SCANCODE_RIGHT])
	{
		//Update pacman movement direction
		strcpy_s(pacmanMovementDirection, 6 * sizeof(char), "RIGHT");

		if (logicMap[pacmanPositionAtLogicMap.y][pacmanPositionAtLogicMap.x + 1] == 0)
		{
			//Move pacman to the left and render it on screen
			moveRight(logicMap, pacmanOpenRight, &pacmanPositionPixels);

			//Update pacman position on logic map
			pacmanPositionAtLogicMap.x = pacmanPositionAtLogicMap.x + 1;
			pacmanPositionAtLogicMap.y = pacmanPositionAtLogicMap.y;
		}
		else
		{
			//Ustaw pacmana w prawo
		}
	}
	else if (currentKeyStates[SDL_SCANCODE_UP])
	{
		if (logicMap[pacmanPositionAtLogicMap.y - 1][pacmanPositionAtLogicMap.x] == 0)
		{
			//Move pacman to the left and render it on screen
			moveUp(logicMap, pacmanOpenUp, &pacmanPositionPixels);

			//Update pacman position on logic map
			pacmanPositionAtLogicMap.x = pacmanPositionAtLogicMap.x;
			pacmanPositionAtLogicMap.y = pacmanPositionAtLogicMap.y - 1;
		}

		//Update pacman movement direction
		strcpy_s(pacmanMovementDirection, 6 * sizeof(char), "UP");
	}
	else if (currentKeyStates[SDL_SCANCODE_DOWN])
	{
		if (logicMap[pacmanPositionAtLogicMap.y + 1][pacmanPositionAtLogicMap.x] == 0)
		{
			//Move pacman to the left and render it on screen
			moveDown(logicMap, pacmanOpenDown, &pacmanPositionPixels);

			//Update pacman position on logic map
			pacmanPositionAtLogicMap.x = pacmanPositionAtLogicMap.x;
			pacmanPositionAtLogicMap.y = pacmanPositionAtLogicMap.y + 1;
		}

		//Update pacman movement direction
		strcpy_s(pacmanMovementDirection, 6 * sizeof(char), "DOWN");
	}
	else
	{
		//notMove(logicMap, pacmanOpenRight, &pacmanPositionPixels);
	}
}

void initializeGhostEngine()
{
	/* 
	* Sprobuj uzyc struktur do trzymania informacji o koordynatach duszkow tak jak w przypadku pacmana i w podobny sposob zaimplementuj warunek kolizji ze scianami.
	* 
	* Aktualizuj informacje o pozycji duszka na mapie logicznej, pomo¿e to przy wykrywaniu kolizji z pacmanem(jesli pacman wjedzie w duszka to GameOver)
	*		-- w jednym cyklu glownej petli programu:  zapisanie informacji o zmianie pozycji duszka --> pacman chce wjechac w to miejsce --> GameOver
	*		-- porownanie pozycji w momencie przesuniecia pacmana w prawo na zasadzie: if logicMap[pacmanPositionAtLogicMap.y][pacmanPositionAtLogicMap.x + 1] == 2 then GameOver  //'2' to purpleGhost na mapie logicznej
	* 
	* Aktualizuj informacje o pozycji pacmana na mapie logicznej, pomo¿e to przy wykrywaniu kolizcji z duszkami(jeœli duszek wjedzie w pacmana to GameOver)
	*		-- w jednym cyklu glownej petli programu(?):  zapisanie informacji o zmianie pozycji pacmana --> ktorykolwiek z duszkow chce wjechac w to miejsce --> GameOver
	*		-- porownanie pozycji w momencie przesuniecia duszka w prawo na zasadzie: if logicMap[duszekPositionAtLogicMap.y][duszekPositionAtLogicMap.x + 1] == 9 then GameOver  //'9' to pacman na mapie logicznej
	*/

	char ghostMovementDirection[6] = "right";
	//strcpy_s(ghostMovementDirection, 6 * sizeof(char), "left");

	if (strcmp(ghostMovementDirection, "right") == 0)
	{
		if (logicMap[purpleGhostPositionAtLogicMap.y][purpleGhostPositionAtLogicMap.x + 1] == 0) //warunek spelniony, jesli na nastepnym polu jest droga. Jesli jest sciana lub duszek to nie jest spelniony
		{
			moveRight(logicMap, purpleGhost, &purpleGhostPositionPixels);

			//Set old ghost position at logic map to 0 (free road)
			logicMap[purpleGhostPositionAtLogicMap.y][purpleGhostPositionAtLogicMap.x] = 0;
			//Update position of ghost at logic map
			purpleGhostPositionAtLogicMap.x = purpleGhostPositionAtLogicMap.x + 1;
			//Set current ghost position at logic map to 2 (purpleGhost)
			logicMap[purpleGhostPositionAtLogicMap.y][purpleGhostPositionAtLogicMap.x] = 2;
		}
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
		//Generate logic map
		logicMap = generateMap();

		//Load actor's textures
		loadMedia();

		bool running = true;
		SDL_Event e;

		//zmienne robocze, do usuniecia

		//Render game's start screen
		renderMap(logicMap);
		SDL_RenderCopy(renderer, pacmanOpenRight, NULL, &pacmanPositionPixels);
		SDL_RenderCopy(renderer, purpleGhost, NULL, &purpleGhostPositionPixels);
		SDL_RenderCopy(renderer, brownGhost, NULL, &brownGhostPositionPixels);
		SDL_RenderCopy(renderer, greenGhost, NULL, &greenGhostPositionPixels);
		SDL_RenderCopy(renderer, yellowGhost, NULL, &yellowGhostPositionPixels);
		SDL_RenderPresent(renderer);

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
			
			//Handling pacman movement on the map
			initializePacmanEngine();
			
			//Handling ghosts movement on the map
			initializeGhostEngine();

			SDL_RenderPresent(renderer);
		}
		//Free memory allocated for logic map
		freeMap(logicMap);
	}

	//Close SDL
	close();
	return 0;
}