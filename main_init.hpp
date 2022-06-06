#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <exception>

#include "Awaitable.hpp"
#include "to_string.hpp"
#include "Thread.hpp"
#include "Events.hpp"

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
    enum class WireStateValueType {
        NONE,
        BIT,
        BYTE,
        WORD,
        DWORD,
        QWORD,
        FLOAT,
        OBJ
    };
    string wirestatetype_to_str(WireStateValueType t) {
        switch (t) {
        case WireStateValueType::NONE:
            return "NONE";
        case WireStateValueType::BIT:
            return "BIT";
        case WireStateValueType::BYTE:
            return "BYTE";
        case WireStateValueType::WORD:
            return "WORD";
        case WireStateValueType::DWORD:
            return "DWORD";
        case WireStateValueType::QWORD:
            return "QWORD";
        case WireStateValueType::FLOAT:
            return "FLOAT";
        case WireStateValueType::OBJ:
            return "OBJECT";
        }
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
        class UnexpectedWireValueTypeError : public LogicSimException {
        public:
            UnexpectedWireValueTypeError(LogicSimObject* obj, WireStateValueType expected, WireStateValueType actual) :
                LogicSimException("Invalid WireValueType: " + wirestatetype_to_str(expected)
                + " expected, but got " + wirestatetype_to_str(actual) + " at "
                + obj->getObjectType() + ":" + to_string(obj)) {};    
        };
        
    };

    inline void memcpy(void* src, void* pst, size_t n) {
        char* sp = (char*)src;
        char* pp = (char*)pst;
        for (size_t i = 0; i < n; i++) 
            pp[i] = sp[i];
        
    }

    class ConfigEntry : public LogicSimObject {
        unique_ptr<void> ptr;
        const char* type = "<null>";
    public:
        virtual string toString() {
            return "<unknown>";
        }
        template<typename T>
        ConfigEntry(T obj, string type) {
            T* new_obj = malloc(sizeof(T));
            memcpy(&obj,new_obj,sizeof(T));
            ptr.reset(new_obj);
        }
        template<typename T>
        T& get_t() {
            return *(T*)ptr.get_t();
        }
        template<typename T>
        void set_t(T obj) {
            T* new_obj = malloc(sizeof(T));
            memcpy(new_obj,&obj,sizeof(T));
            ptr.reset(new_obj);
        }
        void set_t<nullptr_t>(nullptr_t) {
            ptr.reset(nullptr);
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
        unordered_map<K, unique_ptr<V>> entries;
    public:
        virtual const char* getObjectType() {
            return "Table";
        }
        Table() = default;
        unique_ptr<V>& operator[](const K& key) {
            auto v = entries.find(key);
            if (v == entries.end()) {
                throw Exceptions::InvalidKeyError(this, key);
            }
            return v;
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
        void insert(const K& key, unique_ptr<V> value) {
            entries[key] = std::move(value);
        }
        void erase(const K& key) {
            entries.erase(key);
        }
        void clear() {
            entries.clear();
        }
        auto find(const K& key) {
            return entries.find(key);
        }
        size_t size() {
            return entries.size();
        }
        string toString() {
            string ret = "{";
            for (auto& v : entries) {
                ret += to_string(v.first) + ":" + to_string(v.second) + ",";
            }
            ret += "}";
            return ret;
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
    class SubCircuit;   // to define
    class Pin;        
    class MainSim;     // to define
    
    class WireStateValue final : public LogicSimObject {
    public:
        virtual const char* getObjectType() {
            return "WireStateValue";
        }
        unsigned short resistance = 0;
        union {
            void* ptr;
            long double fp;
            unsigned long long ll = 0;
            unsigned long l;
            unsigned short s;
            unsigned char byte;
            bool b;
        };
        WireStateValueType type = WireStateValueType::NONE;
        WireStateValue() : resistance(-1) {};
        WireStateValue(std::nullptr_t) : resistance(-1) {};
        WireStateValue(unsigned char byte) : byte(byte), type(WireStateValueType::BYTE) {};
        WireStateValue(unsigned short v) : s(v), type(WireStateValueType::WORD) {};
        WireStateValue(unsigned long v) : l(v), type(WireStateValueType::DWORD) {};
        WireStateValue(unsigned long long v) : ll(v), type(WireStateValueType::QWORD) {};
        WireStateValue(bool v) : b(v), type(WireStateValueType::BIT) {};
        WireStateValue(void* v) : ptr(v), type(WireStateValueType::OBJ) {};
        WireStateValue(long double v) : fp(v), type(WireStateValueType::FLOAT) {};
        WireStateValue(const WireStateValue& other) {
            type = other.type;
            ll = other.ll; //capture all bytes
        }
        ~WireStateValue() {
            if (type == WireStateValueType::OBJ) {
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
        string s = "WireStateValue(" + wirestatetype_to_str(v->type) + ":";
        switch (v->type) {
            case WireStateValueType::NONE:
            case WireStateValueType::BYTE:
                s += to_string(v->byte);
                break;
            case WireStateValueType::WORD:
                s += to_string(v->s);
                break;
            case WireStateValueType::DWORD:
                s += to_string(v->l);
                break;
            case WireStateValueType::QWORD:
                s += to_string(v->ll);
                break;
            case WireStateValueType::FLOAT:
                s += to_string(v->fp);
                break;
            case WireStateValueType::OBJ:
                s += to_string(v->ptr);
                break;
            case WireStateValueType::BIT:
                s += to_string((int)v->b);
                break;
        }
        s += ")";
        return s;
    }
    namespace Exceptions {
        class ShortCircuitError : public LogicSimException {
        public:
            ShortCircuitError(WireStateValue* a,WireStateValue* b, LogicSimObject* obj) : LogicSimException(
                "short circuit conflict between two wire state values " + to_string_v(a) 
                + "," + to_string_v(b) + " at object " + obj->getObjectType() + ":" + to_string(obj)
                ) {};
        };
    }

    class WireState final : public LogicSimObject {
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
    enum class PinMark {
        INPUT,
        OUTPUT,
        BIDIRECTIONAL
    };
    class Pin final : public LogicSimObject {
        unsigned short pin_num;
        string name;
        Wire* wire = nullptr;
        WireStateValue state = WireStateValue();
        PinMark mark = PinMark::BIDIRECTIONAL;
        BasicGate* root = nullptr;
    public:
        virtual const char* getObjectType() {
            return "Pin";
        }
        Pin(BasicGate* gate, unsigned short pin_num, string name) 
            : root(gate), pin_num(pin_num), name(name) {};
        Pin(BasicGate* gate, unsigned short pin_num) 
            : root(gate), pin_num(pin_num), name(to_string(pin_num)) {};
        Pin(BasicGate* gate, unsigned short pin_num, PinMark mark) 
            : root(gate), pin_num(pin_num), mark(mark), name(to_string(pin_num)) {};
        Pin(BasicGate* gate, unsigned short pin_num, string name, PinMark mark) 
            : root(gate), pin_num(pin_num), name(name), mark(mark) {};
        ~Pin();
        unsigned short getPinNum() {
            return pin_num;
        }
        string getName() {
            return name;
        }
        PinMark getMark() {
            return mark;
        }
        void setMark(PinMark mark) {
            this->mark = mark;
        }
        bool operator == (const Pin& other) {
            return this == &other;
        }
        bool operator != (const Pin& other) {
            return this != &other;
        }
        bool operator == (unsigned short other) {
            return this->pin_num == other;
        }
        bool operator != (unsigned short other) {
            return this->pin_num != other;
        }
        BasicGate* getRoot() {
            return root;
        }
        void setWire(Wire* wire);
        void write(int in,WireStateValue value);
        vector<WireState> read();
        
    };
    
    class BasicGate : public LogicSimObject {
    private:
        bool marked_forUpdate = false;
        void await_for_update();
        friend class Wire;
        void markForUpdate() {
            if (!marked_forUpdate) {
                marked_forUpdate = true;
                await_for_update();
            }
        }
    protected:
        vector<Pin> pins;
        virtual void update() = 0;
        friend class MainSim;
    public:
        virtual bool isConfigurable() {return false;}
        virtual const char* getObjectType() {
            return "BasicGate";
        }
        BasicGate() = default;
        BasicGate(unsigned short num_pins) {
            for (int i = 0; i < num_pins; i++) {
                pins.push_back(Pin(this,i + 1));
            }
        }
        BasicGate(unsigned short input_pins, unsigned short output_pins) {
            for (int i = 0; i < input_pins; i++) {
                pins.push_back(Pin(this, i + 1, PinMark::INPUT));
            }
            for (int i = 0; i < output_pins; i++) {
                pins.push_back(Pin(this, i + output_pins + 1, PinMark::OUTPUT));
            }
        }
        BasicGate(unsigned short input_pins, unsigned short output_pins, unsigned short bidirectional_pins) {
            for (int i = 0; i < input_pins; i++) {
                pins.push_back(Pin(this, i + 1, PinMark::INPUT));
            }
            for (int i = 0; i < output_pins; i++) {
                pins.push_back(Pin(this, i + output_pins + 1, PinMark::OUTPUT));
            }
            for (int i = 0; i < bidirectional_pins; i++) {
                pins.push_back(Pin(this, i + output_pins + input_pins + 1, PinMark::BIDIRECTIONAL));
            }
        }

    };
    class ConfigurableBasicGate : public BasicGate {
    public:
        const bool is_configurable = true;
        ConfigTable config_table;
        virtual const char* getObjectType() {
            return "ConfigurableBasicGate";
        }
        virtual bool isConfigurable() {
            return true;
        }
        ConfigurableBasicGate() = default;
        ConfigurableBasicGate(unsigned short num_pins) : BasicGate(num_pins) {};
        ConfigurableBasicGate(unsigned short input_pins, unsigned short output_pins) : BasicGate(input_pins, output_pins) {};
        ConfigurableBasicGate(unsigned short input_pins, unsigned short output_pins, unsigned short bidirectional_pins) : BasicGate(input_pins, output_pins, bidirectional_pins) {};
    };
    
    class Wire final : public LogicSimObject {
        vector<WireState> state;
        friend class Pin;
        friend void connect(Pin*,Wire*);
        friend void disconnect(Pin*,Wire*);
        vector<Pin*> pins;
    public:
        virtual const char* getObjectType() {
            return "Wire";
        }
        Wire() = default;
        ~Wire() {
            for (auto pin : pins) {
                pin->setWire(nullptr);
            }
        };
        WireStateValue getState(unsigned short index) {
            if (state.size() > 0) return state[index % state.size()].getState();
            return WireStateValue();
        }
        vector<WireState> getState() {
            return state;
        }

        void mark_for_update() {
            for (auto pin : pins) {
                if (pin->getMark() != PinMark::OUTPUT) pin->getRoot()->markForUpdate();
            }
        }
    };
    void connect(Pin* p,Wire* w) {
        p->setWire(w);
        w->pins.push_back(p);
    };
    void disconnect(Pin* p, Wire* w) {
        p->setWire(nullptr);
        w->pins.erase(remove(w->pins.begin(), w->pins.end(), p), w->pins.end());
    };
};