//Using SDL and standard IO
#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL_image.h>
#include <string.h>
#include <math.h>


//Declaring structures
struct coords
{
	int x, y;
};

//Screen dimension constants
const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 768;

//Initialize SDL and creates window
bool initialize();

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

//Flag for slowing down ghosts
bool skipGhostsMove = true;

//Flag for run gameOver function
bool gameOver = false;

bool initialize()
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

	//Quit SDL
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
		{1,0,1,0,0,0,0,0,0,0,1,0,0,0,0,9,0,0,0,0,0,1,0,1,0,0,0,1,0,1,0,1},
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
				rect.w = 32;
				rect.h = 32;
				SDL_RenderFillRect(renderer, &rect);
			}
		}
	}
}

SDL_Texture* loadTexture(char* path)
{
	//The texture to load
	SDL_Texture* texture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path);
	if (loadedSurface == NULL)
	{
		printf("\nImage could not be loaded in loadPacman function! Source: %s SDL_image Error: %s", path, IMG_GetError());
	}
	else
	{
		//Create texture from loadedSurface pixels
		texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
		if (texture == NULL)
		{
			printf("\nTexture could not be created in loadPacman function! SourceFile: %s SDL Error: %s", path, SDL_GetError());
		}

		//free loadedSurface
		SDL_FreeSurface(loadedSurface);
	}

	return texture;
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
}

void moveRight(SDL_Rect* arrayPositionPixels)
{
	arrayPositionPixels->x = arrayPositionPixels->x + 4;
	arrayPositionPixels->y = arrayPositionPixels->y;
	arrayPositionPixels->w = arrayPositionPixels->w;
	arrayPositionPixels->h = arrayPositionPixels->h;
}

void moveUp(SDL_Rect* arrayPositionPixels)
{
	arrayPositionPixels->x = arrayPositionPixels->x;
	arrayPositionPixels->y = arrayPositionPixels->y - 4;
	arrayPositionPixels->w = arrayPositionPixels->w;
	arrayPositionPixels->h = arrayPositionPixels->h;
}

void moveDown(SDL_Rect* arrayPositionPixels)
{
	arrayPositionPixels->x = arrayPositionPixels->x;
	arrayPositionPixels->y = arrayPositionPixels->y + 4;
	arrayPositionPixels->w = arrayPositionPixels->w;
	arrayPositionPixels->h = arrayPositionPixels->h;
}

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
	* 5. Dodano funkcje getGhostDirectionFromAngle(), ktora na podstawie otrzymanego katu obliczonego w funkcji initializeGhostEngine() wybiera kierunek duszka w nastepnym ruchu w taki sposob, 
	*		zeby duszek "gonil" pacmana. Kierunek jest zapisywany pod zmienna directionFlag(ta zmienna jest parametrem funkcji, pod ktory w czasie wywolania sa podpisywane zmienne <color>GhostDirectionFlag)
	* 6. Wywolano funkcje nextStepCycle(&logicMap) w funkcji main()
	*
	*  @@@@@@@ Z NOTATNIKA PAPIEROWEGO ZOSTALO PRAKTYCZNIE NAPISAC FUNKCJE WYBIERAJACA KIERUNEK DUSZKOW I ODPALIC NAPISANE FUNKCJE W MAINIE @@@@@@@
	*/

void updatePosition(int** logicMap, struct coords* positionAtLogicMap, char* directionFlag, int id)
{
	if (strcmp(directionFlag, "LEFT") == 0)
	{
		logicMap[positionAtLogicMap->y][positionAtLogicMap->x] = 0; //Delete old yellowGhost position from logic map (0 - road)
		positionAtLogicMap->x = positionAtLogicMap->x - 1;
		logicMap[positionAtLogicMap->y][positionAtLogicMap->x] = id; //Set new yellowGhost position in logic map (5 - yellowGhost)
	}
	else if (strcmp(directionFlag, "RIGHT") == 0)
	{
		logicMap[positionAtLogicMap->y][positionAtLogicMap->x] = 0; //Delete old yellowGhost position from logic map (0 - road)
		positionAtLogicMap->x = positionAtLogicMap->x + 1;
		logicMap[positionAtLogicMap->y][positionAtLogicMap->x] = id;
	}
	else if (strcmp(directionFlag, "UP") == 0)
	{
		logicMap[positionAtLogicMap->y][positionAtLogicMap->x] = 0; //Delete old yellowGhost position from logic map (0 - road)
		positionAtLogicMap->y = positionAtLogicMap->y - 1;
		logicMap[positionAtLogicMap->y][positionAtLogicMap->x] = id;
	}
	else if (strcmp(directionFlag, "DOWN") == 0)
	{
		logicMap[positionAtLogicMap->y][positionAtLogicMap->x] = 0; //Delete old yellowGhost position from logic map (0 - road)
		positionAtLogicMap->y = positionAtLogicMap->y + 1;
		logicMap[positionAtLogicMap->y][positionAtLogicMap->x] = id;
	}
	else if (strcmp(directionFlag, "SKIP") == 0) { /*do nothing(?)*/ }
}

