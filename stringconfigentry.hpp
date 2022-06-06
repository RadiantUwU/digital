#pragma once

#include "main_init.hpp"

namespace LogicSim {
    class StringConfigEntry : public ConfigEntry {
    public:
        StringConfigEntry(string value) : ConfigEntry(value,"string") {};
        virtual string toString() override {
            return this->get_t<string>();
        };
        string get() {
            return this->get_t<string>();
        };
    };
};