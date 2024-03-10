#include "math.hpp"

namespace Math
{
	int getAverage(const std::vector<int>& list)
	{
		return std::accumulate(list.begin(), list.end(), 0) / list.size();
	}

	int Random(int lb, int hb)
	{
		return lb + rand() % (hb - lb + 1);
	}
	float Random(float lb, float hb)
	{
		std::random_device rd; // Obtain a random number from hardware
		std::mt19937 gen(rd()); // Seed the generator
		std::uniform_real_distribution<float> dis(lb, hb); // Define the range

		// Generate and return a random float
		return dis(gen);
	}

	bool intersect(SDL_Rect& rect1, SDL_Rect& rect2)
	{
		return rect1.x <= rect2.x + rect2.w &&
			rect1.x + rect1.w >= rect2.x &&
			rect1.y <= rect2.y + rect2.h &&
			rect1.y + rect1.h >= rect2.y;
	}
	void lag()
	{
		for (int i = 1; i < 1000000; ++i)
		{
			int z = sin(cos(tan(sqrt(sin(cos(tan(sqrt(tan(sqrt(cos(sin(i))))))))))));
			z += sin(sqrt(sin(cos(z / 7))));
			z += sin(sqrt(sin(cos(z * 30))));
			z += sin(sqrt(sin(cos(z / 7))));
			z += sin(sqrt(sin(cos(z * 75))));
			z += sin(sqrt(sin(cos(z / 7))));
			z += sin(sqrt(sin(cos(z * 0.00001f))));
			z += sin(sqrt(sin(cos(z / Random(1, 20)))));
			z += sin(sqrt(sin(cos(z * Random(1, 20)))));
			z += sin(sqrt(sin(cos(z / Random(1, 20)))));
			z += sin(sqrt(sin(cos(z * Random(1, 20)))));
			z += sin(sqrt(sin(cos(z / Random(1, 20)))));
			z += sin(sqrt(sin(cos(z / Random(1, 20)))));
		}
	}
}

constexpr double operator""_sq(unsigned long long x)
{
	return x * x;
}