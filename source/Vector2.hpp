#pragma once

#include <cmath>
#include <ostream>

template<typename T>
class Vector2 final
{
public:
	Vector2() = default;
	Vector2(const T x, const T y)
		: x(x)
		, y(y)
	{
	}

public:
	T Length() const
	{
		return std::sqrt(this->Length2());
	}

	T Length2() const
	{
		return x * x + y * y;
	}

	Vector2<T> Norm() const
	{
		return Vector2<T>(x / this->Length(), y / this->Length());
	}

public:
	T x, y;
};

template<typename T>
Vector2<T> operator+(const Vector2<T> &a, const Vector2<T> &b)
{
	return Vector2<T>(a.x + b.x, a.y + b.y);
}

template<typename T>
Vector2<T> operator-(const Vector2<T> &a, const Vector2<T> &b)
{
	return Vector2<T>(a.x - b.x, a.y - b.y);
}

template<typename T>
Vector2<T> operator*(const Vector2<T> &a, const Vector2<T> &b)
{
	return Vector2<T>(a.x * b.x, a.y * b.y);
}

template<typename T, typename U>
Vector2<T> operator*(const Vector2<T> &v, const U scalar)
{
	return Vector2<T>(v.x * scalar, v.y * scalar);
}

template<typename T, typename U>
Vector2<T> operator*(const U scalar, const Vector2<T> &v)
{
	return v * scalar;
}

template<typename T>
T Distance2(const Vector2<T> &a, const Vector2<T> &b)
{
	return (b - a).Length2();
}

template<typename T>
T Distance(const Vector2<T> &a, const Vector2<T> &b)
{
	return std::sqrt(Distance2(a, b));
}

template<typename T>
std::ostream& operator<<(std::ostream &out, const Vector2<T> &v)
{
	out << "(" << v.x << "," << v.y << ")";
	return out;
}

using v2 = Vector2<float>;