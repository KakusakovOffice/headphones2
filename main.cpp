#include "TextMenu.hpp"
#include <iostream>
#include <windows.h>
#include <locale>
#include <clocale>
#include <io.h>
#include <fcntl.h>

void try_set_locale() {

    if (!SetConsoleCP(CP_UTF8)) {
        std::cerr << "Warning: failed to set input code page to UTF-8." << std::endl;
        return;
    }

    if (!SetConsoleOutputCP(CP_UTF8))
    {
        std::cerr << "Warning: failed to set output code page to UTF-8." << std::endl;
        return;
    }

    _setmode(_fileno(stdout), _O_BINARY);
    _setmode(_fileno(stdin), _O_BINARY);
    _setmode(_fileno(stderr), _O_BINARY);

    const char* locales[] = {
        ".UTF8",        // Windows 10 1903+
        ".65001",       // Code page number
        "en_US.UTF-8",  // Unix-style
        "C.UTF-8",      // Alternative
        "English_United States.65001"  // Full Windows name
    };

    for (const char* loc : locales) {
        if (std::setlocale(LC_ALL, loc) != nullptr) {
            try
            {
                std::locale::global(std::locale(loc));
                std::cout.imbue(std::locale());
                std::wcout.imbue(std::locale());
                return;
            }
            catch (...)
            {
                continue;
            }
        }
    }

    std::setlocale(LC_ALL, "");
    std::cerr << "Warning: Could not set UTF-8 locale, utf-8 may not work correctly.\n";
}

int main()
{
    try_set_locale();

    TextMenu::session();
    return 0;
}
