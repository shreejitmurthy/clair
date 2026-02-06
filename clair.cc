//
// Created by Shreejit Murthy on 6/2/2026
//

#include "clair.hh"

inline bool check_prefix(const std::string& s, const std::string& p) {
    return s.length() >= p.length() && s.substr(0, p.length()) == p;
}

clair::clair(const std::string& name) : _name(name) {
    FlagDef n = {"help", __DEFAULT_SHORT_FLAG, "Show help and exit."};

    flags.emplace(n, [this](std::string arg) {
        this->help(std::move(arg));
    });
}


void clair::flag(std::string name, Callback cb, std::string name_short, std::string description) {
    FlagDef n = {name, name_short, description};
    for (auto& f : flags) {
        /* 
         * The application will behave strange or break if flags have the same name, 
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

void clair::parse(int argc, char **argv) {
    std::vector<std::string> raw_args(argv, argv + argc);
    
    for (int i = 1; i < raw_args.size(); i++) {
        auto& s = raw_args[i];
        auto& ns = raw_args[i + 1];
        if (check_prefix(s, "--") && !check_prefix(ns, "--")) {
            if (!exec_long(s, ns)) {
                auto err = std::format("Unknown flag '{}'!\n", s);
                if (_fatal) throw std::runtime_error(err);
                else std::cout << err;
            }
        } else if (check_prefix(s, "-") && !check_prefix(ns, "-")) {
            if (!exec_short(s, ns)) {
                auto err = std::format("Unknown flag '{}'!\n", s);
                if (_fatal) throw std::runtime_error(err);
                else std::cout << err;
            }
        }
    }
}
