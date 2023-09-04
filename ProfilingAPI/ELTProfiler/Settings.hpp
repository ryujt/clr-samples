#pragma once

#include <cstdlib>  // for getenv()
#include <string>

using namespace std;

class Settings {
public:
    static Settings& getInstance()
    {
        static Settings instance;
        return instance;
    }

    int getProfileBaseTime() const {
        return profile_basetime;
    }

    const wstring& getNamespacePrefix() const {
        return namespace_prefix;
    }

private:
    Settings() {
        profile_basetime = _getEnvIntDefault(L"PROFILE_BASETIME", 0);
        namespace_prefix = _getEnvStringDefault(L"NAMESPACE_PREFIX", L"");
    }

    ~Settings() = default;

    int _getEnvIntDefault(const wchar_t* name, int defaultValue) {
        const wchar_t* envValue = _wgetenv(name);
        return (envValue ? std::stoi(envValue) : defaultValue);
    }

    wstring _getEnvStringDefault(const wchar_t* name, const wstring& defaultValue) {
        const wchar_t* envValue = _wgetenv(name);
        return (envValue ? wstring(envValue) : defaultValue);
    }

    int profile_basetime;
    wstring namespace_prefix;
};
