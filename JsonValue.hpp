#pragma once
#include <iostream>
#include <variant>
#include <unordered_map>
#include <vector>
#include <cstdint>

class LineHint {
public:
    std::uintptr_t line = 1;
    std::uintptr_t letter = 1;
};

class JsonParseError {
public:
    class UnknownEscapeSequence {};
    class LineBreakInString {};
    class EofInString {};
    class IoError {};
    class NumberOutOfRange {};
    class UnexpectedToken {
    public:
        std::wstring expected;
    };
    class MultipleValues {};

    using Variant =
        std::variant<
            UnknownEscapeSequence,
            LineBreakInString,
            EofInString,
            IoError,
            NumberOutOfRange,
            UnexpectedToken,
            MultipleValues
        >;
    Variant variant;
    LineHint line_hint;

    JsonParseError(
        Variant variant,
        LineHint line_hint
    );
};

using JsonParseResult = std::variant<class JsonValue, JsonParseError>;

class JsonValue {
public:
    class String {
    public:
        std::wstring value;
        String(std::wstring value) : value(value) {}
    };
    class Number {
    public:
        double value;
        Number(double value) : value(value) {}
    };
    class Object {
    public:
        std::unordered_map<std::wstring, JsonValue> value;
    };
    class Array {
    public:
        std::vector<JsonValue> value;
    };
    class Boolean {
    public:
        bool value;
        Boolean(bool value) : value(value) {}
    };
    class Null {};

    using Variant =
        std::variant<
            String,
            Number,
            Object,
            Array,
            Boolean,
            Null
        >;

    Variant variant;

    JsonValue(Variant variant);


    void serialize(std::wostream& os) const;
    static JsonParseResult deserialize(std::wistream& is);
};
