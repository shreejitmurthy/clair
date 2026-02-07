# clair
**Quick and Simple Parser for CLI Applications**

Minimal Example:
```cpp
#include <iostream>
#include <cstdlib>

#include "clair.hh"

void name_response(std::vector<std::string> v) {
    std::cout << "Hello, " << v[0] << std::endl;
}

void sum(std::vector<std::string> v) {
    int s = 0;
    for (auto i : v) {
        int n = std::atoi(i.c_str());
        s += n;
    }
    std::cout << "The sum is: " << s << std::endl;
}

int main(int argc, char** argv) {
    clair::parser app("My CLI Program");

    app.exec("./cli-app");
    app.version("0.1.0");
    app.description("The description to my very awesome CLI program.");
    app.short_description("This is my CLI program.");
    app.notes("This is just an example, there is more to be done with clair!");
    
    app.option("name", name_response, 1, "Provide your name so we can say hello!", 'n');
    app.option("sum", sum, 3, "Enter 3 arguments and get the sum!");

    app.parse(argc, argv);

    return 0;
}
```
```
$ ./cli-app --name Shreejit
Hello, Shreejit
$ ./cli-app -n "Shreejit Murthy"
Hello, Shreejit Murthy
$ ./cli-app --sum 18 44 5
The sum is: 67
```
**Automatic `--help` command output:**
```
$ ./cli-app --help
My CLI Program v0.1.0 - This is my CLI program.

USAGE
    ./cli-app [OPTIONS] [ARGS...]
    ./cli-app --help
    ./cli-app --version

DESCRIPTION
    The description to my very awesome CLI program.

OPTIONS
    --sum       Enter 3 arguments and get the sum!
    -n, --name  Provide your name so we can say hello!
    --help      Show help and exit.

NOTES
    This is just an example, there is more to be done with clair!
```
*clair is written in 95 semicolons (195 LOC)*