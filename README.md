# clair
**Quick and Simple Parser for CLI Application**

Minimal Example:
```cpp
#include <iostream>

#include "clair.hh"

void name_response(std::string v) {
    std::cout << "Hello, " << v << std::endl;
}

int main(int argc, char** argv) {
    clair cl("My CLI Program");
    
    cl.flag("name", name_response, "n");

    cl.parse(argc, argv);

    return 0;
}
```
```
./cli-app --name Shreejit
Hello, Shreejit
./cli-app -n "Shreejit Murthy"
Hello, Shreejit Murthy
```