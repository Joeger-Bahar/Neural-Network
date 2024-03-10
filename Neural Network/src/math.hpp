#pragma once

#include <iostream>
#include <numeric>
#include <vector>
#include <random>

#include <SDL.h>

struct Vector2
{
	Vector2() : x(0.0f), y(0.0f) {}
	Vector2(double x, double y) : x(x), y(y) {}

	void normalize()
	{
		double distance = sqrt(x * x + y * y);

		if (distance)
		{
			x /= distance;
			y /= distance;
		}
	}

	double dot(const Vector2& other) { return this->x * other.x + this->y * other.y; }

	int cross(const Vector2& other) { return (this->x * other.y) - (this->y * other.x); }

	Vector2 operator=(const Vector2& other)
	{
		return { other.x, other.y };
	}

	double length() { return sqrt(this->x * this->x + this->y * this->y); }

	friend std::ostream& operator<<(std::ostream& os, Vector2& v)
	{
		os << v.x << ", " << v.y << "\n";
		return os;
	}

	inline operator bool() const { return (this->x && this->y); }
	inline bool operator!() const { return (!this->x && !this->y); }

	inline bool operator<(const Vector2 other) const { return (this->x < other.x && this->y < other.y); }
	inline bool operator>(const Vector2 other) const { return (this->x > other.x && this->y > other.y); }
	inline bool operator<=(const Vector2 other) const { return (this->x <= other.x && this->y <= other.y); }
	inline bool operator>=(const Vector2 other) const { return (this->x >= other.x && this->y >= other.y); }
	inline bool operator==(const Vector2 other) const { return (this->x == other.x && this->y == other.y); }
	inline bool operator!=(const Vector2 other) const { return (this->x != other.x || this->y != other.y); }

	inline bool operator&&(const Vector2& other) const { return (this->x && this->y && other.x && other.y); }
	inline bool operator||(const Vector2& other) const { return (this->x || this->y || other.x || other.y); }

	inline void operator++() { ++x; ++y; }
	inline void operator++(int s) { ++x; ++y; }
	inline void operator--() { --x; --y; }
	inline void operator--(int s) { --x; --y; }
	inline void operator*=(double s) { x *= s; y *= s; }
	inline void operator/=(double s) { x /= s; y /= s; }
	inline void operator+=(double s) { x += s; y += s; }
	inline void operator-=(double s) { x -= s; y -= s; }
	inline void operator%=(double s) { x = std::fmod(x, s); y = std::fmod(y, s); }

	inline Vector2 operator-() const { return { -x, -y }; }
	inline Vector2 operator+(float a) { return { this->x + a, this->y + a }; }
	inline Vector2 operator-(float a) { return { this->x - a, this->y - a }; }
	inline Vector2 operator*(float a) { return { this->x * a, this->y * a }; }
	inline Vector2 operator/(float a) { return { this->x / a, this->y / a }; }

	union
	{
		double x;
		double w;
	};
	union
	{
		double y;
		double h;
	};
};

inline Vector2 operator+(Vector2 first, Vector2 second)
{
	return { first.x + second.x, first.y + second.y };
}

inline Vector2 operator-(Vector2 first, Vector2 second)
{
	return { first.x - second.x, first.y - second.y };
}

inline Vector2 operator*(Vector2 first, Vector2 second)
{
	return { first.x * second.x, first.y * second.y };
}

inline Vector2 operator/(Vector2 first, Vector2 second)
{
	return { first.x / second.x, first.y / second.y };
}

namespace Math
{
	int getAverage(const std::vector<int>& list);

	int Random(int lb, int hb);
	float Random(float lb, float hb);

	bool intersect(SDL_Rect& first_rect, SDL_Rect& second_rect);

	void lag();
}

// Same as Vector2
typedef Vector2 Point;

template <>
struct std::hash<Vector2>
{
	std::size_t operator()(const Vector2& vec) const
	{
		using std::size_t;

		return ((std::hash<int>()(vec.x)
			^ (std::hash<int>()(vec.y) << 1)) >> 1);
	}
};