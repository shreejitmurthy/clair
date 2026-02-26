//
// Created by Shreejit Murthy on 6/2/2026
//

#pragma once

#include <stdexcept>
#include <type_traits>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include <iostream>
#include <format>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#define __DEFAULT_SHORT_OPT '!'

#define __PHI 0x9e3779b97f4a7c15ULL

#define __TAB "    "

inline bool __check_prefix(const std::string& s, const std::string& p) {
    return s.length() >= p.length() && s.substr(0, p.length()) == p;
}

namespace clair {

class logger {
public:
    // template<typename... Args>
    // static void log(std::string s, Args&&... args) {
    //     auto i = std::format(s, args...);
    //     std::cout << i << std::endl;
    // }
    
    // template<typename... Args>
    // static void runtime_error(const std::string& s, Args&&... args) {
    //     throw std::runtime_error(std::format(s, args...));
    // }
};



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
    parser(const std::string& name) : _name(name) {
        OptionDef n = {"help", __DEFAULT_SHORT_OPT, "Show help and exit."};

        // logger::log("Hello, {}", "World!");
    
        options.emplace(n, [this](std::vector<std::string> arg) {
            this->help(arg);
        });
        
    }
    void exec(const std::string& exec) { _exec = exec; }
    void version(const std::string& version) { _version = version; }
    void fatal(bool f) { _fatal = f; }
    void description(const std::string& description) { _description = description; };
    void short_description(const std::string& short_description) { _short_desc = short_description; }
    void enable_short_help(char short_form) { 
        for (auto& o : options) {
            if (o.first.long_name == "help" && o.first.short_name == __DEFAULT_SHORT_OPT) {
                // TODO: set to 'h'
                // o.first.short_name = short_form;
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
    ) {
        OptionDef n = {name, name_short, description, expect, argtype};
        for (auto& f : options) {
            /* 
             * The application will behave strange or break if options have the same name, 
             * so these must be runtime errors, fatal enabled or not. 
             */
            if (f.first.long_name == name) {
                throw std::runtime_error(std::format("Already defined {} as long name", name));
            } else if (f.first.short_name == name_short && name_short != __DEFAULT_SHORT_OPT) {
                throw std::runtime_error(std::format("Already defined {} as short name", name_short));
            }
        }

        options[n] = cb;
    }
             
    void option(
        std::string name, 
        Callback cb,
        int expect = 1,
        std::string description = "",
        char name_short = __DEFAULT_SHORT_OPT
    ) {
        option(name, cb, ArgumentTypes::String, expect, description, name_short);
    }
    
    void parse(int argc, char** argv) {
        if (options.size() == 0) return;
        std::vector<std::string> raw_args(argv, argv + argc);

        if (_exec == "") _exec = argv[0];
    
        for (int i = 1; i < raw_args.size(); i++) {
            auto& s = raw_args[i];
            // We'll check if the next arg is an option.
            auto& ns = raw_args[i + 1];
        
            if (__check_prefix(s, "--") && !__check_prefix(ns, "--")) {
                if (!exec_long(raw_args, i, s)) {
                    auto err = std::format("Unknown option '{}'!\n", s);
                    if (_fatal) throw std::runtime_error(err);
                    else std::cout << err;
                }
            } else if (__check_prefix(s, "-") && !__check_prefix(ns, "-")) {
                if (!exec_short(raw_args, i, s)) {
                    auto err = std::format("Unknown option '{}'!\n", s);
                    if (_fatal) throw std::runtime_error(err);
                    else std::cout << err;
                }
            } else {
                // TODO: Error handle this.
            }
        }
    }
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
            return h1 ^ (h2 + __PHI + (h1 << 6) + (h1 >> 2));
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
        out.append(std::format(__TAB "{} [OPTIONS] [ARGS...]\n", _exec));
        // out.append(std::format("    {} [OPTIONS] <subcommand> ...\n", _exec));
        out.append(std::format(__TAB "{} --help\n", _exec));
        // out.append(std::format("    {} <command> --help\n", _exec));
        out.append(std::format(__TAB "{} --version\n\n", _exec));

        out += "DESCRIPTION\n";
        out.append(std::format(__TAB "{}\n\n", _description));

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

        if (_notes != "") out += "NOTES\n" __TAB + _notes;

        std::cout << out << std::endl;
    }
};

/* TODO:
 * - Read args as int and float instead of having to manually convert from string
 * - Features:
 *  - Help info for each option/anything with a description.
 *  - Common options
 *  - Arguments 
 *  - Examples 
 *  - Exit status
 *  - Configuration
 *  - See also
 */

};
