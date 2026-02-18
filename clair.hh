//
// Created by Shreejit Murthy on 6/2/2026
//

#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <functional>


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
            return h1 ^ (h2 + __PHI + (h1 << 6) + (h1 >> 2));
        }
    } OptionDefHash;

    std::unordered_map<OptionDef, Callback, OptionDefHash> options;

    bool exec_long(std::vector<std::string> args, int i, const std::string& s);
    bool exec_short(std::vector<std::string> args, int i, const std::string& s);
    void help(std::vector<std::string>);
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
