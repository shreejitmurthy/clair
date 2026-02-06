# clair
**Quick and Simple Parser for CLI Applications**

Minimal Example:
```cpp
#include <iostream>

#include "clair.hh"

void name_response(std::string v) {
    std::cout << "Hello, " << v << std::endl;
}

int main(int argc, char** argv) {
    clair cl("My CLI Program");

    cl.exec("./main");
    cl.version("0.1.0");
    cl.description("The description to my very awesome CLI program.");
    cl.short_description("This is my CLI program.");
    cl.notes("This is just an example, there is more to be done with clair!");
    
    cl.flag("name", name_response, "n", "Provide your name so we can say hello!");

    cl.parse(argc, argv);

    return 0;
}
```
```
$ ./cli-app --name Shreejit
Hello, Shreejit
$ ./cli-app -n "Shreejit Murthy"
Hello, Shreejit Murthy
```
**Automatic `--help` command output:**
```
$ ./cli-app --help
My CLI Program v0.1.0 - This is my CLI program.

USAGE
    ./main [GLOBAL_OPTIONS] [ARGS...]
    ./main --help
    ./main --version

DESCRIPTION
    The description to my very awesome CLI program.

GLOBAL OPTIONS
    -n, --name  Provide your name so we can say hello!
    --help      Show help and exit.

NOTES
    This is just an example, there is more to be done with clair!
```
*Written in 79 semicolons (153 LOC)*
