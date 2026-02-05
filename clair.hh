//
// Created by Shreejit Murthy on 6/2/2026
//

#pragma once

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <type_traits>
#include <functional>
#include <stdexcept>
#include <format>
#include <iostream>

#define __DEFAULT_SHORT_FLAG "__undef_short"

class clair {
public:
    clair(const std::string& name) { _name = name; }
    void exec(const std::string& exec) { _exec = exec; }
    void version(const std::string& version) { _version = version; }
    void fatal(bool f) { _fatal = f; }
    void description(const std::string& description) { _description = description; };
    void short_description(const std::string& short_description) { _short_desc = short_description; }
    // the value of the flag as string
    using Callback = std::function<void(std::string)>;
    void flag(
        std::string name, 
        Callback cb,
        std::string name_short = __DEFAULT_SHORT_FLAG,
        std::string description = ""
    ) {
        FlagDef n = {name, name_short};
        for (auto& f : flags) {
            /* 
             * The application will behave strange or break if flags have the same name, so these must be 
             * runtime errors, fatal enabled or not. 
             */
            if (f.first.long_name == name) {
                throw std::runtime_error(std::format("Already defined {} as long name", name));
            } else if (f.first.short_name == name_short && name_short != __DEFAULT_SHORT_FLAG) {
                throw std::runtime_error(std::format("Already defined {} as short name", name_short));
            }
        }

        flags[n] = cb;
    }
    void parse(int argc, char** argv);
private:
    std::string _name;
    std::string _exec;
    std::string _description;
    std::string _short_desc;
    std::string _version;
    bool _fatal = false;

    typedef struct FlagDef {
        std::string long_name;
        std::string short_name;

        bool operator==(FlagDef const& o) const {
            return long_name == o.long_name && short_name == o.short_name;
        }
    } FlagDef;

    typedef struct FlagDefHash {
        std::size_t operator()(FlagDef const& k) const noexcept {
            std::size_t h1 = std::hash<std::string>{}(k.long_name);
            std::size_t h2 = std::hash<std::string>{}(k.short_name);
            return h1 ^ (h2 + 0x9e3779b97f4a7c15ULL + (h1<<6) + (h1>>2));
        }
    } FlagDefHash;

    // vector stores long and sho {rt names.
    std::unordered_map<FlagDef, Callback, FlagDefHash> flags;

    void exec_long(const std::string& s, const std::string& ns) {
        for (auto& f : flags) {
            if (f.first.long_name == s.substr(2, s.length())) {
                f.second(ns);
            } else {
                auto err = std::format("Unknown flag '{}'!\n", s);
                if (_fatal) throw std::runtime_error(err);
                else std::cout << err;
            }
        }
        
    }
    void exec_short(const std::string& s, const std::string& ns) {
        for (auto& f : flags) {
            if (f.first.short_name == s.substr(1, s.length())) {
                f.second(ns);
            } else {
                auto err = std::format("Unknown flag '{}'!\n", s);
                if (_fatal) throw std::runtime_error(err);
                else std::cout << err;
            }
        }
    }

    // TODO: automatic `--help`
};