#pragma once

#include "Engine/Core/Console.hpp"
#include "Engine/Core/JobSystem.hpp"
#include "Engine/Core/Logger.hpp"
#include "Engine/Core/Profiler.hpp"
#include "Engine/Core/JobSystem.hpp"

#include "Engine/Networking/RemoteCommandService.hpp"

#include "Engine/Physics/ParticleSystem.hpp"

#include "Engine/Renderer/Camera3D.hpp"

#ifdef _DEBUG
#define RENDER_DEBUG
#endif

extern Console* g_theConsole;
extern Logger* g_theFileLogger;
extern Profiler* g_theProfiler;
extern JobSystem* g_theJobSystem;
extern ParticleSystem* g_theParticleSystem;
extern Net::RemoteCommandService* g_theRemoteCommandService;