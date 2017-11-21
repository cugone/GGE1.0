#include "Engine/Core/ProfileLogScope.hpp"

ProfileLogScope::ProfileLogScope(const char* scopeName): m_scopeName(scopeName)
, m_timeAtStart(GetCurrentTimeSeconds()) {
    /* DO NOTHING */
}

ProfileLogScope::~ProfileLogScope() {
    double elapsedSeconds = GetCurrentTimeSeconds() - m_timeAtStart;
    g_theFileLogger->LogTagf("profiler", "ProfileLogScope \"%s\" took %.02f ms\n", m_scopeName, 1000.0 * elapsedSeconds);
}