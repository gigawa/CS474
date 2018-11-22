#include <cstdlib>
#include <cmath>
#include <limits>

float generateGaussianNoise(float mu, float sigma)
{
	static const float epsilon = std::numeric_limits<float>::min();
	static const float two_pi = 2.0*3.14159265358979323846;

	thread_local float z1;
	thread_local bool generate;
	generate = !generate;

	if (!generate)
	   return z1 * sigma + mu;

	float u1, u2;
	do
	 {
	   u1 = rand() * (1.0 / RAND_MAX);
	   u2 = rand() * (1.0 / RAND_MAX);
	 }
	while ( u1 <= epsilon );

	float z0;
	z0 = sqrt(-2.0 * log(u1)) * cos(two_pi * u2);
	z1 = sqrt(-2.0 * log(u1)) * sin(two_pi * u2);
	return z0 * sigma + mu;
}
