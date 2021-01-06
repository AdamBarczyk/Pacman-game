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

//Ghosts position
SDL_Rect purpleGhostPositionPixels = {1*32, 2*32, 32, 32};
struct coords purpleGhostPositionAtLogicMap = { 1, 2 };
SDL_Rect brownGhostPositionPixels = {30*32, 2*32, 32, 32};
struct coords brownGhostPositionAtLogicMap = { 30, 2 };
SDL_Rect greenGhostPositionPixels = {1*32, 22*32, 32, 32};
struct coords greenGhostPositionAtLogicMap = { 1, 22 };
SDL_Rect yellowGhostPositionPixels = {30*32, 22*32, 32, 32};
struct coords yellowGhostPositionAtLogicMap = { 30, 22 };

//Direction flags
char pacmanDirectionFlag[6];
char purpleGhostDirectionFlag[6];
char brownGhostDirectionFlag[6];
char greenGhostDirectionFlag[6];
char yellowGhostDirectionFlag[6];


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

void moveLeft(SDL_Rect* arrayPositionPixels)
{
	arrayPositionPixels->x = arrayPositionPixels->x - 4;
	arrayPositionPixels->y = arrayPositionPixels->y;
	arrayPositionPixels->w = arrayPositionPixels->w;
	arrayPositionPixels->h = arrayPositionPixels->h;

	//Render map based on logic map
	//renderMap(logicMap);

	//Render texture on the map at position specified in arrayPositionPixels
	//SDL_RenderCopy(renderer, texture, NULL, arrayPositionPixels);
	//SDL_RenderPresent(renderer);
}

void moveRight(SDL_Rect* arrayPositionPixels)
{
	arrayPositionPixels->x = arrayPositionPixels->x + 4;
	arrayPositionPixels->y = arrayPositionPixels->y;
	arrayPositionPixels->w = arrayPositionPixels->w;
	arrayPositionPixels->h = arrayPositionPixels->h;

	//Render map based on logic map
	//renderMap(logicMap);

	//Render texture on the map at position specified in arrayPositionPixels
	//SDL_RenderCopy(renderer, texture, NULL, arrayPositionPixels);
	//SDL_RenderPresent(renderer);
}

void moveUp(SDL_Rect* arrayPositionPixels)
{
	arrayPositionPixels->x = arrayPositionPixels->x;
	arrayPositionPixels->y = arrayPositionPixels->y - 4;
	arrayPositionPixels->w = arrayPositionPixels->w;
	arrayPositionPixels->h = arrayPositionPixels->h;

	//Render map based on logic map
	//renderMap(logicMap);

	//Render texture on the map at position specified in arrayPositionPixels
	//SDL_RenderCopy(renderer, texture, NULL, arrayPositionPixels);
	//SDL_RenderPresent(renderer);
}

void moveDown(SDL_Rect* arrayPositionPixels)
{
	arrayPositionPixels->x = arrayPositionPixels->x;
	arrayPositionPixels->y = arrayPositionPixels->y + 4;
	arrayPositionPixels->w = arrayPositionPixels->w;
	arrayPositionPixels->h = arrayPositionPixels->h;

	//Render map based on logic map
	//renderMap(logicMap);

	//Render texture on the map at position specified in arrayPositionPixels
	//SDL_RenderCopy(renderer, texture, NULL, arrayPositionPixels);
	//SDL_RenderPresent(renderer);
}

