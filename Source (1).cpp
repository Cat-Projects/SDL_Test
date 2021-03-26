#define _USE_MATH_DEFINES
#include<iostream>
#include<SDL.h>

SDL_Window* win = NULL;
SDL_Renderer* ren = NULL;

int win_width = 1000, win_height = 800;

void DeInit(int error)
{
	if (ren != NULL) SDL_DestroyRenderer(ren);
	if (win != NULL) SDL_DestroyWindow(win);
	SDL_Quit();
	exit(error);
}

void Init()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		printf("Couldn't init SDL! Error: %s", SDL_GetError());
		system("pause");
		DeInit(1);
	}

	win = SDL_CreateWindow("Just another window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		win_width, win_height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (win == NULL)
	{
		printf("Couldn't create window! Error: %s", SDL_GetError());
		system("pause");
		DeInit(1);
	}

	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	if (ren == NULL)
	{
		printf("Couldn't create renderer! Error: %s", SDL_GetError());
		system("pause");
		DeInit(1);
	}
}

double circle(double x, double x0, double y0, double rad)
{
	return sqrt(rad * rad - (x - x0) * (x - x0)) + y0;
}

void mathCoordsToScreen(double x, double y, double scale, int centerx, int centery, int &sx, int &sy)
{
	sx = round(centerx + x * scale);
	sy = round(centery - y * scale);
}

int main(int argc, char* argv[])
{
	Init();

	double scale = 1.5;

	double x1, y1, x2, y2;
	int sx1, sy1, sx2, sy2;

	SDL_Rect r = { 0, 0, 3, 3 };

	int wanted_points = 3;
	bool rising = true;

	SDL_Event ev;
	bool isRunning = true;

	int coords_x = win_width / 2;
	int coords_y = win_height / 2;

	int mouse_x = win_width / 2;
	int mouse_y = win_height / 2;

	float cubic_x, cubic_y;
	float speed = 700.0f;
	cubic_x = win_width / 2;
	cubic_y = win_height / 2;
	SDL_Rect cubic = { cubic_x, cubic_y, 50, 50 };


	bool isUpPressed = false;
	bool isDownPressed = false;
	bool isRightPressed = false;
	bool isLeftPressed = false;

	const Uint8* state = SDL_GetKeyboardState(NULL);
	int FPS = 30;

	while (isRunning)
	{
		while (SDL_PollEvent(&ev))
		{
			switch (ev.type)
			{
			case SDL_QUIT:
				isRunning = false;
				break;

			case SDL_WINDOWEVENT:
				if (ev.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				{
					win_width = ev.window.data1;
					win_height = ev.window.data2;
				}
				break;

			case SDL_MOUSEBUTTONDOWN:
				if (ev.button.button == SDL_BUTTON_LEFT)
				{
					coords_x = ev.button.x;
					coords_y = ev.button.y;
				}
				break;

			case SDL_MOUSEMOTION:
				mouse_x = ev.motion.x;
				mouse_y = ev.motion.y;
				break;

			case SDL_KEYDOWN:
				switch (ev.key.keysym.scancode)
				{
				case SDL_SCANCODE_ESCAPE:
					isRunning = false;
					break;
				}
				break;

			}
		}
		
		if (state[SDL_SCANCODE_UP] && !state[SDL_SCANCODE_DOWN]) coords_y -= 5;
		if (!state[SDL_SCANCODE_UP] && state[SDL_SCANCODE_DOWN]) coords_y += 5;
		if (state[SDL_SCANCODE_RIGHT]) coords_x += 5;
		if (state[SDL_SCANCODE_LEFT]) coords_x -= 5;

		// ---------------------------------------------------------------------------
		
		float x = mouse_x - cubic_x;
		float y = mouse_y - cubic_y;
		float len = sqrt(x * x + y * y);
		if (len > speed / FPS)
		{
			x /= len;
			y /= len;

			cubic_x += x * speed / FPS;
			cubic_y += y * speed / FPS;
		}
		else
		{
			cubic_x = mouse_x;
			cubic_y = mouse_y;
		}

		cubic.x = cubic_x;
		cubic.y = cubic_y;
		
		// ---------------------------------------------------------------------------

		#pragma region DRAWING
		SDL_SetRenderDrawColor(ren, 200, 200, 200, 255);
		SDL_RenderClear(ren);

		SDL_SetRenderDrawColor(ren, 0, 0, 255, 255);

		x1 = -250; x2 = 250;
		y1 = 0; y2 = 0;

		mathCoordsToScreen(x1, y1, scale, coords_x, coords_y, sx1, sy1);
		mathCoordsToScreen(x2, y2, scale, coords_x, coords_y, sx2, sy2);

		SDL_RenderDrawLine(ren, sx1, sy1, sx2, sy2);

		y1 = -250; y2 = 250;
		x1 = 0; x2 = 0;

		mathCoordsToScreen(x1, y1, scale, coords_x, coords_y, sx1, sy1);
		mathCoordsToScreen(x2, y2, scale, coords_x, coords_y, sx2, sy2);

		SDL_RenderDrawLine(ren, sx1, sy1, sx2, sy2);


		int point_count = wanted_points;
		SDL_Point* points = (SDL_Point *) malloc(sizeof(SDL_Point)*(point_count+1));

		float alpha = 0;
		for (int i = 0; i < point_count; i++)
		{
			alpha += 2*M_PI/point_count;
			mathCoordsToScreen(200 * cos(alpha), 200 * sin(alpha), scale, coords_x, coords_y, points[i].x, points[i].y);
		}
		points[point_count] = points[0];

		SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
		SDL_RenderDrawLines(ren, points, point_count+1);

		free(points);

		if (rising)
			wanted_points++;
		else
			wanted_points--;

		if (rising && wanted_points > 30 || !rising && wanted_points <= 3)
			rising = !rising;

		SDL_SetRenderDrawColor(ren, 255, 180, 0, 255);
		SDL_RenderFillRect(ren, &cubic);
#pragma endregion

		SDL_RenderPresent(ren);
		SDL_Delay(1000/FPS);
	}

	DeInit(0);
	return 0;
}