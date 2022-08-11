#pragma once
#include <P7_Client.h>
#include <P7_Trace.h>
#include <stdlib.h>

#include <algorithm>
#include <string>

using std::forward;
using std::wstring;

template <typename... ARGS>
inline void LogIfFailed(HRESULT hr, const wchar_t* line, ARGS&&... args) {
    if (FAILED(hr)) {
        Logger::get()->Error(L"%s with code %x", line, forward<ARGS>(args)...,
                             hr);
    }
}

template <typename... ARGS>
inline void FatalErrorIfFailed(HRESULT hr, const wchar_t* line,
                               ARGS&&... args) {
    if (FAILED(hr)) {
        Logger::get()->Error(L"%s with code %x", line, forward<ARGS>(args)...,
                             hr);
        Window::get()->Terminate();
    }
}

class Logger {
   public:
    inline static Logger* get() {
        static Logger instance;
        return &instance;
    }

    template <typename... ARGS>
    inline void Info(const wchar_t* line, ARGS&&... args) {
        m_trace->P7_INFO(m_module, line, forward<ARGS>(args)..., 0);
    }

    template <typename... ARGS>
    inline void Error(const wchar_t* line, ARGS&&... args) {
        m_trace->P7_ERROR(m_module, line, forward<ARGS>(args)..., 0);
    }

    template <typename... ARGS>
    inline void Debug(const wchar_t* line, ARGS&&... args) {
        m_trace->P7_DEBUG(m_module, line, forward<ARGS>(args)..., 0);
    }

   private:
    Logger() {
        m_client =
            P7_Create_Client(TM("/P7.Sink=FileTxt /P7.Format=\"[%tf] %lv %ms\" "
                                "/P7.Roll=10mb /P7.Files=5"));
        m_trace = P7_Create_Trace(m_client, TM("Trace channel 1"));
    }

    ~Logger() {
        m_trace->Release();
        m_client->Release();
    }

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    IP7_Client* m_client;
    IP7_Trace* m_trace;
    IP7_Trace::hModule m_module;
};