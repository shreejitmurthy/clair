//
// Created by Shreejit Murthy on 6/2/2026
//

#include "clair.hh"

#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>

#define TAB "    "

inline bool check_prefix(const std::string& s, const std::string& p) {
    return s.length() >= p.length() && s.substr(0, p.length()) == p;
}

clair::parser::parser(const std::string& name) : _name(name) {
    OptionDef n = {"help", __DEFAULT_SHORT_OPT, "Show help and exit."};

    options.emplace(n, [this](std::vector<std::string> arg) {
        this->help(arg);
    });
}

void clair::parser::option(std::string name, Callback cb, ArgumentTypes argtype, int expect, std::string description, char name_short) {
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

void clair::parser::option(std::string name, Callback cb, int expect, std::string description, char name_short) {
    option(name, cb, ArgumentTypes::String, expect, description, name_short);
}

void clair::parser::parse(int argc, char **argv) {
    if (options.size() == 0) return;
    std::vector<std::string> raw_args(argv, argv + argc);

    if (_exec == "") _exec = argv[0];
    
    for (int i = 1; i < raw_args.size(); i++) {
        auto& s = raw_args[i];
        // We'll check if the next arg is an option.
        auto& ns = raw_args[i + 1];
        
        if (check_prefix(s, "--") && !check_prefix(ns, "--")) {
            if (!exec_long(raw_args, i, s)) {
                auto err = std::format("Unknown option '{}'!\n", s);
                if (_fatal) throw std::runtime_error(err);
                else std::cout << err;
            }
        } else if (check_prefix(s, "-") && !check_prefix(ns, "-")) {
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

bool clair::parser::exec_long(std::vector<std::string> args, int i, const std::string &s) {
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

bool clair::parser::exec_short(std::vector<std::string> args, int i, const std::string &s) {
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

void clair::parser::help(std::vector<std::string>) {
    std::string out = std::format("{} v{} {}\n\n", 
            _name, _version, 
            (_short_desc == "") ? _short_desc : std::format("- {}", _short_desc)
        );

        out += "USAGE\n";
        // out.append(std::format("    {} [GLOBAL_OPTIONS] <command> [COMMAND_OPTIONS] [ARGS...]\n", _exec));
        out.append(std::format(TAB "{} [OPTIONS] [ARGS...]\n", _exec));
        // out.append(std::format("    {} [OPTIONS] <subcommand> ...\n", _exec));
        out.append(std::format(TAB "{} --help\n", _exec));
        // out.append(std::format("    {} <command> --help\n", _exec));
        out.append(std::format(TAB "{} --version\n\n", _exec));

        out += "DESCRIPTION\n";
        out.append(std::format(TAB "{}\n\n", _description));

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

        if (_notes != "") out += "NOTES\n" TAB + _notes;

        std::cout << out << std::endl;
}
