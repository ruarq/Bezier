#pragma once

#include <stdexcept>
#include <iostream>
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
	v2 Point(const float t) const
	{
		return Lerp(std::vector<v2>(points), t);
	}

	v2 Tangent(const float t) const
	{
		v2 a, b;
		Lerp(std::vector<v2>(points), t, &a, &b);
		return (b - a).Norm();
	}

	v2 Normal(const float t) const
	{
		const v2 tangent = Tangent(t);
		return v2(-tangent.y, tangent.x);
	}

	float Length(const unsigned steps) const
	{
		v2 point, prevPoint;
		float length = 0.0f;
		const float stepSize = 1.0f / static_cast<float>(steps);

		for (float step = 0.0f; step <= 1.0f; step += stepSize)
		{
			prevPoint = point;

			point = this->Point(step);

			length += Distance(point, prevPoint);
		}

		return length;
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
	static v2 Lerp(std::vector<v2> &&points, const float t, v2 *a = nullptr, v2 *b = nullptr)
	{
		if (points.size() == 2)
		{
			if (a)
			{
				*a = points[0];
			}

			if (b)
			{
				*b = points[1];
			}

			return ::Lerp(points[0], points[1], t);
		}

		std::vector<v2> subPoints;
		for (auto itr = points.begin(); itr != points.end() - 1; itr++)
		{
			subPoints.push_back(::Lerp(*itr, *(itr + 1), t));
		}

		return Lerp(std::move(subPoints), t, a, b);
	}

	

private:
	std::vector<v2> points;
};