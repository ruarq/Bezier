#pragma once

#include <stdexcept>
#include <vector>

#include "Vector2.hpp"
#include "Lerp.hpp"

class Bezier final
{
public:
	Bezier(const unsigned long degree)
		: points(degree)
	{
	}

	Bezier(const std::vector<v2> &points)
		: points(points)
	{
	}

public:
	v2 Lerp(const float t)
	{
		return Lerp(std::vector<v2>(points.begin(), points.end()), t);
	}

	std::vector<v2>::iterator begin()
	{
		return points.begin();
	}

	std::vector<v2>::iterator end()
	{
		return points.end();
	}

	std::vector<v2>::const_iterator begin() const
	{
		return points.begin();
	}

	std::vector<v2>::const_iterator end() const
	{
		return points.end();
	}

	v2& At(const unsigned long index)
	{
		return points.at(index);
	}

	const v2& At(const unsigned long index) const
	{
		return points.at(index);
	}

	v2& operator[](const unsigned long index)
	{
		return points[index];
	}

	const v2& operator[](const unsigned long index) const
	{
		return points[index];
	}

	unsigned long Size() const
	{
		return points.size();
	}

	void Push(const v2 &point)
	{
		points.push_back(point);
	}

	void Resize(const unsigned long size)
	{
		points.resize(size);
	}

private:
	static v2 Lerp(std::vector<v2> &&points, const float t)
	{
		if (points.size() == 2)
		{
			return ::Lerp(points[0], points[1], t);
		}

		std::vector<v2> newPoints;
		for (auto itr = points.begin(); itr != points.end() - 1; itr++)
		{
			newPoints.push_back(::Lerp(*itr, *(itr + 1), t));
		}

		return Lerp(std::move(newPoints), t);
	}

private:
	std::vector<v2> points;
};