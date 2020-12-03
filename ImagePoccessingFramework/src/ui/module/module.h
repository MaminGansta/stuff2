#pragma once

#include "core.h"


namespace Bubble
{
	struct Module
	{
		virtual void Draw(DeltaTime) = 0;
		virtual bool IsOpen() = 0;
		virtual ~Module() = 0;
	};
}