#pragma once

#include <immintrin.h>
#include <algorithm>
#include <omp.h>
#include "image.h"

Image filterNegative(Image input)
{
	Image output;
	output.creatImage(input.getSize().x, input.getSize().y);

	//#pragma omp parallel for 
	for (int y = 0; y < output.getSize().y; y++)
	{
		for (int x = 0; x < output.getSize().x; x++)
		{
			Color c = input.getPixel(x, y);
			output.setPixel(x, y, Color{ unsigned char(255 - c.r),unsigned char(255 - c.g), unsigned char(255 - c.b), 255 });
		}
	}

	return output;
}

Image filterNegativeVec(Image input)
{
	Image output;
	output.creatImage(input.getSize().x, input.getSize().y);

	unsigned char negativ[] {	255,255,255,254, 255,255,255,254, 
								255,255,255,254, 255,255,255,254, 
								255,255,255,254, 255,255,255,254, 
								255,255,255,254, 255,255,255,254, };

	__m256i Y = _mm256_loadu_epi8(&negativ);

	int edge = (output.getSize().x / 8) * 8;

	for (int y = 0; y < output.getSize().y; y++)
	{
		for (int x = 0; x < edge; x += 8)
		{
			auto iterIn = input.getIterPixel(x, y);
			auto iterOut = output.getIterPixel(x, y);

			__m256i X = _mm256_loadu_epi8(&*iterIn);

			__m256i A = _mm256_sub_epi8(Y, X);
			
			_mm256_storeu_epi8(&*iterOut, A);
		}
		for (int x = edge; x < output.getSize().x; x++)
		{
			auto iterIn = input.getIterPixel(x, y);
			auto iterOut = output.getIterPixel(x, y);

			iterOut[0] = 255 - iterIn[0];
			iterOut[1] = 255 - iterIn[1];
			iterOut[2] = 255 - iterIn[2];
			iterOut[3] = 255;
		}
	}

	return output;
}

Image filterMedian(Image input, unsigned int kernal)
{
	Image output;
	output.creatImage(input.getSize().x, input.getSize().y);

	int radios = kernal / 2;

	//#pragma omp parallel for collapse(2)
	for (int y = 0; y < output.getSize().y; y++)
	{
		for (int x = 0; x < output.getSize().x; x++)
		{
			std::vector<unsigned char> r(kernal * kernal);
			std::vector<unsigned char> g(kernal * kernal);
			std::vector<unsigned char> b(kernal * kernal);

			int iter = 0;
			for (int i = -radios; i < radios; i++)
			{
				for (int j = -radios; j < radios; j++)
				{
					int ry = y + i;
					int rx = x + j;

					if (rx < 0) rx = 0;
					if (ry < 0) ry = 0;
					if (output.getSize().x <= rx) rx = output.getSize().x - 1;
					if (output.getSize().y <= ry) ry = output.getSize().y - 1;

					Color c = input.getPixel(rx, ry);
					r[iter] = c.r;
					g[iter] = c.g;
					b[iter] = c.b;

					iter++;
				}
			}

			std::sort(r.begin(), r.end()); 
			std::sort(g.begin(), g.end());
			std::sort(b.begin(), b.end());

			output.setPixel(x, y, Color{ r[r.size() / 2], g[g.size() / 2], b[b.size() / 2], 255 });
		}
	}

	return output;
}

Image filterMedianVec(Image input, unsigned int kernal)
{
	Image output;
	output.creatImage(input.getSize().x, input.getSize().y);

	int radios = kernal / 2;
	std::vector<unsigned short> r(kernal * kernal);
	std::vector<unsigned short> g(kernal * kernal);
	std::vector<unsigned short> b(kernal * kernal);

	std::vector<unsigned short>::iterator R, G, B;

	for (int y = 0; y < output.getSize().y; y++)
	{
		for (int x = 0; x < output.getSize().x; x++)
		{
			for (int i = 0; i < radios * 2; i++)
			{
				for (int j = 0 ; j < (radios * 2 / 16) * 16; j += 16)
				{
					int ry = y + i - radios;
					int rx = x + j - radios;

					if (rx < 0) rx = 0;
					if (ry < 0) ry = 0;
					if (output.getSize().x <= rx) rx = output.getSize().x - 1;
					if (output.getSize().y <= ry) ry = output.getSize().y - 1;

					R = input.getIterR(rx, ry);
					G = input.getIterG(rx, ry);
					B = input.getIterB(rx, ry);

					_mm256_store_si256((__m256i*) &r[j + i * radios * 2], _mm256_load_si256((__m256i*) &R[0] + j));
					_mm256_store_si256((__m256i*) &g[j + i * radios * 2], _mm256_load_si256((__m256i*) &G[0] + j));
					_mm256_store_si256((__m256i*) &b[j + i * radios * 2], _mm256_load_si256((__m256i*) &B[0] + j));
				}

				for (int j = (radios * 2 / 16) * 16; j < radios * 2; i++)
				{
					int ry = y + i - radios;
					int rx = x + j - radios;

					if (rx < 0) rx = 0;
					if (ry < 0) ry = 0;
					if (output.getSize().x <= rx) rx = output.getSize().x - 1;
					if (output.getSize().y <= ry) ry = output.getSize().y - 1;

					Color c = input.getPixel(rx, ry);
					r[j + i * radios * 2] = c.r;
					g[j + i * radios * 2] = c.g;
					b[j + i * radios * 2] = c.b;
				}
			}

			std::sort(r.begin(), r.end());
			std::sort(g.begin(), g.end());
			std::sort(b.begin(), b.end());

			auto iterOut = output.getIterPixel(x, y);

			iterOut[0] = (unsigned char)r[r.size() / 2];
			iterOut[1] = (unsigned char)g[g.size() / 2];
			iterOut[2] = (unsigned char)b[b.size() / 2];
			iterOut[3] = 255;
		}
	}

	return output;
}