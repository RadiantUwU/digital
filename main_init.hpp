#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

#include "Awaitable.hpp"
#include "Thread.hpp"

namespace LogicSim {
    using std::vector;
    using std::string;
    using std::unordered_map;
    using std::unique_ptr;

    inline void memcpy(void* src, void* pst, size_t n) {
        char* sp = (char*)src;
        char* pp = (char*)pst;
        for (size_t i = 0; i < n; i++) 
            pp[i] = sp[i];
        
    }

    class ConfigEntry {
    private:
        unique_ptr<void> ptr;
        string type;
    public:
        template<typename T>
        ConfigEntry(T obj, string type) {
            T* new_obj = malloc(sizeof(T));
            memcpy(&obj,new_obj,sizeof(T));
            ptr.reset(new_obj);
        }
        string getType() { return type; }
        void* getPtr() {
            return ptr.get();
        }
    };
    using ConfigTable = std::unordered_map<string, ConfigEntry>;

    class BasicGate;
    class Wire;
    class WireState;
    class SubCircuit;
    class Pin;

    class BasicGate {
        vector<Pin*> pins;
    };
    class Wire;
    class WireState;
    class SubCircuit;
    class Pin;
    void connect(Pin*,Wire*);
    void disconnect(Pin*, Wire*);
};