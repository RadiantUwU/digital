#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <exception>

#include "Awaitable.hpp"
#include "to_string.hpp"
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
    bool optimizedmode = false;
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
        LogicSimException(const string& what, LogicSimObject* obj) : runtime_error(what + ": " + obj->getObjectType() + ":" + to_string(obj)) {};
        LogicSimException(const string& what) : runtime_error(what) {};
        
    };
    namespace Exceptions {
        class ShortCircuitError;
        class InvalidKeyError : public LogicSimException {
        public:
            template <typename K>
            InvalidKeyError(LogicSimObject* obj, K key) : 
                LogicSimException("No value found for key: " + to_string(key)
                + " for TableType "
                + obj->getObjectType() + ":" + to_string(obj)) {};
            template <typename K>
            InvalidKeyError(LogicSimObject* obj, K key, LogicSimObject* root) :
                LogicSimException("No value found for key: " + to_string(key)
                + " for TableType "
                + obj->getObjectType() + ":" + to_string(obj)
                + " in " + root->getObjectType() + ":" + to_string(root)) {};
        };
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
        const char* type = "<null>";
    public:
        template<typename T>
        ConfigEntry(T obj, string type) {
            T* new_obj = malloc(sizeof(T));
            memcpy(&obj,new_obj,sizeof(T));
            ptr.reset(new_obj);
        }
        ConfigEntry(nullptr_t) {
            ptr.reset(nullptr);
        }
        string getType() { return type; }
        void* getPtr() {
            return ptr.get();
        }
        bool isNull() {
            return ptr.get() == nullptr;
        }
        virtual const char* getObjectType() override {
            return "ConfigEntry";
        }
        bool is(const ConfigEntry& other) {
            return ptr.get() == other.ptr.get();
        }
    };

    template<typename K, typename V>
    class Table : public LogicSimObject {
    private:
        unordered_map<K, V*> entries;
    public:
        virtual const char* getObjectType() {
            return "Table";
        }
        Table() {}
        ~Table() {
            for (auto& entry : entries) {
                delete entry.second;
            }
        }
        V& operator[](const K& key) {
            auto v = entries.find(key);
            if (v == entries.end()) {
                throw Exceptions::InvalidKeyError(this, key);
            }
            return *v;
        }
        auto begin() {
            return entries.begin();
        }
        auto end() {
            return entries.end();
        }
        bool exists(const K& key) {
            return entries.find(key) != entries.end();
        }
    };
    class ConfigTable final : public Table<string, ConfigEntry> {
    public:
        virtual const char* getObjectType() {
            return "ConfigTable";
        }
    };

    class BasicGate;
    class Wire;
    class WireState;
    class SubCircuit;
    class Pin;

    class WireStateValue final : public LogicSimObject {
    public:
        virtual const char* getObjectType() {
            return "WireStateValue";
        }
        unsigned short resistance = 0;
        union {
            void* ptr;
            unsigned long long ll = 0;
            unsigned long l;
            unsigned short s;
            unsigned char byte;
            bool b;
        };
        const char* type = "<null>";
        WireStateValue() : resistance(-1) {};
        WireStateValue(unsigned char byte) : byte(byte), type("by") {};
        WireStateValue(unsigned short v) : s(v), type("s") {};
        WireStateValue(unsigned long v) : l(v), type("l") {};
        WireStateValue(unsigned long long v) : ll(v), type("ll") {};
        WireStateValue(bool v) : b(v), type("b") {};
        WireStateValue(void* v) : ptr(v), type("ptr") {};
        WireStateValue(const WireStateValue& other) {
            type = other.type;
            ll = other.ll; //capture all bytes
        }
        ~WireStateValue() {
            if (type == "ptr") {
                free(ptr);
            }
        }
        bool isNone() {
            return resistance == -1;
        }
        bool is(const WireStateValue& other) {
            if (type != other.type) {
                return false;
            }
            if (resistance != other.resistance) {
                return false;
            }
            return ll == other.ll;
        }
    };
    std::vector<std::tuple<bool, WireStateValue*> (*)(const WireStateValue&, const WireStateValue&)> wire_state_conflicts_handlers;
    
    inline std::string to_string_v(WireStateValue* v) {
        if (v->isNone()) {
            return "None";
        }
        return (string)"WireStateValue(" + (string)v->type + ":" + to_string(v->ll) + ")";
    }
    namespace Exceptions {
        class ShortCircuitError : public LogicSimException {
        public:
            ShortCircuitError(WireStateValue* a,WireStateValue* b, LogicSimObject* obj) : LogicSimException(
                "short circuit conflict between two wire state values " + to_string(a) 
                + "," + to_string(b) + " at object " + obj->getObjectType() + ":" + to_string(obj)
                ) {};
        };
    }

    class WireState : public LogicSimObject {
    private:
        std::vector<WireStateValue> values;
        Wire* root;
        WireStateValue overridevalue = WireStateValue();
        std::vector<WireStateValue*> getStatesForResistance(unsigned short resistance) {
            std::vector<WireStateValue*> ret;
            for (auto& value : values) {
                if (value.resistance == resistance) {
                    ret.push_back(&value);
                }
            }
            return ret;
        }
        WireStateValue& HandlerCheck(WireStateValue* a, WireStateValue* b) {
            for (auto& handler : wire_state_conflicts_handlers) {
                auto ret = handler(*a, *b);
                if (std::get<0>(ret)) {
                    return *std::get<1>(ret);
                }
            }
            throw Exceptions::ShortCircuitError(a, b, (LogicSimObject*)this->root);
        }
        unsigned short getLowestResistance() {
            return overridevalue.resistance;
        }
        unsigned short getLowestResistanceArr() {
            unsigned short ret = 65535;
            for (auto& value : values) {
                if (value.resistance < ret) {
                    ret = value.resistance;
                }
            }
            return ret;
        }
    public:
        virtual const char* getObjectType() {
            return "WireState";
        }
        WireState(Wire* root) : root(root) {};
        WireStateValue getState() {
            return overridevalue;
        }
        void pushState(WireStateValue value) {
            if (value.resistance == -1) return;
            values.push_back(value);
            if (value.resistance < overridevalue.resistance) {
                overridevalue = value;
                return;
            } else if (value.resistance == overridevalue.resistance) {
                auto arr = getStatesForResistance(value.resistance);
                WireStateValue v = WireStateValue();
                if (arr.size() >= 1) {
                    for (int i = 0; i < arr.size(); i++) {
                        if (i == 0) {
                            v = arr[i];
                        } else {
                            void* c = &HandlerCheck(arr[i], &v);
                            v = *(WireStateValue*)c;
                            delete c;
                        }
                    }
                }
                overridevalue = v;
            } else if (!optimizedmode) {
                auto arr = getStatesForResistance(value.resistance);
                WireStateValue v = WireStateValue();
                if (arr.size() >= 1) {
                    for (int i = 0; i < arr.size(); i++) {
                        if (i == 0) {
                            v = arr[i];
                        } else {
                            void* c = &HandlerCheck(arr[i], &v);
                            v = *(WireStateValue*)c;
                            delete c;
                        }
                    }
                }
            }
        }
        void popState(WireStateValue value) {
            if (value.resistance == -1) return;
            for (auto it = values.begin(); it != values.end(); it++) {
                if (it->is(value)) {
                    values.erase(it);
                    break;
                }
            }
            if (value.is(overridevalue)) {
                overridevalue = WireStateValue();
            }
            if (values.size() == 0) return;
            auto arr = getStatesForResistance(value.resistance);
            WireStateValue v = WireStateValue();
            if (arr.size() >= 1) {
                for (int i = 0; i < arr.size(); i++) {
                    if (i == 0) {
                        v = arr[i];
                    } else {
                        void* c = &HandlerCheck(arr[i], &v);
                        v = *(WireStateValue*)c;
                        delete c;
                    }
                }
            }
            overridevalue = v;
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