void initializePacmanEngine() //get information about direction of the next pacman's movement
{
	//Handling user input from keyboard
	const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
	if (currentKeyStates[SDL_SCANCODE_LEFT])
	{
		if (logicMap[pacmanPositionAtLogicMap.y][pacmanPositionAtLogicMap.x - 1] == 0)
		{
			//Move pacman to the left and render it on screen
			//moveLeft(logicMap, pacmanOpenLeft, &pacmanPositionPixels);

			//Set pacman direction flag for next 32px(1 field) move on the map
			strcpy_s(pacmanDirectionFlag, 6 * sizeof(char), "LEFT");

			//Update pacman position on logic map - nie trzeba, dopoki po wcisnieciu klawisza zmieniaja sie tylko flagi kierunku dla nastepnego ruchu
			//pacmanPositionAtLogicMap.x = pacmanPositionAtLogicMap.x - 1;
			//pacmanPositionAtLogicMap.y = pacmanPositionAtLogicMap.y;
		}
	}
	else if (currentKeyStates[SDL_SCANCODE_RIGHT])
	{
		if (logicMap[pacmanPositionAtLogicMap.y][pacmanPositionAtLogicMap.x + 1] == 0)
		{
			//Move pacman to the left and render it on screen
			//moveRight(logicMap, pacmanOpenRight, &pacmanPositionPixels);

			//Set pacman direction flag for next 32px(1 field) move on the map
			strcpy_s(pacmanDirectionFlag, 6 * sizeof(char), "RIGHT");

			//Update pacman position on logic map - nie trzeba, dopoki po wcisnieciu klawisza zmieniaja sie tylko flagi kierunku dla nastepnego ruchu
			//pacmanPositionAtLogicMap.x = pacmanPositionAtLogicMap.x + 1;
			//pacmanPositionAtLogicMap.y = pacmanPositionAtLogicMap.y;
		}
	}
	else if (currentKeyStates[SDL_SCANCODE_UP])
	{
		if (logicMap[pacmanPositionAtLogicMap.y - 1][pacmanPositionAtLogicMap.x] == 0)
		{
			//Move pacman to the left and render it on screen
			//moveUp(logicMap, pacmanOpenUp, &pacmanPositionPixels);

			//Set pacman direction flag for next 32px(1 field) move on the map
			strcpy_s(pacmanDirectionFlag, 6 * sizeof(char), "UP");

			//Update pacman position on logic map - nie trzeba, dopoki po wcisnieciu klawisza zmieniaja sie tylko flagi kierunku dla nastepnego ruchu
			//pacmanPositionAtLogicMap.x = pacmanPositionAtLogicMap.x;
			//pacmanPositionAtLogicMap.y = pacmanPositionAtLogicMap.y - 1;
		}
	}
	else if (currentKeyStates[SDL_SCANCODE_DOWN])
	{
		if (logicMap[pacmanPositionAtLogicMap.y + 1][pacmanPositionAtLogicMap.x] == 0)
		{
			//Move pacman to the left and render it on screen
			//moveDown(logicMap, pacmanOpenDown, &pacmanPositionPixels);

			//Set pacman direction flag for next 32px(1 field) move on the map
			strcpy_s(pacmanDirectionFlag, 6 * sizeof(char), "DOWN");

			//Update pacman position on logic map - nie trzeba, dopoki po wcisnieciu klawisza zmieniaja sie tylko flagi kierunku dla nastepnego ruchu
			//pacmanPositionAtLogicMap.x = pacmanPositionAtLogicMap.x;
			//pacmanPositionAtLogicMap.y = pacmanPositionAtLogicMap.y + 1;
		}
	}
	else
	{
		//Set pacman direction flag for next 32px(1 field) move on the map
		strcpy_s(pacmanDirectionFlag, 6 * sizeof(char), "SKIP");
	}
}

void initializeGhostEngine(int** logicMap) //get information about direction of the next ghost movement(every ghost)
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
	* 
	* 
	* 1. zmieniono funkcje od movementu w taki sposob, zeby przesuwaly texture o 4px zamiast 32px
	* 2. zmieniono funkcje initializePacmanEngine() w taki sposob, zeby ustawiala flage kierunku dla nastepnego ruchu. Dotychczas ta funkcja wywolywala funkcje odpowiadajaca za ruch
	* 3. zmieniono funckje od movementu w taki posob, zeby teraz przesuwaly texture o 4px, ale teraz funkcje te nie renderuja juz obrazu mapy ani textur - zmieniaja tylko dane o polozeniu
	* 4. Dodano funkcje nextStepCycle(), ktora wywoluje funkcje initializePacmanEngine() oraz initializeGhostEngine() w celu zebrania informacji o ruchach pacmana i duszkow w nastepnym kroku(StepCycle). 
	*		Potem przesuwane sa wszystkie elementy o 4px, a nastepnie renderowany jest obraz. Dzieje sie tak 8 razy, zatem elementy przesuwaja sie o 4*8=32px lub nie przesuwaja sie wcale(jesli flaga kierunku
	*		ktoregokolwiek z elementow byla ustawiona na "SKIP"). Ostatnim krokiem funkcji jest zaktualizowanie informacji o przesunieciu elementow na mapie logicznej.
	
	
	
	
	*/
}

