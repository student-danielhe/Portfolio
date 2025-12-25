#include "Engine/Math/RandomNumberGenerator.hpp"
#include <stdlib.h>

RandomNumberGenerator::RandomNumberGenerator() {

}
int RandomNumberGenerator::RollRandomIntLessThan(int maxNotInclusive)
{
	int num = rand() % (maxNotInclusive);
	return num;
}
int RandomNumberGenerator::RollRandomIntInRange(int minInclusive, int maxInclusive) 
{
	int num = rand() % (maxInclusive - minInclusive+1);
	return num+minInclusive;
}
float RandomNumberGenerator::RollRandomFloatZeroToOne() 
{
	float num =static_cast<float>(rand()) / static_cast < float>(RAND_MAX);
	return num;
}
float RandomNumberGenerator::RollRandomFloatInRange(float minInclusive, float maxInclusive) 
{
	float scale = maxInclusive - minInclusive;
	float num = static_cast<float>(rand()) / static_cast <float>(RAND_MAX);
	num *= scale;
	num += minInclusive;
	return num;

}