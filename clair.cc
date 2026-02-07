//
// Created by Shreejit Murthy on 6/2/2026
//

#include "clair.hh"

inline bool check_prefix(const std::string& s, const std::string& p) {
    return s.length() >= p.length() && s.substr(0, p.length()) == p;
}

clair::parser::parser(const std::string& name) : _name(name) {
    FlagDef n = {"help", __DEFAULT_SHORT_FLAG, "Show help and exit."};

    flags.emplace(n, [this](std::vector<std::string> arg) {
        this->help(arg);
    });
}

void clair::parser::flag(std::string name, Callback cb, ArgumentTypes argtype, int expect, std::string description, char name_short) {
    FlagDef n = {name, name_short, description, expect, argtype};
    for (auto& f : flags) {
        /* 
         * The application will behave strange or break if options have the same name, 
         * so these must be runtime errors, fatal enabled or not. 
         */
        if (f.first.long_name == name) {
            throw std::runtime_error(std::format("Already defined {} as long name", name));
        } else if (f.first.short_name == name_short && name_short != __DEFAULT_SHORT_FLAG) {
            throw std::runtime_error(std::format("Already defined {} as short name", name_short));
        }
    }

    flags[n] = cb;
}

void clair::parser::flag(std::string name, Callback cb, int expect, std::string description, char name_short) {
    flag(name, cb, ArgumentTypes::String, expect, description, name_short);
}

void clair::parser::parse(int argc, char **argv) {
    std::vector<std::string> raw_args(argv, argv + argc);

    if (_exec == "") _exec = argv[0];
    
    for (int i = 1; i < raw_args.size(); i++) {
        auto& s = raw_args[i];
        // We'll check if the next arg is an option.
        auto& ns = raw_args[i + 1];
        
        if (check_prefix(s, "--") && !check_prefix(ns, "--")) {
            if (!exec_long(raw_args, i, s)) {
                auto err = std::format("Unknown flag '{}'!\n", s);
                if (_fatal) throw std::runtime_error(err);
                else std::cout << err;
            }
        } else if (check_prefix(s, "-") && !check_prefix(ns, "-")) {
            if (!exec_short(raw_args, i, s)) {
                auto err = std::format("Unknown flag '{}'!\n", s);
                if (_fatal) throw std::runtime_error(err);
                else std::cout << err;
            }
        } else {
            // TODO: Error handle this.
        }
    }
}
