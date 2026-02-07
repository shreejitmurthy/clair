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

#define __DEFAULT_SHORT_OPT '!'

#define __PHI 0x9e3779b97f4a7c15ULL

namespace clair {

enum class ArgumentTypes {
    String = 0,  // default

    // ato/i/f
    Int,
    Double,
    // str/ol/oll/oul/oull
    Long,
    LongLong,
    ULong,
    ULongLong,

    Bool
};
    
class parser {
public:
    parser(const std::string& name);
    void exec(const std::string& exec) { _exec = exec; }
    void version(const std::string& version) { _version = version; }
    void fatal(bool f) { _fatal = f; }
    void description(const std::string& description) { _description = description; };
    void short_description(const std::string& short_description) { _short_desc = short_description; }
    void enable_short_help(char short_form) { 
        for (auto f : options) {
            if (f.first.long_name == "help" && f.first.short_name == __DEFAULT_SHORT_OPT) {
                // TODO: set to 'h'
            }
        }
    }
    void notes(const std::string& n) { _notes = n; }
    // the value/s of the option as string vector
    using Callback = std::function<void(std::vector<std::string>)>;
    void option(
        std::string name, 
        Callback cb,
        ArgumentTypes argtype,
        int expect = 1,
        std::string description = "",
        char name_short = __DEFAULT_SHORT_OPT
    );
    void option(
        std::string name, 
        Callback cb,
        int expect = 1,
        std::string description = "",
        char name_short = __DEFAULT_SHORT_OPT
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

    typedef struct OptionDef {
        std::string long_name;
        char short_name;
        std::string desc;
        int expected_args;
        ArgumentTypes expected_type;

        // Options are differentiated by their long and short name.
        bool operator==(OptionDef const& o) const {
            return long_name == o.long_name && short_name == o.short_name;
        }
    } OptionDef;

    typedef struct OptionDefHash {
        std::size_t operator()(OptionDef const& k) const noexcept {
            std::size_t h1 = std::hash<std::string>{}(k.long_name);
            std::size_t h2 = std::hash<char>{}(k.short_name);
            // Fibonacci hashing
            return h1 ^ (h2 + __PHI + (h1<<6) + (h1>>2));
        }
    } OptionDefHash;

    std::unordered_map<OptionDef, Callback, OptionDefHash> options;

    bool exec_long(std::vector<std::string> args, int i, const std::string& s) {
        for (auto& f : options) {
            std::vector<std::string> ns;
            int n = f.first.expected_args;
            if (i + 1 < args.size()) {
                const auto start = i + 1;
                const auto end = std::min((int)args.size(), start + n);
                ns.insert(ns.end(), args.begin() + start, args.begin() + end);
            }
            if (f.first.long_name == s.substr(2, s.length())) {
                f.second(ns);
                return true;
            }
        }
        return false;
    }

    bool exec_short(std::vector<std::string> args, int i, const std::string& s) {
        for (auto& o : options) {
            std::vector<std::string> ns;
            int n = o.first.expected_args;
            if (i + 1 < args.size()) {
                const auto start = i + 1;
                const auto end = std::min((int)args.size(), start + n);
                ns.insert(ns.end(), args.begin() + start, args.begin() + end);
            }
            if (o.first.short_name == s[1]) {
                o.second(ns);
                return true;
            }
        }
        return false;
    }

    void help(std::vector<std::string>) {
        std::string out = std::format("{} v{} {}\n\n", 
            _name, _version, 
            (_short_desc == "") ? _short_desc : std::format("- {}", _short_desc)
        );

        out += "USAGE\n";
        // out.append(std::format("    {} [GLOBAL_OPTIONS] <command> [COMMAND_OPTIONS] [ARGS...]\n", _exec));
        out.append(std::format("    {} [OPTIONS] [ARGS...]\n", _exec));
        // out.append(std::format("    {} [OPTIONS] <subcommand> ...\n", _exec));
        out.append(std::format("    {} --help\n", _exec));
        // out.append(std::format("    {} <command> --help\n", _exec));
        out.append(std::format("    {} --version\n\n", _exec));

        out += "DESCRIPTION\n";
        out.append(std::format("    {}\n\n", _description));

        // TODO: Commands support
        // out += "COMMANDS\n";

        auto make_names = [&](const auto& f) {
            bool has_short = f.first.short_name != __DEFAULT_SHORT_OPT;
            return has_short
              ? (std::string("-") + std::string(1, f.first.short_name) + ", --" + f.first.long_name)
              : (std::string("--") + f.first.long_name);   
        };
        std::size_t maxlen = 0;
        for (auto& f : options) maxlen = std::max(maxlen, make_names(f).size());
        std::ostringstream oss;
        // Give option to sort this list in future.
        oss << "OPTIONS\n";
        for (auto& f : options) {
            std::string names = make_names(f);
            oss << "    "
                << std::left << std::setw(static_cast<int>(maxlen + 2)) << names
                << f.first.desc << '\n';
        }
        out += oss.str() + "\n";

        if (_notes != "") out += "NOTES\n    " + _notes;

        std::cout << out << std::endl;
    }
};

/* TODO:
 * - Read args as int and float instead of having to manually convert from string
 * - Features:
 *  - Common options
 *  - Arguments 
 *  - Examples 
 *  - Exit status
 *  - Configuration
 *  - See also
 */

};