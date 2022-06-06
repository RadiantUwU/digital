#pragma once

#include "main_init.hpp"

namespace LogicSim {
    class UShortConfigEntry : public ConfigEntry {
    public:
        UShortConfigEntry(unsigned short value) : ConfigEntry(value,"ushort") {};
        virtual string toString() override {
            return std::to_string(this->get_t<unsigned short>());
        };
        unsigned short get() {
            return this->get_t<unsigned short>();
        };
        void set(unsigned short value) {
            this->set_t<unsigned short>(value);
        };
        virtual const char* getObjectType() override {
            return "UShortConfigEntry";
        };
    };
    class UByteConfigEntry : public ConfigEntry {
    public:
        UByteConfigEntry(unsigned char value) : ConfigEntry(value,"ubyte") {};
        virtual string toString() override {
            return std::to_string(this->get_t<unsigned char>());
        };
        unsigned char get() {
            return this->get_t<unsigned char>();
        };
        void set(unsigned char value) {
            this->set_t<unsigned char>(value);
        };
        virtual const char* getObjectType() override {
            return "UByteConfigEntry";
        };
    };
    class ULongConfigEntry : public ConfigEntry {
    public:
        ULongConfigEntry(unsigned long value) : ConfigEntry(value,"ulong") {};
        virtual string toString() override {
            return std::to_string(this->get_t<unsigned long>());
        };
        unsigned long get() {
            return this->get_t<unsigned long>();
        };
        void set(unsigned long value) {
            this->set_t<unsigned long>(value);
        };
        virtual const char* getObjectType() override {
            return "ULongConfigEntry";
        };
    };
    class ULongLongConfigEntry : public ConfigEntry {
    public:
        ULongLongConfigEntry(unsigned long long value) : ConfigEntry(value,"ulonglong") {};
        virtual string toString() override {
            return std::to_string(this->get_t<unsigned long long>());
        };
        unsigned long long get() {
            return this->get_t<unsigned long long>();
        };
        void set(unsigned long long value) {
            this->set_t<unsigned long long>(value);
        };
        virtual const char* getObjectType() override {
            return "ULongLongConfigEntry";
        };
    };
    class ShortConfigEntry : public ConfigEntry {
    public:
        ShortConfigEntry(signed short value) : ConfigEntry(value,"short") {};
        virtual string toString() override {
            return std::to_string(this->get_t<signed short>());
        };
        signed short get() {
            return this->get_t<short>();
        };
        void set(signed short value) {
            this->set_t<short>(value);
        };
        virtual const char* getObjectType() override {
            return "ShortConfigEntry";
        };
    };
    class ByteConfigEntry : public ConfigEntry {
    public:
        ByteConfigEntry(signed char value) : ConfigEntry(value,"byte") {};
        virtual string toString() override {
            return std::to_string(this->get_t<signed char>());
        };
        signed char get() {
            return this->get_t<signed char>();
        };
        void set(signed char value) {
            this->set_t<signed char>(value);
        };
        virtual const char* getObjectType() override {
            return "ByteConfigEntry";
        };
    };
    class LongConfigEntry : public ConfigEntry {
    public:
        LongConfigEntry(signed long value) : ConfigEntry(value,"long") {};
        virtual string toString() override {
            return std::to_string(this->get_t<signed long>());
        };
        signed long get() {
            return this->get_t<signed long>();
        };
        void set(signed long value) {
            this->set_t<signed long>(value);
        };
        virtual const char* getObjectType() override {
            return "LongConfigEntry";
        };
    };
    class LongLongConfigEntry : public ConfigEntry {
    public:
        LongLongConfigEntry(signed long long value) : ConfigEntry(value,"longlong") {};
        virtual string toString() override {
            return std::to_string(this->get_t<signed long long>());
        };
        signed long long get() {
            return this->get_t<signed long long>();
        };
        void set(signed long long value) {
            this->set_t<signed long long>(value);
        };
        virtual const char* getObjectType() override {
            return "LongLongConfigEntry";
        };
    };
    class FloatConfigEntry : public ConfigEntry {
    public:
        FloatConfigEntry(long double value) : ConfigEntry(value,"float") {};
        virtual string toString() override {
            return std::to_string(this->get_t<long double>());
        };
        long double get() {
            return this->get_t<long double>();
        };
        void set(long double value) {
            this->set_t<long double>(value);
        };
        virtual const char* getObjectType() override {
            return "FloatConfigEntry";
        };
    };  

}