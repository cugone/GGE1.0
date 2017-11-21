#pragma once

#include "Engine/EngineConfig.hpp"

#include "Engine/Core/Time.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

class ProfileLogScope {
public:
    ProfileLogScope(const char* scopeName);
	~ProfileLogScope();
protected:
private:
	const char* m_scopeName;
    double m_timeAtStart;
};

#if !defined PROFILE_LOG_SCOPE && !defined PROFILE_LOG_SCOPE_FUNCTION
#define PROFILE_LOG_SCOPE(tag_str) ProfileLogScope __pscope_##__LINE__##(tag_str)
#define PROFILE_LOG_SCOPE_FUNCTION() PROFILE_LOG_SCOPE(__FUNCTION__)
#endif