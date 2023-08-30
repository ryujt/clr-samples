#pragma once

#include "CorProfiler.h"
#include "corhlpr.h"
#include "CComPtr.h"
#include "profiler_pal.h"
#include <unordered_map>
#include <string>

#define NAME_BUFFER_SIZE 1024

using namespace std;

class MethodList
{
public:
    static MethodList& getIncetance()
    {
        static MethodList instance;
        return instance;
    }

    void Initialize(ICorProfilerInfo8* pCorProfilerInfo)
    {
        this->pCorProfilerInfo = pCorProfilerInfo;
    }

    wstring GetName(FunctionID functionId)
    {
        auto it = functionNameCache.find(functionId);
        if (it != functionNameCache.end())
        {
            return it->second;
        }

        try {
            wstring name = _getMethodName(functionId);
            functionNameCache[functionId] = name;
            return name;
        } catch (...) {
            return L"";
        }
    }

private:
    wstring _getMethodName(FunctionID id) {
        if (pCorProfilerInfo == nullptr) return L"";

        IMetaDataImport* pIMetaDataImport = 0;
        mdToken funcToken = 0;
        WCHAR szFunction[NAME_BUFFER_SIZE];
        WCHAR szClass[NAME_BUFFER_SIZE];

        HRESULT hr = pCorProfilerInfo->GetTokenAndMetaDataFromFunction(id, IID_IMetaDataImport, (LPUNKNOWN*)&pIMetaDataImport, &funcToken);
        if (FAILED(hr)) return L"";

        mdTypeDef classTypeDef;
        ULONG cchFunction;
        ULONG cchClass;

        hr = pIMetaDataImport->GetMethodProps(funcToken, &classTypeDef, szFunction, NAME_BUFFER_SIZE, &cchFunction, 0, 0, 0, 0, 0);
        if (FAILED(hr)) return L"";

        hr = pIMetaDataImport->GetTypeDefProps(classTypeDef, szClass, NAME_BUFFER_SIZE, &cchClass, 0, 0);
        if (FAILED(hr)) return L"";

        WCHAR szMethodName[NAME_BUFFER_SIZE];
        _snwprintf_s(szMethodName, NAME_BUFFER_SIZE, NAME_BUFFER_SIZE, L"%s.%s", szClass, szFunction);

        pIMetaDataImport->Release();

        return wstring(szMethodName);
    }

    MethodList() = default;
    ~MethodList() = default;
    MethodList(const MethodList&) = delete;
    MethodList& operator=(const MethodList&) = delete;

    ICorProfilerInfo8* pCorProfilerInfo = nullptr;
    unordered_map<FunctionID, wstring> functionNameCache;
};
