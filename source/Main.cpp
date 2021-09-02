#include <cmath>
#include <ctime>
#include <queue>
#include <vector>

#include <SDL2/SDL.h>

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

	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window *window = SDL_CreateWindow(
		"Bezier Curves - C++ & SDL2",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		1920, 1200,
		SDL_WINDOW_FULLSCREEN);

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	State state = State::Show;
	const float showStateShow = 1.5f;
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
	Bezier bezier(*currentSize);

	std::vector<SDL_Point> points;

	while (running)
	{
		prevTicks = ticks;
		ticks = SDL_GetTicks();
		const float dt = static_cast<float>(ticks - prevTicks) / 1000.0f;
		
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
			t += dt;

			if (t > showStateShow)
			{
				t = 0.0f;
				state = State::Draw;
				count++;
				points.clear();

				int w, h;
				SDL_GetWindowSize(window, &w, &h);

				bezier.Resize(*currentSize);
				for (v2 &point : bezier)
				{
					point = v2(std::rand() % w, std::rand() % h);
				}
			}
		}

		if (count >= 5)
		{
			count = 0;
			int w, h;
			SDL_GetWindowSize(window, &w, &h);
			bezier.Push(v2(std::rand() % w, std::rand() % h));
			currentSize++;

			if (currentSize >= bezierSizes.end())
			{
				currentSize = bezierSizes.begin();
			}
		}

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

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		if (state == State::Draw)
		{
			const v2 p = RenderBezier(renderer, bezier, t);

			if (points.empty())
			{
				points.push_back(SDL_Point{ int(p.x), int(p.y) });
			}

			if (Distance2(p, v2(points.back().x, points.back().y)) >= 2.0f)
			{
				points.push_back(SDL_Point{ int(p.x), int(p.y) });
			}
		}

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderDrawLines(renderer, points.data(), points.size());

		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}