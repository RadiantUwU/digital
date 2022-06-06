#pragma once

#include "main_init.hpp"

namespace LogicSim {
    class StringConfigEntry : public ConfigEntry {
    public:
        StringConfigEntry(string value) : ConfigEntry(value,"string") {
            this->set_t<string>("");
        };
        virtual const char* getObjectType() override {
            return "StringConfigEntry";
        };
        virtual string toString() override {
            return this->get_t<string>();
        };
        string get() {
            return this->get_t<string>();
        };
        void set(string value) {
            this->set_t<string>(value);
        };
    };
};