void getGhostDirectionFromAngle(int** logicMap, double angle, struct coords* positionAtLogicMap, char* directionFlag, int id)
{
	/* Ghost can not choose direction he is coming from to avoid bug with ghost moving between two fields endlessly*/

	/* Collision it's all about current posisions at logic map. If the ghosts sees '0' value in logicMap, then he can go there */

	if (angle > 0 && angle <= 45)
	{
		/* 1. PRAWO --> 2. GORA --> 3. DOL --> 4. LEWO */
		if (logicMap[positionAtLogicMap->y][positionAtLogicMap->x + 1] == 0 && strcmp(directionFlag, "LEFT") != 0) { 
			strcpy_s(directionFlag, 6 * sizeof(char), "RIGHT"); 
			updatePosition(logicMap, positionAtLogicMap, directionFlag, id);
		}  //PRAWO
		else if (logicMap[positionAtLogicMap->y - 1][positionAtLogicMap->x] == 0 && strcmp(directionFlag, "DOWN") != 0) { 
			strcpy_s(directionFlag, 6 * sizeof(char), "UP"); 
			updatePosition(logicMap, positionAtLogicMap, directionFlag, id);
		} //GORA
		else if (logicMap[positionAtLogicMap->y + 1][positionAtLogicMap->x] == 0 && strcmp(directionFlag, "UP") != 0) { 
			strcpy_s(directionFlag, 6 * sizeof(char), "DOWN");  
			updatePosition(logicMap, positionAtLogicMap, directionFlag, id);
		} //DOL
		else if (logicMap[positionAtLogicMap->y][positionAtLogicMap->x - 1] == 0 && strcmp(directionFlag, "RIGHT") != 0) { 
			strcpy_s(directionFlag, 6 * sizeof(char), "LEFT"); 
			updatePosition(logicMap, positionAtLogicMap, directionFlag, id);
		} //LEWO
		else { strcpy_s(directionFlag, 6 * sizeof(char), "SKIP"); } //STOP - without this statement ghosts sometimes moves through walls and changing them into road
	}
	else if (angle > 45 && angle <= 90)
	{
		/* 1. GORA --> 2. PRAWO --> 3. LEWO --> 4. DOL */
		if (logicMap[positionAtLogicMap->y - 1][positionAtLogicMap->x] == 0 && strcmp(directionFlag, "DOWN") != 0) { 
			strcpy_s(directionFlag, 6 * sizeof(char), "UP"); 
			updatePosition(logicMap, positionAtLogicMap, directionFlag, id);
		}  //GORA
		else if (logicMap[positionAtLogicMap->y][positionAtLogicMap->x + 1] == 0 && strcmp(directionFlag, "LEFT") != 0) { 
			strcpy_s(directionFlag, 6 * sizeof(char), "RIGHT"); 
			updatePosition(logicMap, positionAtLogicMap, directionFlag, id);
		} //PRAWO
		else if (logicMap[positionAtLogicMap->y][positionAtLogicMap->x - 1] == 0 && strcmp(directionFlag, "RIGHT") != 0) { 
			strcpy_s(directionFlag, 6 * sizeof(char), "LEFT"); 
			updatePosition(logicMap, positionAtLogicMap, directionFlag, id);
		} //LEWO
		else if (logicMap[positionAtLogicMap->y + 1][positionAtLogicMap->x] == 0 && strcmp(directionFlag, "UP") != 0) { 
			strcpy_s(directionFlag, 6 * sizeof(char), "DOWN"); 
			updatePosition(logicMap, positionAtLogicMap, directionFlag, id);
		} //DOL
		else { strcpy_s(directionFlag, 6 * sizeof(char), "SKIP"); } //STOP - without this statement ghosts sometimes moves through walls and changing them into road
	}
	else if (angle > 90 && angle <= 135)
	{
		/* 1. GORA --> 2. LEWO --> 3. PRAWO --> 4. DOL */
		if (logicMap[positionAtLogicMap->y - 1][positionAtLogicMap->x] == 0 && strcmp(directionFlag, "DOWN") != 0) { 
			strcpy_s(directionFlag, 6 * sizeof(char), "UP"); 
			updatePosition(logicMap, positionAtLogicMap, directionFlag, id);
		}  //GORA
		else if (logicMap[positionAtLogicMap->y][positionAtLogicMap->x - 1] == 0 && strcmp(directionFlag, "RIGHT") != 0) { 
			strcpy_s(directionFlag, 6 * sizeof(char), "LEFT"); 
			updatePosition(logicMap, positionAtLogicMap, directionFlag, id);
		} //LEWO
		else if (logicMap[positionAtLogicMap->y][positionAtLogicMap->x + 1] == 0 && strcmp(directionFlag, "LEFT") != 0) { 
			strcpy_s(directionFlag, 6 * sizeof(char), "RIGHT"); 
			updatePosition(logicMap, positionAtLogicMap, directionFlag, id);
		} //PRAWO
		else if (logicMap[positionAtLogicMap->y + 1][positionAtLogicMap->x] == 0 && strcmp(directionFlag, "UP") != 0) { 
			strcpy_s(directionFlag, 6 * sizeof(char), "DOWN"); 
			updatePosition(logicMap, positionAtLogicMap, directionFlag, id);
		} //DOL
		else { strcpy_s(directionFlag, 6 * sizeof(char), "SKIP"); } //STOP - without this statement ghosts sometimes moves through walls and changing them into road
	}
	else if ((angle > 135 && angle <= 180) || angle == -180)
	{
		/* 1. LEWO --> 2. GORA --> 3. DOL --> 4. PRAWO */
		if (logicMap[positionAtLogicMap->y][positionAtLogicMap->x - 1] == 0 && strcmp(directionFlag, "RIGHT") != 0) { 
			strcpy_s(directionFlag, 6 * sizeof(char), "LEFT"); 
			updatePosition(logicMap, positionAtLogicMap, directionFlag, id);
		} //LEWO
		else if (logicMap[positionAtLogicMap->y - 1][positionAtLogicMap->x] == 0 && strcmp(directionFlag, "DOWN") != 0) { 
			strcpy_s(directionFlag, 6 * sizeof(char), "UP"); 
			updatePosition(logicMap, positionAtLogicMap, directionFlag, id);
		}  //GORA
		else if (logicMap[positionAtLogicMap->y + 1][positionAtLogicMap->x] == 0 && strcmp(directionFlag, "UP") != 0) { 
			strcpy_s(directionFlag, 6 * sizeof(char), "DOWN"); 
			updatePosition(logicMap, positionAtLogicMap, directionFlag, id);
		} //DOL
		else if (logicMap[positionAtLogicMap->y][positionAtLogicMap->x + 1] == 0 && strcmp(directionFlag, "LEFT") != 0) { 
			strcpy_s(directionFlag, 6 * sizeof(char), "RIGHT"); 
			updatePosition(logicMap, positionAtLogicMap, directionFlag, id);
		} //PRAWO
		else { strcpy_s(directionFlag, 6 * sizeof(char), "SKIP"); } //STOP - without this statement ghosts sometimes moves through walls and changing them into road
	}
	else if (angle > -180 && angle <= -135)
	{
		/* 1. LEWO --> 2. DOL --> 3. GORA --> 4. PRAWO */
		if (logicMap[positionAtLogicMap->y][positionAtLogicMap->x - 1] == 0 && strcmp(directionFlag, "RIGHT") != 0) { 
			strcpy_s(directionFlag, 6 * sizeof(char), "LEFT"); 
			updatePosition(logicMap, positionAtLogicMap, directionFlag, id);
		} //LEWO
		else if (logicMap[positionAtLogicMap->y + 1][positionAtLogicMap->x] == 0 && strcmp(directionFlag, "UP") != 0) { 
			strcpy_s(directionFlag, 6 * sizeof(char), "DOWN"); 
			updatePosition(logicMap, positionAtLogicMap, directionFlag, id);
		} //DOL
		else if (logicMap[positionAtLogicMap->y - 1][positionAtLogicMap->x] == 0 && strcmp(directionFlag, "DOWN") != 0) { 
			strcpy_s(directionFlag, 6 * sizeof(char), "UP"); 
			updatePosition(logicMap, positionAtLogicMap, directionFlag, id);
		}  //GORA
		else if (logicMap[positionAtLogicMap->y][positionAtLogicMap->x + 1] == 0 && strcmp(directionFlag, "LEFT") != 0) { 
			strcpy_s(directionFlag, 6 * sizeof(char), "RIGHT"); 
			updatePosition(logicMap, positionAtLogicMap, directionFlag, id);
		} //PRAWO
		else { strcpy_s(directionFlag, 6 * sizeof(char), "SKIP"); } //STOP - without this statement ghosts sometimes moves through walls and changing them into road
	}
	else if (angle > -135 && angle <= -90)
	{
		/* 1. DOL --> 2. LEWO --> 3. PRAWO --> GORA */
		if (logicMap[positionAtLogicMap->y + 1][positionAtLogicMap->x] == 0 && strcmp(directionFlag, "UP") != 0) { 
			strcpy_s(directionFlag, 6 * sizeof(char), "DOWN"); 
			updatePosition(logicMap, positionAtLogicMap, directionFlag, id);
		} //DOL
		else if (logicMap[positionAtLogicMap->y][positionAtLogicMap->x - 1] == 0 && strcmp(directionFlag, "RIGHT") != 0) { 
			strcpy_s(directionFlag, 6 * sizeof(char), "LEFT"); 
			updatePosition(logicMap, positionAtLogicMap, directionFlag, id);
		} //LEWO
		else if (logicMap[positionAtLogicMap->y][positionAtLogicMap->x + 1] == 0 && strcmp(directionFlag, "LEFT") != 0) { 
			strcpy_s(directionFlag, 6 * sizeof(char), "RIGHT"); 
			updatePosition(logicMap, positionAtLogicMap, directionFlag, id);
		} //PRAWO
		else if (logicMap[positionAtLogicMap->y - 1][positionAtLogicMap->x] == 0 && strcmp(directionFlag, "DOWN") != 0) { 
			strcpy_s(directionFlag, 6 * sizeof(char), "UP"); 
			updatePosition(logicMap, positionAtLogicMap, directionFlag, id);
		}  //GORA
		else { strcpy_s(directionFlag, 6 * sizeof(char), "SKIP"); } //STOP - without this statement ghosts sometimes moves through walls and changing them into road
	}
	else if (angle > -90 && angle <= -45)
	{
		/* 1. DOL --> 2. PRAWO --> 3. LEWO --> GORA */
		if (logicMap[positionAtLogicMap->y + 1][positionAtLogicMap->x] == 0 && strcmp(directionFlag, "UP") != 0) { 
			strcpy_s(directionFlag, 6 * sizeof(char), "DOWN"); 
			updatePosition(logicMap, positionAtLogicMap, directionFlag, id);
		} //DOL
		else if (logicMap[positionAtLogicMap->y][positionAtLogicMap->x + 1] == 0 && strcmp(directionFlag, "LEFT") != 0) { 
			strcpy_s(directionFlag, 6 * sizeof(char), "RIGHT"); 
			updatePosition(logicMap, positionAtLogicMap, directionFlag, id);
		} //PRAWO
		else if (logicMap[positionAtLogicMap->y][positionAtLogicMap->x - 1] == 0 && strcmp(directionFlag, "RIGHT") != 0) { 
			strcpy_s(directionFlag, 6 * sizeof(char), "LEFT"); 
			updatePosition(logicMap, positionAtLogicMap, directionFlag, id);
		} //LEWO
		else if (logicMap[positionAtLogicMap->y - 1][positionAtLogicMap->x] == 0 && strcmp(directionFlag, "DOWN") != 0) { 
			strcpy_s(directionFlag, 6 * sizeof(char), "UP"); 
			updatePosition(logicMap, positionAtLogicMap, directionFlag, id);
		}  //GORA
		else { strcpy_s(directionFlag, 6 * sizeof(char), "SKIP"); } //STOP - without this statement ghosts sometimes moves through walls and changing them into road
	}
	else if (angle > -45 && angle <= 0)
	{
		/* 1. PRAWO --> 2. DOL --> 3. GORA --> 4. LEWO */
		if (logicMap[positionAtLogicMap->y][positionAtLogicMap->x + 1] == 0 && strcmp(directionFlag, "LEFT") != 0) { 
			strcpy_s(directionFlag, 6 * sizeof(char), "RIGHT"); 
			updatePosition(logicMap, positionAtLogicMap, directionFlag, id);
		} //PRAWO
		else if (logicMap[positionAtLogicMap->y + 1][positionAtLogicMap->x] == 0 && strcmp(directionFlag, "UP") != 0) { 
			strcpy_s(directionFlag, 6 * sizeof(char), "DOWN"); 
			updatePosition(logicMap, positionAtLogicMap, directionFlag, id);
		} //DOL
		else if (logicMap[positionAtLogicMap->y - 1][positionAtLogicMap->x] == 0 && strcmp(directionFlag, "DWON") != 0) { 
			strcpy_s(directionFlag, 6 * sizeof(char), "UP"); 
			updatePosition(logicMap, positionAtLogicMap, directionFlag, id);
		}  //GORA
		else if (logicMap[positionAtLogicMap->y][positionAtLogicMap->x - 1] == 0 && strcmp(directionFlag, "RIGHT") != 0) { 
			strcpy_s(directionFlag, 6 * sizeof(char), "LEFT"); 
			updatePosition(logicMap, positionAtLogicMap, directionFlag, id);
		} //LEWO
		else { strcpy_s(directionFlag, 6 * sizeof(char), "SKIP"); } //STOP - without this statement ghosts sometimes moves through walls and changing them into road
	}
	//else { strcpy_s(directionFlag, 6 * sizeof(char), "SKIP"); } //STOP - without this statement ghosts sometimes moves through walls and changing them into road
}

