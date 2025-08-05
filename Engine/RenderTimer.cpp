#include "RenderTimer.h"
#include "pch.h"
#include "Core.h"

RenderTimer::RenderTimer() : m_engineTimer(MIYABI::Core::GetInstance().GetEngineTimer())
{
	
}