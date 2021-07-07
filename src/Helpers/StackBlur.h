#pragma once

/*
The Stack Blur Algorithm was invented by Mario Klingemann,
mario@quasimondo.com and described here:
http://incubator.quasimondo.com/processing/fast_blur_deluxe.php

This is C++ RGBA (32 bit color) multi-threaded version
by Victor Laskin (victor.laskin@gmail.com)
More details: http://vitiy.info/stackblur-algorithm-multi-threaded-blur-for-cpp
*/

static constexpr std::array<const uint16_t, 255> stackblur_mul =
{
	512,512,456,512,328,456,335,512,405,328,271,456,388,335,292,512,
	454,405,364,328,298,271,496,456,420,388,360,335,312,292,273,512,
	482,454,428,405,383,364,345,328,312,298,284,271,259,496,475,456,
	437,420,404,388,374,360,347,335,323,312,302,292,282,273,265,512,
	497,482,468,454,441,428,417,405,394,383,373,364,354,345,337,328,
	320,312,305,298,291,284,278,271,265,259,507,496,485,475,465,456,
	446,437,428,420,412,404,396,388,381,374,367,360,354,347,341,335,
	329,323,318,312,307,302,297,292,287,282,278,273,269,265,261,512,
	505,497,489,482,475,468,461,454,447,441,435,428,422,417,411,405,
	399,394,389,383,378,373,368,364,359,354,350,345,341,337,332,328,
	324,320,316,312,309,305,301,298,294,291,287,284,281,278,274,271,
	268,265,262,259,257,507,501,496,491,485,480,475,470,465,460,456,
	451,446,442,437,433,428,424,420,416,412,408,404,400,396,392,388,
	385,381,377,374,370,367,363,360,357,354,350,347,344,341,338,335,
	332,329,326,323,320,318,315,312,310,307,304,302,299,297,294,292,
	289,287,285,282,280,278,275,273,271,269,267,265,263,261,259
};

static constexpr std::array<const uint8_t, 255> stackblur_shr =
{
	9, 11, 12, 13, 13, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 17,
	17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18, 19,
	19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 20, 20, 20,
	20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 21,
	21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
	21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22,
	22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23,
	23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24
};

class StackBlur
{
public:
	StackBlur(uint8_t p_radius, uint32_t p_width, uint32_t p_height) : radius(std::clamp<uint8_t>(p_radius, 2, 254)), width(p_width), height(p_height)
	{
		div = (radius * 2) + 1;
		cores = std::max(1U, std::thread::hardware_concurrency());
		shr_sum = stackblur_shr[radius];
		mul_sum = stackblur_mul[radius];
	}

	void Run(uint8_t* src)
	{
		ImageBuffer stack(div * 4 * cores);
		std::vector<std::thread> workers(cores);

		for (uint32_t core = 0; core < cores; ++core)
		{
			workers[core] = std::thread([&, core]()
				{
					StackBlurThread(1, core, src, &stack[div * 4 * core]);
				});
		}

		for (auto& worker : workers)
		{
			worker.join();
		}

		for (uint32_t core = 0; core < cores; ++core)
		{
			workers[core] = std::thread([&, core]()
				{
					StackBlurThread(2, core, src, &stack[div * 4 * core]);
				});
		}

		for (auto& worker : workers)
		{
			worker.join();
		}
	}

private:
	struct StackRGBA
	{
		StackRGBA& operator +=(const StackRGBA& other)
		{
			this->r += other.r;
			this->g += other.g;
			this->b += other.b;
			this->a += other.a;
			return *this;
		}

		StackRGBA& operator -=(const StackRGBA& other)
		{
			this->r -= other.r;
			this->g -= other.g;
			this->b -= other.b;
			this->a -= other.a;
			return *this;
		}

		StackRGBA& operator -=(const uint8_t* ptr)
		{
			this->r -= ptr[0];
			this->g -= ptr[1];
			this->b -= ptr[2];
			this->a -= ptr[3];
			return *this;
		}

		void Add(const uint8_t* ptr, uint8_t multiplier = 1)
		{
			r += ptr[0] * multiplier;
			g += ptr[1] * multiplier;
			b += ptr[2] * multiplier;
			a += ptr[3] * multiplier;
		}

		void Reset()
		{
			r = 0;
			g = 0;
			b = 0;
			a = 0;
		}

		uint32_t r = 0, g = 0, b = 0, a = 0;
	};