void getPacmanDirectionInNextStep(int** logicMap) //get information about direction of the next pacman's move and update position to position after this move
{
	//Handling user input from keyboard
	const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
	if (currentKeyStates[SDL_SCANCODE_LEFT])
	{
		if (logicMap[pacmanPositionAtLogicMap.y][pacmanPositionAtLogicMap.x - 1] == 0)
		{
			strcpy_s(pacmanDirectionFlag, 6 * sizeof(char), "LEFT");
			updatePosition(logicMap, &pacmanPositionAtLogicMap, pacmanDirectionFlag, 9);  //9 = pacman
		}
		else { strcpy_s(pacmanDirectionFlag, 6 * sizeof(char), "SKIP"); }
	}
	else if (currentKeyStates[SDL_SCANCODE_RIGHT])
	{
		if (logicMap[pacmanPositionAtLogicMap.y][pacmanPositionAtLogicMap.x + 1] == 0)
		{
			strcpy_s(pacmanDirectionFlag, 6 * sizeof(char), "RIGHT");
			updatePosition(logicMap, &pacmanPositionAtLogicMap, pacmanDirectionFlag, 9);  //9 = pacman
		}
		else { strcpy_s(pacmanDirectionFlag, 6 * sizeof(char), "SKIP"); }
	}
	else if (currentKeyStates[SDL_SCANCODE_UP])
	{
		if (logicMap[pacmanPositionAtLogicMap.y - 1][pacmanPositionAtLogicMap.x] == 0)
		{
			strcpy_s(pacmanDirectionFlag, 6 * sizeof(char), "UP");
			updatePosition(logicMap, &pacmanPositionAtLogicMap, pacmanDirectionFlag, 9);  //9 = pacman
		}
		else { strcpy_s(pacmanDirectionFlag, 6 * sizeof(char), "SKIP"); }
	}
	else if (currentKeyStates[SDL_SCANCODE_DOWN])
	{
		if (logicMap[pacmanPositionAtLogicMap.y + 1][pacmanPositionAtLogicMap.x] == 0)
		{
			strcpy_s(pacmanDirectionFlag, 6 * sizeof(char), "DOWN");
			updatePosition(logicMap, &pacmanPositionAtLogicMap, pacmanDirectionFlag, 9);  //9 = pacman
		}
		else { strcpy_s(pacmanDirectionFlag, 6 * sizeof(char), "SKIP"); }
	}
	else
	{
		//Set pacman direction flag for next 32px(1 field) move on the map
		strcpy_s(pacmanDirectionFlag, 6 * sizeof(char), "SKIP");
	}
}

