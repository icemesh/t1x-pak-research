#pragma once

#include <cinttypes>

namespace LoadingHeap
{
	//every page in the pak has this size limit
	static const uint32_t s_PageSize = 0x200000; //1024*2048

	static const uint32_t kBigPs3MaxPages = 0xAC; //172
};