	void InitPtr(uint8_t* ptr, const StackRGBA& sum)
	{
		ptr[0] = static_cast<uint8_t>((sum.r * mul_sum) >> shr_sum);
		ptr[1] = static_cast<uint8_t>((sum.g * mul_sum) >> shr_sum);
		ptr[2] = static_cast<uint8_t>((sum.b * mul_sum) >> shr_sum);
		ptr[3] = static_cast<uint8_t>((sum.a * mul_sum) >> shr_sum);
	}

	void StackBlurThread(uint32_t step, uint32_t core, uint8_t* src, uint8_t* stack)
	{
		StackRGBA sum, sum_in, sum_out;

		uint8_t* stack_ptr;
		uint8_t* src_ptr;
		uint8_t* dst_ptr;

		uint32_t wm = width - 1;
		uint32_t hm = height - 1;
		uint32_t w4 = width * 4;
		uint32_t sp = 0, stack_start = 0;

		if (step == 1)
		{
			uint32_t minY = core * height / cores;
			uint32_t maxY = (core + 1) * height / cores;

			for (uint32_t y = minY; y < maxY; ++y)
			{
				sum.Reset();
				sum_in.Reset();
				sum_out.Reset();

				src_ptr = src + w4 * y;

				for (uint8_t i = 0; i <= radius; ++i)
				{
					stack_ptr = &stack[i * 4];
					memcpy(stack_ptr, src_ptr, 4);
					sum.Add(src_ptr, i + 1);
					sum_out.Add(src_ptr);
				}

				for (uint8_t i = 1; i <= radius; ++i)
				{
					if (i <= wm) src_ptr += 4;
					stack_ptr = &stack[4 * (i + radius)];
					memcpy(stack_ptr, src_ptr, 4);
					sum.Add(src_ptr, radius + 1 - i);
					sum_in.Add(src_ptr);
				}

				sp = radius;
				uint32_t xp = std::min<uint32_t>(radius, wm);
				src_ptr = src + 4 * (xp + y * width);
				dst_ptr = src + y * w4;

				for (uint32_t x = 0; x < width; ++x)
				{
					InitPtr(dst_ptr, sum);
					dst_ptr += 4;

					stack_start = sp + div - radius;
					if (stack_start >= div) stack_start -= div;
					stack_ptr = &stack[4 * stack_start];

					if (xp < wm)
					{
						src_ptr += 4;
						++xp;
					}

					sum -= sum_out;
					sum_out -= stack_ptr;
					memcpy(stack_ptr, src_ptr, 4);
					sum_in.Add(src_ptr);
					sum += sum_in;

					++sp;
					if (sp >= div) sp = 0;
					stack_ptr = &stack[sp * 4];

					sum_out.Add(stack_ptr);
					sum_in -= stack_ptr;
				}
			}
		}
		else if (step == 2)
		{
			uint32_t minX = core * width / cores;
			uint32_t maxX = (core + 1) * width / cores;

			for (uint32_t x = minX; x < maxX; ++x)
			{
				sum.Reset();
				sum_in.Reset();
				sum_out.Reset();

				src_ptr = src + 4 * x;

				for (uint8_t i = 0; i <= radius; ++i)
				{
					stack_ptr = &stack[i * 4];
					memcpy(stack_ptr, src_ptr, 4);
					sum.Add(src_ptr, i + 1);
					sum_out.Add(src_ptr);
				}

				for (uint8_t i = 1; i <= radius; ++i)
				{
					if (i <= hm) src_ptr += w4;
					stack_ptr = &stack[4 * (i + radius)];
					memcpy(stack_ptr, src_ptr, 4);
					sum.Add(src_ptr, radius + 1 - i);
					sum_in.Add(src_ptr);
				}

				sp = radius;
				uint32_t yp = std::min<uint32_t>(radius, hm);
				src_ptr = src + 4 * (x + yp * width);
				dst_ptr = src + 4 * x;

				for (uint32_t y = 0; y < height; ++y)
				{
					InitPtr(dst_ptr, sum);
					dst_ptr += w4;

					stack_start = sp + div - radius;
					if (stack_start >= div) stack_start -= div;
					stack_ptr = &stack[4 * stack_start];

					if (yp < hm)
					{
						src_ptr += w4;
						++yp;
					}

					sum -= sum_out;
					sum_out -= stack_ptr;
					memcpy(stack_ptr, src_ptr, 4);
					sum_in.Add(src_ptr);
					sum += sum_in;

					++sp;
					if (sp >= div) sp = 0;
					stack_ptr = &stack[sp * 4];

					sum_out.Add(stack_ptr);
					sum_in -= stack_ptr;
				}
			}
		}
	}

	uint8_t radius = 0, shr_sum = 0;
	uint16_t mul_sum = 0;
	uint32_t div = 0, cores = 0, width = 0, height = 0;
};