void getGhostsDirectionInNextStep(int** logicMap) //get information about direction of the next ghost movement(every ghost)
{
	//variable of type double to store angle of stretch between 2 points at cartesian coordinate system
	//(WARNING! Implemented in this program cartesian coordinate system assumes that THE GREATER THE Y, THE LOWER THE POSITION OF P(X,Y) - cartesian coordinate system for graphic) 
	//1st quadrant of cartesian coordinate system: 0 - 90 degree
	//2nd quadrant: 90 - 180 degree
	//3rd quadrant: (-180) - (-90) degree
	//4th quadrant: (-90) - 0 degree
	double angle;

	//purpleGhost
	angle = atan2(-((double)pacmanPositionAtLogicMap.y - (double)purpleGhostPositionAtLogicMap.y), ((double)pacmanPositionAtLogicMap.x - (double)purpleGhostPositionAtLogicMap.x)) * 180 / M_PI;
	printf("purple pos: (%d,%d)      purple angle = %f           ", purpleGhostPositionAtLogicMap.x, purpleGhostPositionAtLogicMap.y, angle);
	getGhostDirectionFromAngle(logicMap, angle, &purpleGhostPositionAtLogicMap, purpleGhostDirectionFlag, 2);
	printf("--> %s\n", purpleGhostDirectionFlag);

	//brownGhost
	angle = atan2(-((double)pacmanPositionAtLogicMap.y - (double)brownGhostPositionAtLogicMap.y), ((double)pacmanPositionAtLogicMap.x - (double)brownGhostPositionAtLogicMap.x)) * 180 / M_PI;
	//printf("brown angle = %f\n", angle);
	getGhostDirectionFromAngle(logicMap, angle, &brownGhostPositionAtLogicMap, brownGhostDirectionFlag, 3);

	//greenGhost
	angle = atan2(-((double)pacmanPositionAtLogicMap.y - (double)greenGhostPositionAtLogicMap.y), ((double)pacmanPositionAtLogicMap.x - (double)greenGhostPositionAtLogicMap.x)) * 180 / M_PI;
	//printf("green angle = %f           ", angle);
	getGhostDirectionFromAngle(logicMap, angle, &greenGhostPositionAtLogicMap, greenGhostDirectionFlag, 4);

	//yellowGhost
	angle = atan2(-((double)pacmanPositionAtLogicMap.y - (double)yellowGhostPositionAtLogicMap.y), ((double)pacmanPositionAtLogicMap.x - (double)yellowGhostPositionAtLogicMap.x)) * 180 / M_PI;
	//printf("yellow angle = %f\n", angle);
	getGhostDirectionFromAngle(logicMap, angle, &yellowGhostPositionAtLogicMap, yellowGhostDirectionFlag, 5);

	//printf("purple pos: (%d,%d)                 ", purpleGhostPositionAtLogicMap.x, purpleGhostPositionAtLogicMap.y);
	//printf("green pos: (%d,%d)           \n", greenGhostPositionAtLogicMap.x, greenGhostPositionAtLogicMap.y);
}