void nextStepCycle(int** logicMap) //get info about next step of everything on the map and do this step (DOESN'T UPDATE INFO ABOUT ELEMENT'S POSITION ON THE LOGIC MAP!)
{
	//Get info about next pacman step
	initializePacmanEngine();

	//Get info about next ghosts step
	//TODO make algorithm for initializeGhostEngine() function
	//	  initializeGhostEngine();

	//Move elements of the map. 8 steps: each moves elements by 4px = 8*4px = 32px = 1 (logic)field on the map
	for (int i = 0; i < 8; i++)
	{
		//move pacman
		if (strcmp(pacmanDirectionFlag, "LEFT") == 0) { moveLeft(&pacmanPositionPixels); }
		else if (strcmp(pacmanDirectionFlag, "RIGHT") == 0) { moveRight(&pacmanPositionPixels); }
		else if (strcmp(pacmanDirectionFlag, "UP") == 0) { moveUp(&pacmanPositionPixels); }
		else if (strcmp(pacmanDirectionFlag, "DOWN") == 0) { moveDown(&pacmanPositionPixels); }
		else if (strcmp(pacmanDirectionFlag, "SKIP") == 0) { /*do nothing(?)*/ }

		//move purpleGhost
		if (strcmp(purpleGhostDirectionFlag, "LEFT") == 0) { moveLeft(&purpleGhostPositionPixels); }
		else if (strcmp(purpleGhostDirectionFlag, "RIGHT") == 0) { moveRight(&purpleGhostPositionPixels); }
		else if (strcmp(purpleGhostDirectionFlag, "UP") == 0) { moveUP(&purpleGhostPositionPixels); }
		else if (strcmp(purpleGhostDirectionFlag, "DOWN") == 0) { moveDown(&purpleGhostPositionPixels); }
		else if (strcmp(purpleGhostDirectionFlag, "SKIP") == 0) { /*do nothing(?)*/ }

		//move brownGhost
		if (strcmp(brownGhostDirectionFlag, "LEFT") == 0) { moveLeft(&brownGhostPositionPixels); }
		else if (strcmp(brownGhostDirectionFlag, "RIGHT") == 0) { moveRight(&brownGhostPositionPixels); }
		else if (strcmp(brownGhostDirectionFlag, "UP") == 0) { moveUp(&brownGhostPositionPixels); }
		else if (strcmp(brownGhostDirectionFlag, "DOWN") == 0) { moveDown(&brownGhostPositionPixels); }
		else if (strcmp(brownGhostDirectionFlag, "SKIP") == 0) { /*do nothing(?)*/ }

		//move greenGhost
		if (strcmp(greenGhostDirectionFlag, "LEFT") == 0) { moveLeft(&greenGhostPositionPixels); }
		else if (strcmp(greenGhostDirectionFlag, "RIGHT") == 0) { moveRight(&greenGhostPositionPixels); }
		else if (strcmp(greenGhostDirectionFlag, "UP") == 0) { moveUp(&greenGhostPositionPixels); }
		else if (strcmp(greenGhostDirectionFlag, "DOWN") == 0) { moveDown(&greenGhostPositionPixels); }
		else if (strcmp(greenGhostDirectionFlag, "SKIP") == 0) { /*do nothing(?)*/ }

		//move yellowGhost
		if (strcmp(yellowGhostDirectionFlag, "LEFT") == 0) { moveLeft(&yellowGhostPositionPixels); }
		else if (strcmp(yellowGhostDirectionFlag, "RIGHT") == 0) { moveRight(&yellowGhostPositionPixels); }
		else if (strcmp(yellowGhostDirectionFlag, "UP") == 0) { moveUp(&yellowGhostPositionPixels); }
		else if (strcmp(yellowGhostDirectionFlag, "DOWN") == 0) { moveDown(&yellowGhostPositionPixels); }
		else if (strcmp(yellowGhostDirectionFlag, "SKIP") == 0) { /*do nothing(?)*/ }

		//render map and all moves on the map
		//render map
		renderMap(logicMap);
		//render pacman
		SDL_RenderCopy(renderer, pacmanOpenRight, NULL, &pacmanPositionPixels);
		//render all ghosts
		SDL_RenderCopy(renderer, purpleGhost, NULL, &purpleGhostPositionPixels);
		SDL_RenderCopy(renderer, brownGhost, NULL, &brownGhostPositionPixels);
		SDL_RenderCopy(renderer, greenGhost, NULL, &greenGhostPositionPixels);
		SDL_RenderCopy(renderer, yellowGhost, NULL, &yellowGhostPositionPixels);
	}

	//Update pacman position
	logicMap[pacmanPositionAtLogicMap.y][pacmanPositionAtLogicMap.x] = 0; //Delete old pacman position from logic map (0 - road)
	if (strcmp(pacmanDirectionFlag, "LEFT") == 0)
	{ 
		pacmanPositionAtLogicMap.x = pacmanPositionAtLogicMap.x - 1;
		logicMap[pacmanPositionAtLogicMap.y][pacmanPositionAtLogicMap.x - 1] = 9; //Set new pacman position in logic map (9 - pacman)
	}
	else if (strcmp(pacmanDirectionFlag, "RIGHT") == 0)
	{ 
		pacmanPositionAtLogicMap.x = pacmanPositionAtLogicMap.x + 1;
		logicMap[pacmanPositionAtLogicMap.y][pacmanPositionAtLogicMap.x + 1] = 9;
	}
	else if (strcmp(pacmanDirectionFlag, "UP") == 0)
	{ 
		pacmanPositionAtLogicMap.y = pacmanPositionAtLogicMap.y - 1; 
		logicMap[pacmanPositionAtLogicMap.y - 1][pacmanPositionAtLogicMap.x] = 9;
	}
	else if (strcmp(pacmanDirectionFlag, "DOWN") == 0)
	{
		pacmanPositionAtLogicMap.y = pacmanPositionAtLogicMap.y + 1;
		logicMap[pacmanPositionAtLogicMap.y + 1][pacmanPositionAtLogicMap.x] = 9;
	}
	else if (strcmp(pacmanDirectionFlag, "SKIP") == 0) { /*do nothing(?)*/ }

	//Update purpleGhost
	logicMap[purpleGhostPositionAtLogicMap.y][purpleGhostPositionAtLogicMap.x] = 0; //Delete old purpleGhost position from logic map (0 - road)
	if (strcmp(purpleGhostDirectionFlag, "LEFT") == 0)
	{ 
		purpleGhostPositionAtLogicMap.x = purpleGhostPositionAtLogicMap.x - 1;
		logicMap[purpleGhostPositionAtLogicMap.y][purpleGhostPositionAtLogicMap.x - 1] = 2; //Set new purpleGhost position in logic map (2 - purpleGhost)
	}
	else if (strcmp(purpleGhostDirectionFlag, "RIGHT") == 0)
	{ 
		purpleGhostPositionAtLogicMap.x = purpleGhostPositionAtLogicMap.x + 1;
		logicMap[purpleGhostPositionAtLogicMap.y][purpleGhostPositionAtLogicMap.x + 1] = 2;
	}
	else if (strcmp(purpleGhostDirectionFlag, "UP") == 0)
	{ 
		purpleGhostPositionAtLogicMap.y = purpleGhostPositionAtLogicMap.y - 1;
		logicMap[purpleGhostPositionAtLogicMap.y - 1][purpleGhostPositionAtLogicMap.x] = 2;
	}
	else if (strcmp(purpleGhostDirectionFlag, "DOWN") == 0)
	{ 
		purpleGhostPositionAtLogicMap.y = purpleGhostPositionAtLogicMap.y + 1;
		logicMap[purpleGhostPositionAtLogicMap.y + 1][purpleGhostPositionAtLogicMap.x] = 2;
	}
	else if (strcmp(purpleGhostDirectionFlag, "SKIP") == 0) { /*do nothing(?)*/ }

	//Update brownGhost
	logicMap[brownGhostPositionAtLogicMap.y][brownGhostPositionAtLogicMap.x] = 0; //Delete old brownGhost position from logic map (0 - road)
	if (strcmp(brownGhostDirectionFlag, "LEFT") == 0)
	{
		brownGhostPositionAtLogicMap.x = brownGhostPositionAtLogicMap.x - 1;
		logicMap[brownGhostPositionAtLogicMap.y][brownGhostPositionAtLogicMap.x - 1] = 3; //Set new brownGhost position in logic map (3 - brownGhost)
	}
	else if (strcmp(brownGhostDirectionFlag, "RIGHT") == 0)
	{
		brownGhostPositionAtLogicMap.x = brownGhostPositionAtLogicMap.x + 1;
		logicMap[brownGhostPositionAtLogicMap.y][brownGhostPositionAtLogicMap.x + 1] = 3;
	}
	else if (strcmp(brownGhostDirectionFlag, "UP") == 0)
	{
		brownGhostPositionAtLogicMap.y = brownGhostPositionAtLogicMap.y - 1;
		logicMap[brownGhostPositionAtLogicMap.y - 1][brownGhostPositionAtLogicMap.x] = 3;
	}
	else if (strcmp(brownGhostDirectionFlag, "DOWN") == 0)
	{
		brownGhostPositionAtLogicMap.y = brownGhostPositionAtLogicMap.y + 1;
		logicMap[brownGhostPositionAtLogicMap.y + 1][brownGhostPositionAtLogicMap.x] = 3;
	}
	else if (strcmp(brownGhostDirectionFlag, "SKIP") == 0) { /*do nothing(?)*/ }

	//Update greenGhost
	logicMap[greenGhostPositionAtLogicMap.y][greenGhostPositionAtLogicMap.x] = 0; //Delete old greenGhost position from logic map (0 - road)
	if (strcmp(greenGhostDirectionFlag, "LEFT") == 0)
	{
		greenGhostPositionAtLogicMap.x = greenGhostPositionAtLogicMap.x - 1;
		logicMap[greenGhostPositionAtLogicMap.y][greenGhostPositionAtLogicMap.x - 1] = 4; //Set new greenGhost position in logic map (4 - greenGhost)
	}
	else if (strcmp(greenGhostDirectionFlag, "RIGHT") == 0)
	{
		greenGhostPositionAtLogicMap.x = greenGhostPositionAtLogicMap.x + 1;
		logicMap[greenGhostPositionAtLogicMap.y][greenGhostPositionAtLogicMap.x + 1] = 4;
	}
	else if (strcmp(greenGhostDirectionFlag, "UP") == 0)
	{
		greenGhostPositionAtLogicMap.y = greenGhostPositionAtLogicMap.y - 1;
		logicMap[greenGhostPositionAtLogicMap.y - 1][greenGhostPositionAtLogicMap.x] = 4;
	}
	else if (strcmp(greenGhostDirectionFlag, "DOWN") == 0)
	{
		greenGhostPositionAtLogicMap.y = greenGhostPositionAtLogicMap.y + 1;
		logicMap[greenGhostPositionAtLogicMap.y + 1][greenGhostPositionAtLogicMap.x] = 4;
	}
	else if (strcmp(greenGhostDirectionFlag, "SKIP") == 0) { /*do nothing(?)*/ }

	//Update yellowGhost
	logicMap[yellowGhostPositionAtLogicMap.y][yellowGhostPositionAtLogicMap.x] = 0; //Delete old yellowGhost position from logic map (0 - road)
	if (strcmp(yellowGhostDirectionFlag, "LEFT") == 0)
	{
		yellowGhostPositionAtLogicMap.x = yellowGhostPositionAtLogicMap.x - 1;
		logicMap[yellowGhostPositionAtLogicMap.y][yellowGhostPositionAtLogicMap.x - 1] = 5; //Set new yellowGhost position in logic map (5 - yellowGhost)
	}
	else if (strcmp(yellowGhostDirectionFlag, "RIGHT") == 0)
	{
		yellowGhostPositionAtLogicMap.x = yellowGhostPositionAtLogicMap.x + 1;
		logicMap[yellowGhostPositionAtLogicMap.y][yellowGhostPositionAtLogicMap.x + 1] = 5;
	}
	else if (strcmp(yellowGhostDirectionFlag, "UP") == 0)
	{
		yellowGhostPositionAtLogicMap.y = yellowGhostPositionAtLogicMap.y - 1;
		logicMap[yellowGhostPositionAtLogicMap.y - 1][yellowGhostPositionAtLogicMap.x] = 5;
	}
	else if (strcmp(yellowGhostDirectionFlag, "DOWN") == 0)
	{
		yellowGhostPositionAtLogicMap.y = yellowGhostPositionAtLogicMap.y + 1;
		logicMap[yellowGhostPositionAtLogicMap.y + 1][yellowGhostPositionAtLogicMap.x] = 5;
	}
	else if (strcmp(yellowGhostDirectionFlag, "SKIP") == 0) { /*do nothing(?)*/ }
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

			SDL_RenderPresent(renderer);
		}
		//Free memory allocated for logic map
		freeMap(logicMap);
	}

	//Close SDL
	close();
	return 0;
}