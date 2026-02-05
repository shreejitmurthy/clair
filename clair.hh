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

#define __DEFAULT_SHORT_FLAG "__undef_short"

class clair {
public:
    clair() {}
    void version(const std::string& v) { this->v = v; }
    // the value of the flag as string
    using Callback = std::function<void(std::string)>;
    void flag(
        std::string name, 
        Callback cb,
        std::string description = "", 
        std::string name_short = __DEFAULT_SHORT_FLAG
    ) {
        FlagDef n = {name, name_short};
        for (auto& f : flags) {
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
    std::string v;

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
};