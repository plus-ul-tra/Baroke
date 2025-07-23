#pragma once

#include <cstdint>
#include <DirectXMath.h>
namespace mycore
{
	//using MessageID = uint32_t;

	enum class MessageID : uint32_t {
		Move = 1,
		Rotate =2,
		Scale = 3,
	};
}