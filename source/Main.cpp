#include <cmath>
#include <ctime>
#include <queue>
#include <vector>
#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "Vector2.hpp"
#include "Lerp.hpp"
#include "Bezier.hpp"

void RenderPoint(SDL_Renderer *renderer, const v2 &point)
{
	SDL_Rect rect;
	rect.w = 5;
	rect.h = 5;
	rect.x = point.x - rect.w / 2;
	rect.y = point.y - rect.h / 2;
	SDL_RenderDrawRect(renderer, &rect);
}

v2 RenderBezier(SDL_Renderer *renderer, const Bezier &bezier, const float t, unsigned long size = 0)
{
	if (size == 0)
	{
		size = bezier.Size();
	}

	if (bezier.Size() == 2)
	{
		const v2 p = Lerp(bezier[0], bezier[1], t);

		SDL_SetRenderDrawColor(renderer, 55, 55, 55, 255);
		SDL_RenderDrawLine(renderer, bezier[0].x, bezier[0].y, bezier[1].x, bezier[1].y);

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		RenderPoint(renderer, p);

		return p;
	}

	Bezier subBezier(bezier.Size() - 1);
	for (auto itr = bezier.begin(); itr != bezier.end() - 1; itr++)
	{
		const v2 lerp = Lerp(*itr, *(itr + 1), t);
		subBezier[std::distance(bezier.begin(), itr)] = lerp;

		// draw lines in gray
		const float gradient = static_cast<float>(bezier.Size()) / static_cast<float>(size);
		SDL_SetRenderDrawColor(renderer, 55 + 50 * gradient, 55 + 60 * gradient, 55 + 70 * gradient, 255);
		SDL_RenderDrawLine(renderer, itr->x, itr->y, (itr + 1)->x, (itr + 1)->y);

		// draw points in red
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		RenderPoint(renderer, lerp);
		RenderPoint(renderer, *itr);
		RenderPoint(renderer, *(itr + 1));
	}

	return RenderBezier(renderer, subBezier, t, size);
}

enum class State
{
	Draw,
	Show
};

int main()
{
	std::srand(std::time(nullptr));

	/**
	 * init sdl
	 */
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);

	SDL_Window *window = SDL_CreateWindow(
		"Bezier Curves - C++ & SDL2",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		1280, 720,
		SDL_WINDOW_SHOWN);

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	/**
	 * game state stuff
	 */
	State state = State::Draw;
	const float showStateShow = 6.0f;
	const float showStateDraw = 5.0f;
	bool running = true;
	unsigned ticks = SDL_GetTicks(), prevTicks;

	const std::vector<unsigned long> bezierSizes = {
		3, 4, 5, 6, 7, 8, 9, 10,
		12, 14, 16, 17, 18, 20,
		20, 25, 30,
		40, 50, 60, 70, 80,
		100
	};
	auto currentSize = bezierSizes.begin();

	unsigned count = 0;
	float t = 0.0f;

	/**
	 * create the bezier curve and points to draw it
	 */
	Bezier bezier(*currentSize);
	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	for (v2 &point : bezier)
	{
		point = v2(std::rand() % w, std::rand() % h);
	}

	std::vector<SDL_Point> points;

	/**
	 * load a racecar sprite to move along the tracks
	 */
	SDL_Texture *car = IMG_LoadTexture(renderer, "res/car.png");

	/**
	 * main loop
	 */
	while (running)
	{
		/**
		 * calculate delta time
		 */
		prevTicks = ticks;
		ticks = SDL_GetTicks();
		const float dt = static_cast<float>(ticks - prevTicks) / 1000.0f;
		
		/**
		 * update t based on state & generate a new bezier curve based on state
		 */
		if (state == State::Draw)
		{
			t += dt / showStateDraw;

			if (t > 1.0f)
			{
				t = 0.0f;
				state = State::Show;
			}
		}
		else if (state == State::Show)
		{
			t += dt / showStateShow;

			if (t > 1.0f)
			{
				t = 0.0f;
				state = State::Draw;
				count++;
				points.clear();

				SDL_GetWindowSize(window, &w, &h);

				bezier.Resize(*currentSize);
				for (v2 &point : bezier)
				{
					point = v2(std::rand() % w, std::rand() % h);
				}
			}
		}

		/**
		 * change the degree of the bezier curve every 5 new curves
		 */
		if (count >= 5)
		{
			count = 0;
			SDL_GetWindowSize(window, &w, &h);
			bezier.Push(v2(std::rand() % w, std::rand() % h));
			currentSize++;

			if (currentSize >= bezierSizes.end())
			{
				currentSize = bezierSizes.begin();
			}
		}

		/**
		 * handle events
		 */
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				running = false;
				break;
			
			default:
				break;
			}
		}

		/**
		 * clear window to black
		 */
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		/**
		 * draw the bezier curve
		 */
		if (state == State::Draw)
		{
			/**
			 * draw the bezier curve stuff (line segments, points, normals, tangents, all that good stuff)
			 */
			const v2 p = RenderBezier(renderer, bezier, t);
			v2 tangent = bezier.Tangent(t) * 20.0f;
			v2 normal = bezier.Normal(t) * 20.0f;

			SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
			SDL_RenderDrawLine(renderer, p.x, p.y, p.x + tangent.x, p.y + tangent.y);

			SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
			SDL_RenderDrawLine(renderer, p.x, p.y, p.x + normal.x, p.y + normal.y);

			/**
			 * add the point to the "points" list which represents the path of the cruve
			 */
			if (points.empty())
			{
				points.push_back(SDL_Point{ int(p.x), int(p.y) });
			}

			if (Distance2(p, v2(points.back().x, points.back().y)) >= 2.0f)
			{
				points.push_back(SDL_Point{ int(p.x), int(p.y) });
			}
		}

		/**
		 * show the bezier curve
		 */
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderDrawLines(renderer, points.data(), points.size());

		/**
		 * draw a beautiful car
		 */
		if (state == State::Show)
		{
			const v2 p = bezier.Point(t);
			const v2 tangent = bezier.Tangent(t);

			SDL_Rect carRect;
			carRect.w = 25;
			carRect.h = 12;
			carRect.x = p.x - carRect.w / 2;
			carRect.y = p.y - carRect.h / 2;
			SDL_RenderCopyEx(renderer, car, nullptr, &carRect, std::atan2(tangent.y, tangent.x) * 180.0f / M_PI, nullptr, SDL_FLIP_NONE);
		}

		/**
		 * swap the screen buffers
		 */
		SDL_RenderPresent(renderer);
	}

	/**
	 * destroy the car
	 */
	SDL_DestroyTexture(car);

	/**
	 * quit sdl
	 */
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	IMG_Quit();
	SDL_Quit();

	return 0;
}