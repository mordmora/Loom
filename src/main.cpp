#include "driver.h"
#include <iostream>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <archivo_fuente>\n";
        return 1;
    }

    loom::LoomDriver driver(64 * 1024); // 64KB blocks
    return driver.run(argv[1]);
}
