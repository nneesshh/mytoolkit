#pragma once
//------------------------------------------------------------------------------
/**
    @class CLRandom
    
    (C) 2016 n.lee
*/
#include <time.h>
#include <string>
#include <vector>

#include "../platform/types.h"

#define LOCAL_RANDOM_MAX	0x7fff

//------------------------------------------------------------------------------
/** 
	@brief CLRandom
 */
class CLRandom {
public:
	CLRandom() {
		_holdRand = (int)time(nullptr);
	}

	~CLRandom() { }

	int Rand() {
		return (((_holdRand = _holdRand * 214013L + 2531011L) >> 16) & LOCAL_RANDOM_MAX);
	}

	int Rand(int min,int max) {
		if ( max==min )
			return min;
		else
			return Rand() % (max - min) + min;
	}

	void SRand (int seed) {
		_holdRand = seed;
	}

	/** Gets a random float value
		* @param[in]   _fMin                           Minimum boundary (inclusive)
		* @param[in]   _fMax                           Maximum boundary (inclusive)
		* @return      random value
	*/
	float GetRandomFloat(float fmin, float fmax) {
		return((float(Rand()) * (float(1.0f / LOCAL_RANDOM_MAX)) * (fmax - fmin)) + fmin);
	}

	/** Gets a random uint32_t value
		* @param[in]   _u32Min                         Minimum boundary (inclusive)
		* @param[in]   _u32Max                         Maximum boundary (inclusive)
		* @return      random value
	*/
	uint32_t GetRandomU32(uint32_t umin, uint32_t umax) {
		uint32_t u32Rand;

		/* Gets raw random number */
		u32Rand = Rand();

		/* Done! */
		return (u32Rand == LOCAL_RANDOM_MAX) ? umax : (uint32_t((float(u32Rand) * (float(1.0f / LOCAL_RANDOM_MAX)) * (float(umax) + 1.0f - float(umin))) + float(umin)));
	}

	/** Gets a random int32_t value
		* @param[in]   _s32Min                         Minimum boundary (inclusive)
		* @param[in]   _s32Max                         Maximum boundary (inclusive)
		* @return      random value
	*/
	int32_t GetRandomS32(int32_t smin, int32_t smax) {
		int32_t u32Rand;

		/* Gets raw random number */
		u32Rand = Rand();

		/* Done! */
		return (u32Rand == LOCAL_RANDOM_MAX) ? smax : (int32_t((float(u32Rand) * (float(1.0f / LOCAL_RANDOM_MAX)) * (float(smax) + 1.0f - float(smin))) + float(smin)));
	}

	std::string GetRandomString(size_t len, int table_idx = 0);

	void GetRandomIndex(int size, int num, std::vector<int>& out);

	double GaussRand() {
		static double V1, V2, S;
		static int phase = 0;
		double X;

		if (phase == 0) {
			do {
				double U1 = (double)rand() / RAND_MAX;
				double U2 = (double)rand() / RAND_MAX;

				V1 = 2 * U1 - 1;
				V2 = 2 * U2 - 1;
				S = V1 * V1 + V2 * V2;
			} while (S >= 1 || S == 0);

			X = V1 * sqrt(-2 * log(S) / S);
		}
		else
			X = V2 * sqrt(-2 * log(S) / S);

		phase = 1 - phase;

		return X; //高斯分布随机数序列的期望为0.0，方差为1.0。若指定期望为E，方差为V，则只需增加： X = X * V + E;
	}

private:
	int	_holdRand;
};

/*EOF*/