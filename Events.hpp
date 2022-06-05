#pragma once

#include <thread>
#include <memory>
#include <vector>
#include <tuple>

#include "Awaitable.hpp"

namespace Event_Namespace__ {
    template <typename... Args>
    class Event;
    template <typename... Args>
    class EventHandler;
    template <typename... Args>
    class EventCaller;
    template <typename... Args>
    class Event final {
        std::vector<std::shared_ptr<EventHandler<Args...>>> handles;
        bool callermade = false;
        ArgumentedAwaitable<Args...> awaitable;
        
        template <typename... Args>
        friend class EventHandler;
        template <typename... Args>
        friend class EventCaller;
        
        void InternalCall(Args...);

    public:
        Event() {};
        ~Event();
        Event(const Event&) = delete;
        Event& operator=(const Event&) = delete;

        EventCaller<Args...> createCaller();
        std::shared_ptr<EventHandler<Args...>> Connect(void (*)(void*, Args...), void*);
        std::tuple<Args...> Wait();
    };
    template <typename... Args>
    class EventHandler final {
        Event<Args...>* event;
        bool isDisconnected = false;
        void* instance;
        void (*function)(void*,Args...);
        template <typename... Args>
        friend class Event;
        EventHandler(Event<Args...>* e, void (*f)(void*,Args...), void* instance) : event(e), function(f), instance(instance) {};
        ~EventHandler() {};
    public:
        EventHandler() {
            isDisconnected = true;
        }
        EventHandler(const EventHandler&) = delete;
        EventHandler& operator=(const EventHandler&) = delete;
        void Disconnect() {
            if (!isDisconnected) {
                isDisconnected = true;
                event->handles.erase(std::remove(event->handles.begin(), event->handles.end(), std::shared_ptr<EventHandler<Args...>>(this)), event->handles.end());
            }
        }
    };
    template <typename... Args>
    class EventCaller final {
        Event<Args...>* event;
        template <typename... Args>
        friend class Event;
        EventCaller(Event<Args...>* e) : event(e) {};
        ~EventCaller() {};
    public:
        void operator()(Args... args) {
            event->InternalCall(event, args...);
        }
    };
    template <typename... Args>
    void Event<Args...>::InternalCall(Args... args) {
        ArgumentedAwaitable.notify_all(args...);
        for (auto& handle : handles) {
            std::thread(handle->function, handle->instance, args...).detach();
        }
    }
    template <typename... Args>
    std::shared_ptr<EventHandler<Args...>> Event<Args...>::Connect(void (*f)(void*,Args...), void* instance) {
        auto handle = std::make_shared<Event_Handler<Args...>>(this, f, instance);
        handles.push_back(handle);
        return handle;
    }
    template <typename... Args>
    std::tuple<Args...> Event<Args...>::Wait() {
        return awaitable.wait();
    }
    template <typename... Args>
    EventCaller<Args...> Event<Args...>::createCaller() {
        callermade = true;
        return Event_Caller<Args...>(this);
    }
}
using Event_Namespace__::Event;
using Event_Namespace__::EventHandler;
using Event_Namespace__::EventCaller;