void initializeEngine(int** logicMap) //get info about next step of everything on the map and do this step (DOESN'T UPDATE INFO ABOUT ELEMENT'S POSITION ON THE LOGIC MAP!)
{
	//Get info about next pacman step
	getPacmanDirectionInNextStep(logicMap);

	//Get info about next ghosts step
	if (!skipGhostsMove)
	{
		getGhostsDirectionInNextStep(logicMap);
	}
	//printf("%s: ", pacmanDirectionFlag);
	//Move elements of the map. 8 steps: each moves elements by 4px = 8*4px = 32px = 1 (logic)field on the map
	for (int i = 0; i < 8; i++)
	{
		SDL_Delay(12); //~60FPS refresh rate
		//move pacman
		if (strcmp(pacmanDirectionFlag, "LEFT") == 0) { moveLeft(&pacmanPositionPixels); }
		else if (strcmp(pacmanDirectionFlag, "RIGHT") == 0) { moveRight(&pacmanPositionPixels); }
		else if (strcmp(pacmanDirectionFlag, "UP") == 0) { moveUp(&pacmanPositionPixels); }
		else if (strcmp(pacmanDirectionFlag, "DOWN") == 0) { moveDown(&pacmanPositionPixels); }
		else if (strcmp(pacmanDirectionFlag, "SKIP") == 0) { /*do nothing(?)*/ }

		if (i%2 == 0)
		{
			//move purpleGhost
			if (strcmp(purpleGhostDirectionFlag, "LEFT") == 0) { moveLeft(&purpleGhostPositionPixels); }
			else if (strcmp(purpleGhostDirectionFlag, "RIGHT") == 0) { moveRight(&purpleGhostPositionPixels); }
			else if (strcmp(purpleGhostDirectionFlag, "UP") == 0) { moveUp(&purpleGhostPositionPixels); }
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
		}

		//render map and all moves on the map
		//render map
		renderMap(logicMap);
		//render pacman
		if (strcmp(pacmanDirectionFlag, "LEFT") == 0) { SDL_RenderCopy(renderer, pacmanOpenLeft, NULL, &pacmanPositionPixels); }
		else if (strcmp(pacmanDirectionFlag, "RIGHT") == 0) { SDL_RenderCopy(renderer, pacmanOpenRight, NULL, &pacmanPositionPixels); }
		else if (strcmp(pacmanDirectionFlag, "UP") == 0) { SDL_RenderCopy(renderer, pacmanOpenUp, NULL, &pacmanPositionPixels); }
		else if (strcmp(pacmanDirectionFlag, "DOWN") == 0) { SDL_RenderCopy(renderer, pacmanOpenDown, NULL, &pacmanPositionPixels); }
		else { SDL_RenderCopy(renderer, pacmanOpenRight, NULL, &pacmanPositionPixels); }
		
		//render all ghosts
		SDL_RenderCopy(renderer, purpleGhost, NULL, &purpleGhostPositionPixels);
		SDL_RenderCopy(renderer, brownGhost, NULL, &brownGhostPositionPixels);
		SDL_RenderCopy(renderer, greenGhost, NULL, &greenGhostPositionPixels);
		SDL_RenderCopy(renderer, yellowGhost, NULL, &yellowGhostPositionPixels);
		SDL_RenderPresent(renderer);
	}

	/*printf("%d, %d", pacmanPositionAtLogicMap.x, pacmanPositionAtLogicMap.y);
	printf(" [%d,%d]=%d; ", pacmanPositionAtLogicMap.x - 1, pacmanPositionAtLogicMap.y, logicMap[pacmanPositionAtLogicMap.y][pacmanPositionAtLogicMap.x - 1]);
	printf("[%d,%d]=%d; ", pacmanPositionAtLogicMap.x, pacmanPositionAtLogicMap.y - 1, logicMap[pacmanPositionAtLogicMap.y - 1][pacmanPositionAtLogicMap.x]);
	printf("[%d,%d]=%d; ", pacmanPositionAtLogicMap.x, pacmanPositionAtLogicMap.y + 1, logicMap[pacmanPositionAtLogicMap.y + 1][pacmanPositionAtLogicMap.x]);
	printf("[%d,%d]=%d; \n", pacmanPositionAtLogicMap.x + 1, pacmanPositionAtLogicMap.y, logicMap[pacmanPositionAtLogicMap.y][pacmanPositionAtLogicMap.x + 1]);*/

	//update position of pacman and ghosts on the logical map
	//updatePosition(logicMap, &pacmanPositionAtLogicMap, pacmanDirectionFlag, 9);
	//updatePosition(logicMap, &purpleGhostPositionAtLogicMap, purpleGhostDirectionFlag, 2);
	//updatePosition(logicMap, &brownGhostPositionAtLogicMap, brownGhostDirectionFlag, 3);
	//updatePosition(logicMap, &greenGhostPositionAtLogicMap, greenGhostDirectionFlag, 4);
	//updatePosition(logicMap, &yellowGhostPositionAtLogicMap, yellowGhostDirectionFlag, 5);
}

int main(int argc, char* args[])
{
	if (!initialize())
	{
		printf("\nFailed to initialize!");
	}
	else
	{
		//Generate logic map
		int** logicMap = generateMap();

		//Load actor's textures
		if (!loadMedia()) 
		{
			printf("\nMedia could not be loaded!");
		}
		else 
		{
			bool running = true;
			SDL_Event e;

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

				initializeEngine(logicMap);
				//SDL_Delay(17);

				if (skipGhostsMove) {
					skipGhostsMove = false;
				}
				else {
					skipGhostsMove = true;
				}
			}
		
		}
		//Free memory allocated for logic map
		freeMap(logicMap);
	}

	//Close SDL
	close();
	return 0;
}