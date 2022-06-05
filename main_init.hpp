#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <exception>

#include "Awaitable.hpp"
#include "Thread.hpp"

namespace LogicSim {
    using std::vector;
    using std::string;
    using std::unordered_map;
    using std::unique_ptr;
    using std::shared_ptr;
    using std::make_shared;
    using std::exception;
    using std::runtime_error;
    vector<LogicSimObject*> objects;
    class LogicSimObject {
    public:
        virtual const char* getObjectType() {
            return "LogicSimObject";
        }
        LogicSimObject() {
            objects.push_back(this);
        }
        ~LogicSimObject() {
            objects.erase(std::remove(objects.begin(), objects.end(), this), objects.end());
        }
    };

    class LogicSimException : public runtime_error {
    public:
        LogicSimException(const string& what, LogicSimObject* obj) : runtime_error(what + ": " + obj->getObjectType()) {};
        LogicSimException(const string& what) : runtime_error(what) {};
        
    };
    namespace Exceptions {
        class ShortCircuitError;
    };

    inline void memcpy(void* src, void* pst, size_t n) {
        char* sp = (char*)src;
        char* pp = (char*)pst;
        for (size_t i = 0; i < n; i++) 
            pp[i] = sp[i];
        
    }

    class ConfigEntry : public LogicSimObject {
    private:
        unique_ptr<void> ptr;
        const char* type;
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
        virtual const char* getObjectType() override {
            return "ConfigEntry";
        }
    };
    class ConfigTable final : public LogicSimObject {
    private:
        unordered_map<string, ConfigEntry*> entries;
    public:
        virtual const char* getObjectType() {
            return "ConfigTable";
        }
        ConfigTable() {
            entries.emplace("", new ConfigEntry(0, ""));
        }
    };


    class BasicGate;
    class Wire;
    class WireState;
    class SubCircuit;
    class Pin;
    
    class WireStateValueValue : public LogicSimObject {
    public:
        virtual const char* getObjectType() {
            return "WireStateValueValue";
        }
        unsigned short resistance = 0;
        unsigned short value = 0;
        WireStateValueValue() {};
        WireStateValueValue(unsigned short v) : value(v) {};
        WireStateValueValue(unsigned short r, unsigned short v) : resistance(r), value(v) {};
        
    };
    class WireStateValue : public LogicSimObject {
    public:
        virtual const char* getObjectType() {
            return "WireStateValue";
        }
        
    };
    class WireState : public LogicSimObject {
    private:
        std::vector<WireStateValue*> values;
    public:
        virtual const char* getObjectType() {
            return "WireState";
        }
    };
    
    class Pin  : public LogicSimObject {
        int pin_num;
        string name;
    public:
        virtual const char* getObjectType() {
            return "Pin";
        }
    };
    class BasicGate {
        vector<Pin> pins;
    public:
        ConfigTable config;
        BasicGate() {};
    };
    class Wire;
    class SubCircuit;
    void connect(Pin*,Wire*);
    void disconnect(Pin*, Wire*);
};