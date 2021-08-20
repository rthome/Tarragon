#pragma once

#include <functional>
#include <unordered_map>

#include "common.h"

namespace tarragon
{
    //
    // Signal stuff
    //
    
    template <typename... Args>
    class Signal;

    template <typename... Args>
    class SignalSource;

    // Signal source, for invoking callbacks
    template <typename... Args>
    class SignalSource
    {
        friend class Signal<Args...>;

    public:
        using Callback = std::function<void(Args...)>;

    private:
        IncId<int32_t> m_ids;
        std::unordered_map<int32_t, Callback> m_callbacks;

    public:
        SignalSource(SignalSource const&) = delete;
        SignalSource& operator= (SignalSource const&) = delete;

        SignalSource() = default;

        void publish(Args... arg) const
        {
            for (const auto& [_, callback] : m_callbacks)
                std::invoke(callback, arg...);
        }
    };

    // Signal to connect callbacks to
    template <typename... Args>
    class Signal
    {
    public:
        using SignalSourceType = SignalSource<Args...>;

    private:
        SignalSourceType *m_psignal;

    public:
        Signal(Signal const&) = delete;
        Signal& operator= (Signal const&) = delete;

        Signal(SignalSourceType& source)
            : m_psignal{ &source }
        { }

        int32_t attach(typename SignalSourceType::Callback callback)
        {
            auto id = m_psignal->m_ids();
            m_psignal->m_callbacks[id] = callback;

            return id;
        }

        bool detach(int32_t id)
        {
            return m_psignal->m_callbacks.erase(id) == 1;
        }
    };

    // deduction guide
    template<typename... Args>
    Signal(SignalSource<Args...> &) -> Signal<Args...>;
}
