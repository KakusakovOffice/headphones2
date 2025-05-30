TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        HeadphoneList.cpp \
        Headphones.cpp \
        JsonValue.cpp \
        main.cpp

HEADERS += \
    Headphones.hpp \
    HeadphonesList.hpp \
    JsonValue.hpp
