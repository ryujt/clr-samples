﻿#pragma once

#include "MethodList.hpp"
#include "PerformanceCounter.hpp"

using namespace std;

class CallHistory {
public:
    static CallHistory& getInstance()
    {
        static CallHistory instance;
        return instance;
    }

    void Enter(FunctionID functionId)
    {
        PerformanceCounter::getIncetance().enter();
        _addRecord(functionId, _callDepth++, 0, CallType::Enter);
    }

    void Leave(FunctionID functionId)
    {
        double duration = PerformanceCounter::getIncetance().leave();
        _addRecord(functionId, --_callDepth, duration, CallType::Leave);
        if (_callDepth == 0) _printTree();
    }

    void Tailcall(FunctionID functionId)
    {
        _addRecord(functionId, _callDepth - 1, 0, CallType::Leave);
        _addRecord(functionId, _callDepth, 0, CallType::Tailcall);
        if (_callDepth == 0) _printTree();
    }

private:
    CallHistory() = default;
    ~CallHistory() = default;

    enum class CallType { Enter, Leave, Tailcall };
    struct CallRecord {
        FunctionID functionId;
        int depth;
        double duration;
        CallType type;
    };

    vector<CallRecord> _callHistory;
    int _callDepth = 0;

    wstring _padding(int count) {
        wstring result = L"";
        for (int i = 0; i < count; i++) result += L"    ";
        return result;
    }

    void _addRecord(FunctionID functionId, int depth, double duration, CallType type) {
        constexpr double MIN_TICKS_THRESHOLD = 100.0;

        if (type == CallType::Leave && duration <= MIN_TICKS_THRESHOLD) {
            if (!_callHistory.empty() && _callHistory.back().type == CallType::Enter && _callHistory.back().functionId == functionId) {
                _callHistory.pop_back();
            }
        }
        else {
            _callHistory.push_back({ functionId, depth, duration, type });
        }
    }

    void _printTree()
    {
        for (const auto& record : _callHistory) {
            wstring name = MethodList::getIncetance().GetName(record.functionId);
            wstring prefix = _padding(record.depth);
            wstring duration = to_wstring(static_cast<int>(record.duration)) + L" ticks";

            switch (record.type) {
            case CallType::Enter:
                UdpSocket::getInstance().sendText(prefix + L"Enter: " + name);
                break;
            case CallType::Leave:
                UdpSocket::getInstance().sendText(prefix + L"Leave: " + name + L", Duration: " + duration);
                break;
            case CallType::Tailcall:
                UdpSocket::getInstance().sendText(prefix + L"Tailcall: " + name);
                break;
            }
        }
    }
};
