TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        HeadphoneList.cpp \
        Headphones.cpp \
        Main.cpp \
        TextMenu.cpp

HEADERS += \
    Headphones.hpp \
    HeadphonesList.hpp \
    TextMenu.hpp
