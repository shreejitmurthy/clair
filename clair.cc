//
// Created by Shreejit Murthy on 6/2/2026
//

#include "clair.hh"

#include <algorithm>

inline bool check_prefix(const std::string& s, const std::string& p) {
    return s.length() >= p.length() && s.substr(0, p.length()) == p;
}

void clair::parse(int argc, char** argv) {
    std::vector<std::string> raw_args(argv, argv + argc);
    
    for (int i = 1; i < raw_args.size(); i++) {
        auto& s = raw_args[i];
        auto& ns = raw_args[i + 1];
        if (check_prefix(s, "--") && !check_prefix(ns, "--")) {
            for (auto& f : flags) {
                if (f.first.long_name == s.substr(2, s.length())) {
                    f.second(ns);
                }
            }
        }
    }
}
