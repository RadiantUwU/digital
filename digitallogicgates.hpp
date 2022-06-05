#pragma once

#include "main_init.hpp"

namespace DigitalLogic {
    using LogicSim::ConfigurableBasicGate;
    using LogicSim::Pin;
    using LogicSim::PinMark;
    using LogicSim::WireStateValue;
    
    class AndGate : public ConfigurableBasicGate {
    protected:
        virtual void update() override {
            WireStateValue a = this->pins[0].read();
            WireStateValue b = this->pins[1].read();
            
        };
    public:
        AndGate() : ConfigurableBasicGate(2,1) {};
        virtual const char* getObjectType() override {
            return "AndGate";
        };
    };
};