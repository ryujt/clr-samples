#pragma once

#include "UdpSocket.hpp"
#include "WhatapLog.hpp"

#include <atomic>
#include "cor.h"
#include "corprof.h"
#include "CComPtr.h"

class Profiler
{
public:
    static Profiler& getInstance()
    {
        static Profiler instance;
        return instance;
    }

    Profiler(Profiler const&) = delete;
    void operator=(Profiler const&) = delete;

    void Initialize(IUnknown* pICorProfilerInfoUnk)
    {
        UdpSocket::getInstance().setAddress("127.0.0.1", 2222);
        UdpSocket::getInstance().sendText("CorProfiler::Initialize ---->");

        HRESULT hr = pICorProfilerInfoUnk->QueryInterface<ICorProfilerInfo3>(&_profilerInfo);
        if (FAILED(hr)) {
            WhatapLog::Warning("Failed to attach profiler: interface ICorProfilerInfo3 not found.");
            return;
        }

    }

    void JITCompilationStarted(FunctionID functionId, BOOL fIsSafeToBlock)
    {
        if (_profilerInfo == nullptr) return;

        ModuleID moduleId;
        mdToken functionToken = mdTokenNil;
        HRESULT hr = _profilerInfo->GetFunctionInfo(functionId, nullptr, &moduleId, &functionToken);
        if (FAILED(hr)) return;

        // Get metadata interface
        CComPtr<IMetaDataImport> metaDataImport;
        hr = _profilerInfo->GetModuleMetaData(moduleId, ofRead, IID_IMetaDataImport, reinterpret_cast<IUnknown**>(&metaDataImport));
        if (FAILED(hr)) return;

        // Get function name
        mdTypeDef classTypeDef;
        WCHAR functionName[1024];
        ULONG functionNameLen;
        hr = metaDataImport->GetMethodProps(functionToken, &classTypeDef, functionName, 1024, &functionNameLen, nullptr, nullptr, nullptr, nullptr, nullptr);
        if (FAILED(hr)) return;

        // Get class name
        WCHAR className[1024];
        ULONG classNameLen;
        hr = metaDataImport->GetTypeDefProps(classTypeDef, className, 1024, &classNameLen, nullptr, nullptr);
        if (FAILED(hr)) return;

        // Convert WCHAR to std::wstring
        wstring functionNameW(functionName);
        wstring classNameW(className);

        UdpSocket::getInstance().sendText(L"Class: " + classNameW + L", Method: " + functionNameW);
    }

    void ModuleLoadFinished(ModuleID module_id, HRESULT hr_status)
    {
        if (FAILED(hr_status)) return;

//        ModuleMetadataList::getInstance().addModuleMetadata(this->info_, module_id, hr_status);

    }

private:
    Profiler() {
        _profilerInfo = nullptr;
    } 

    ~Profiler() {}

    ICorProfilerInfo3* _profilerInfo;

};
