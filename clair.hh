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
#include <iomanip>
#include <sstream>

#define __DEFAULT_SHORT_FLAG "__undef_short"

#define __PHI 0x9e3779b97f4a7c15ULL

class clair {
public:
    clair(const std::string& name);
    void exec(const std::string& exec) { _exec = exec; }
    void version(const std::string& version) { _version = version; }
    void fatal(bool f) { _fatal = f; }
    void description(const std::string& description) { _description = description; };
    void short_description(const std::string& short_description) { _short_desc = short_description; }
    void enable_short_help(char short_form) { 
        for (auto& f : flags) {
            if (f.first.long_name == "help" && f.first.short_name == __DEFAULT_SHORT_FLAG) {
                // FIXME
                // f.first.short_name = short_form;
            }
        }
    }
    void notes(const std::string& n) { _notes = n; }
    // the value of the flag as string
    using Callback = std::function<void(std::string)>;
    void flag(
        std::string name, 
        Callback cb,
        std::string name_short = __DEFAULT_SHORT_FLAG,
        std::string description = ""
    );
    void parse(int argc, char** argv);
private:
    std::string _name;
    std::string _exec;
    std::string _description;
    std::string _short_desc;
    std::string _version;
    std::string _notes;
    bool _fatal = false;
    bool _help_short_form = false;

    typedef struct FlagDef {
        std::string long_name;
        std::string short_name;
        std::string desc;

        bool operator==(FlagDef const& o) const {
            return long_name == o.long_name && short_name == o.short_name;
        }
    } FlagDef;

    typedef struct FlagDefHash {
        std::size_t operator()(FlagDef const& k) const noexcept {
            std::size_t h1 = std::hash<std::string>{}(k.long_name);
            std::size_t h2 = std::hash<std::string>{}(k.short_name);
            // Fibonacci hashing
            return h1 ^ (h2 + __PHI + (h1<<6) + (h1>>2));
        }
    } FlagDefHash;

    std::unordered_map<FlagDef, Callback, FlagDefHash> flags;

    bool exec_long(const std::string& s, const std::string& ns) {
        for (auto& f : flags) {
            if (f.first.long_name == s.substr(2, s.length())) {
                f.second(ns);
                return true;
            }
        }
        return false;
    }

    bool exec_short(const std::string& s, const std::string& ns) {
        for (auto& f : flags) {
            if (f.first.short_name == s.substr(1, s.length())) {
                f.second(ns);
                return true;
            }
        }
        return false;
    }

    void help(std::string) {
        std::string out = std::format("{} v{} {}\n\n", 
            _name, _version, 
            (_short_desc == "") ? _short_desc : std::format("- {}", _short_desc)
        );

        out += "USAGE\n";
        // out.append(std::format("    {} [GLOBAL_OPTIONS] <command> [COMMAND_OPTIONS] [ARGS...]\n", _exec));
        out.append(std::format("    {} [GLOBAL_OPTIONS] [ARGS...]\n", _exec));
        // out.append(std::format("    {} [GLOBAL_OPTIONS] <subcommand> ...\n", _exec));
        out.append(std::format("    {} --help\n", _exec));
        // out.append(std::format("    {} <command> --help\n", _exec));
        out.append(std::format("    {} --version\n\n", _exec));

        out += "DESCRIPTION\n";
        out.append(std::format("    {}\n\n", _description));

        // TODO: Commands support
        // out += "COMMANDS\n";

        auto make_names = [&](const auto& f) {
            bool has_short = f.first.short_name != __DEFAULT_SHORT_FLAG;
            return has_short
                ? ("-" + f.first.short_name + ", --" + f.first.long_name)
                : ("--" + f.first.long_name);
        };
        std::size_t maxlen = 0;
        for (auto& f : flags) maxlen = std::max(maxlen, make_names(f).size());
        std::ostringstream oss;
        oss << "GLOBAL OPTIONS\n";
        for (auto& f : flags) {
            std::string names = make_names(f);
            oss << "    "
                << std::left << std::setw(static_cast<int>(maxlen + 2)) << names
                << f.first.desc << '\n';
        }
        out += oss.str();

        if (_notes != "") out += "NOTES\n    " + _notes + "\n";

        std::cout << out << std::endl;
    }
};

// TODO: Common options, arguments, examples, exit status, configuration, see also