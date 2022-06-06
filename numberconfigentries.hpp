#pragma once

#include "main_init.hpp"

namespace LogicSim {
    class UShortConfigEntry : public ConfigEntry {
    public:
        UShortConfigEntry(unsigned short value) : ConfigEntry(value,"ushort") {};
        virtual string toString() override {
            return std::to_string(this->get<unsigned short>());
        };
    };
    class UByteConfigEntry : public ConfigEntry {
    public:
        UByteConfigEntry(unsigned char value) : ConfigEntry(value,"ubyte") {};
        virtual string toString() override {
            return std::to_string(this->get<unsigned char>());
        };
    };
    class ULongConfigEntry : public ConfigEntry {
    public:
        ULongConfigEntry(unsigned long value) : ConfigEntry(value,"ulong") {};
        virtual string toString() override {
            return std::to_string(this->get<unsigned long>());
        };
    };
    class ULongLongConfigEntry : public ConfigEntry {
    public:
        ULongLongConfigEntry(unsigned long long value) : ConfigEntry(value,"ulonglong") {};
        virtual string toString() override {
            return std::to_string(this->get<unsigned long long>());
        };
    };
    class ShortConfigEntry : public ConfigEntry {
    public:
        ShortConfigEntry(short value) : ConfigEntry(value,"short") {};
        virtual string toString() override {
            return std::to_string(this->get<short>());
        };
    };
    class ByteConfigEntry : public ConfigEntry {
    public:
        ByteConfigEntry(char value) : ConfigEntry(value,"byte") {};
        virtual string toString() override {
            return std::to_string(this->get<char>());
        };
    };
    class LongConfigEntry : public ConfigEntry {
    public:
        LongConfigEntry(long value) : ConfigEntry(value,"long") {};
        virtual string toString() override {
            return std::to_string(this->get<long>());
        };
    };
    class LongLongConfigEntry : public ConfigEntry {
    public:
        LongLongConfigEntry(long long value) : ConfigEntry(value,"longlong") {};
        virtual string toString() override {
            return std::to_string(this->get<long long>());
        };
    };

}