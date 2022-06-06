#pragma once

#include "main_init.hpp"

namespace LogicSim {
    template <typename K, typename V>
    class TableConfigEntry : public ConfigEntry {
    public:
        TableConfigEntry() : ConfigEntry(Table<K,V>(), "table") {};
        virtual const char* getObjectType() override {
            return "TableConfigEntry";
        };
        virtual string toString() override {
            return this->get_t<Table<K,V>>().toString();
        };
        V& operator[](K key) {
            return this->get_t<Table<K,V>>()[key];
        };
        auto begin() {
            return this->get_t<Table<K,V>>().begin();
        };
        auto end() {
            return this->get_t<Table<K,V>>().end();
        };
        auto size() {
            return this->get_t<Table<K,V>>().size();
        };
        auto find(K key) {
            return this->get_t<Table<K,V>>().find(key);
        };
        auto erase(K key) {
            return this->get_t<Table<K,V>>().erase(key);
        };
        auto clear() {
            this->get_t<Table<K,V>>().clear();
        };
        bool exists(K key) {
            return this->get_t<Table<K,V>>().exists(key);
        };
    };
};