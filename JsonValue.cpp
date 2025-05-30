#include "JsonValue.hpp"
#include <cwctype>
#include <optional>
#include <type_traits>
#include <stdexcept>
#include <iomanip>

JsonParseError::JsonParseError(
    Variant variant,
    LineHint line_hint
) :
    variant(variant),
    line_hint(line_hint)
{}

class TokenizerError {
public:
    class NewlineInString {};
    class UnknownEscapeSequence {};
    class EofInString {};
    class IoError {};

    using Variant =
        std::variant<
            NewlineInString,
            UnknownEscapeSequence,
            EofInString,
            IoError
        >;
    Variant variant;
    LineHint line_hint;

    TokenizerError(
        Variant variant,
        LineHint line_hint
    ) :
        variant(variant),
        line_hint(line_hint)
    {}
};

class Token {
public:
    class ObjectOpened {};
    class ObjectClosed {};
    class ArrayOpened {};
    class ArrayClosed {};
    class Comma {};
    class Colon {};
    class String {
    public:
        std::wstring value;
        String(std::wstring value) : value(value) {}
    };
    class Identifier {
    public:
        std::wstring value;
        Identifier(std::wstring value) : value(value) {}
    };

    using Variant =
        std::variant<
            ObjectOpened,
            ObjectClosed,
            ArrayOpened,
            ArrayClosed,
            Comma,
            Colon,
            String,
            Identifier
        >;
    Variant variant;
    LineHint line_hint;

    Token(
        Variant variant,
        LineHint line_hint
    ) :
        variant(variant),
        line_hint(line_hint)
    {}
};

using TokenizerResult = std::variant<std::vector<Token>, TokenizerError>;

TokenizerResult tokenize(std::wistream& is)
{
    LineHint line_hint;
    std::vector<Token> tokens;
    for (wint_t ch = is.get();;)
    {
        if (!is.good())
        {
            return TokenizerError(TokenizerError::IoError(), LineHint());
        }

        if (ch == WEOF)
        {
            return tokens;
        }

        if (ch == L'{')
        {
            tokens.emplace_back(Token::ObjectOpened(), line_hint);
        }
        else if (ch == L'}')
        {
            tokens.emplace_back(Token::ObjectClosed(), line_hint);
        }
        else if (ch == L'[')
        {
            tokens.emplace_back(Token::ArrayOpened(), line_hint);
        }
        else if (ch == L']')
        {
            tokens.emplace_back(Token::ArrayClosed(), line_hint);
        }
        else if (ch == L',')
        {
            tokens.emplace_back(Token::Comma(), line_hint);
        }
        else if (ch == L':')
        {
            tokens.emplace_back(Token::Colon(), line_hint);
        }
        else if (ch == L'"')
        {
            std::wstring value;
            LineHint token_line_hint = line_hint;

            for (wint_t ch = is.get();;)
            {
                line_hint.letter++;

                if (!is.good())
                {
                    return TokenizerError(TokenizerError::IoError(), LineHint());
                }

                if (ch == WEOF)
                {
                    return TokenizerError(TokenizerError::EofInString(), LineHint());
                }

                if (ch == L'\n')
                {
                    return TokenizerError(TokenizerError::NewlineInString(), line_hint);
                }

                if (ch == L'"')
                {
                    tokens.emplace_back(Token::Identifier(value), token_line_hint);
                    break;
                }

                if (ch == L'\\')
                {
                    wint_t ch = is.get();

                    if (!is.good())
                    {
                        return TokenizerError(TokenizerError::IoError(), LineHint());
                    }

                    if (ch == WEOF)
                    {
                        return TokenizerError(TokenizerError::EofInString(), LineHint());
                    }

                    switch (ch)
                    {
                    case L'"':
                        value.push_back(L'\"');
                        break;
                    case L'\\':
                        value.push_back(L'\\');
                        break;
                    case L'n':
                        value.push_back(L'\n');
                        break;
                    case L'r':
                        value.push_back(L'\r');
                        break;
                    case L't':
                        value.push_back(L'\t');
                        break;
                    case L'b':
                        value.push_back(L'\b');
                        break;
                    case L'f':
                        value.push_back(L'\f');
                        break;
                    default:
                        return TokenizerError(TokenizerError::UnknownEscapeSequence(), line_hint);
                        break;
                    }
                }
                else
                {
                    value.push_back(ch);
                }

            }
        }
        else if (!std::iswspace(ch))
        {
            std::wstring value;
            LineHint token_line_hint = line_hint;
            value.push_back(ch);
            for (wint_t ch = is.get();;)
            {
                line_hint.letter++;

                if (!is.good())
                {
                    return TokenizerError(TokenizerError::IoError(), LineHint());
                }

                if (ch == WEOF || std::iswspace(ch))
                {
                    tokens.emplace_back(Token::Identifier(value), token_line_hint);
                    break;
                }

                value.push_back(ch);
            }
        }

        if (ch == L'\n')
        {
            line_hint.line++;
            line_hint.letter = 1;
        }
        else
        {
            line_hint.letter++;
        }
    }
}

class TokenStream {
public:
    std::vector<Token> tokens;
    std::size_t index;

    TokenStream(
        std::vector<Token>&& tokens
    ) :
        tokens(tokens),
        index(0)
    {}

    bool has_finished()
    {
        return index == tokens.size();
    }
    Token* peek()
    {
        if (has_finished())
        {
            return nullptr;
        }

        return &tokens[index];
    }
    Token* next()
    {
        Token* result = peek();
        if (result)
        {
            index++;
        }
        return result;
    }
};

JsonParseResult parse(TokenStream ts)
{
    Token* token_p = ts.next();
    if (!token_p)
    {
        JsonParseError::UnexpectedToken error;
        error.expected = L"value (got EOF)";
        return JsonParseError(error, LineHint());
    }
    Token& token = *token_p;
    return std::visit([&ts, &token](auto&& arg) -> JsonParseResult
    {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, Token::ObjectOpened>)
        {
            JsonValue::Object object = JsonValue::Object();
            Token* token_p = ts.next();
            if (token_p && std::holds_alternative<Token::ObjectClosed>(token_p->variant))
            {
                return JsonValue(object);
            }

            while (token_p)
            {
                Token& token = *token_p;
                JsonParseResult parse_result = parse(ts);
                if (std::holds_alternative<JsonParseError>(parse_result))
                {
                    return parse_result;
                }

                JsonValue key = std::get<JsonValue>(parse_result);
                if (!std::holds_alternative<JsonValue::String>(key.variant))
                {
                    JsonParseError::UnexpectedToken error;
                    error.expected = L"string";
                    return JsonParseError(error, token.line_hint);
                }

                std::wstring string_key = std::get<JsonValue::String>(key.variant).value;

                if (ts.has_finished())
                {
                    break;
                }

                token = *ts.next();
                if (!std::holds_alternative<Token::Colon>(token.variant))
                {
                    JsonParseError::UnexpectedToken error;
                    error.expected = L"\":\"";
                    return JsonParseError(error, token.line_hint);
                }

                parse_result = parse(ts);
                if (std::holds_alternative<JsonParseError>(parse_result))
                {
                    return parse_result;
                }

                JsonValue value = std::get<JsonValue>(parse_result);
                object.value.insert({string_key, value});

                token = *ts.next();
                if (std::holds_alternative<Token::ObjectClosed>(token.variant))
                {
                    return JsonValue(object);
                }
                else if (!std::holds_alternative<Token::Comma>(token.variant))
                {
                    JsonParseError::UnexpectedToken error;
                    error.expected = L"\",\"";
                    return JsonParseError(error, token.line_hint);
                }

                token_p = ts.next();
            }

            JsonParseError::UnexpectedToken error;
            error.expected = L"\"}\" (got EOF)";
            return JsonParseError(error, LineHint());
        }
        else if constexpr (std::is_same_v<T, Token::ArrayOpened>)
        {
            JsonValue::Array array = JsonValue::Array();
            Token* token_p = ts.next();
            if (token_p && std::holds_alternative<Token::ArrayClosed>(token_p->variant))
            {
                return JsonValue(array);
            }

            while (token_p)
            {
                Token& token = *token_p;
                JsonParseResult parse_result = parse(ts);
                if (std::holds_alternative<JsonParseError>(parse_result))
                {
                    return parse_result;
                }

                array.value.push_back(std::get<JsonValue>(parse_result));

                if (ts.has_finished())
                {
                    break;
                }

                token = *ts.next();
                if (std::holds_alternative<Token::ArrayClosed>(token.variant))
                {
                    return JsonValue(array);
                }
                else if (!std::holds_alternative<Token::Comma>(token.variant))
                {
                    JsonParseError::UnexpectedToken error;
                    error.expected = L"\",\"";
                    return JsonParseError(error, token.line_hint);
                }
                token_p = ts.next();
            }

            JsonParseError::UnexpectedToken error;
            error.expected = L"\"]\" (got EOF)";
            return JsonParseError(error, LineHint());
        }
        else if constexpr (std::is_same_v<T, Token::Identifier>)
        {
            if (arg.value == L"true")
            {
                return JsonValue(JsonValue::Boolean(true));
            }
            if (arg.value == L"false")
            {
                return JsonValue(JsonValue::Boolean(false));
            }
            if (arg.value == L"null")
            {
                return JsonValue(JsonValue::Null());
            }

            if (!std::iswdigit(arg.value[0]))
            {
                JsonParseError::UnexpectedToken error;
                error.expected = L"value";
                return JsonParseError(error, token.line_hint);
            }
            double value = 0;
            try
            {
                value = std::stod(arg.value);
            }
            catch (const std::invalid_argument& e)
            {
                JsonParseError::UnexpectedToken error;
                error.expected = L"value (got unsupported number fromat or malformed identifier)";
                return JsonParseError(error, token.line_hint);
            }
            catch (const std::out_of_range& e)
            {
                return JsonParseError(JsonParseError::NumberOutOfRange(), token.line_hint);
            }

            return JsonValue(JsonValue::Number(value));
        }
        else if constexpr (std::is_same_v<T, Token::String>)
        {
            return JsonValue(JsonValue::String(arg.value));
        }
        else
        {
            JsonParseError::UnexpectedToken error;
            error.expected = L"value";
            return JsonParseError(error, token.line_hint);
        }
    }, token.variant);
}

JsonValue::JsonValue(
    Variant variant
) :
    variant(variant)
{}

JsonParseResult JsonValue::deserialize(std::wistream& is)
{
    std::vector<Token> tokens;
    std::optional<TokenizerError> tokenizer_error_option;
    TokenizerResult tokenizer_result = tokenize(is);
    std::visit([&tokenizer_error_option, &tokens](auto&& arg)
    {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::vector<Token>>)
        {
            tokens = arg;
        }
        else if constexpr (std::is_same_v<T, TokenizerError>)
        {
            tokenizer_error_option = arg;
        }
        else
        {
            static_assert(false, "visitor is not exhaustive!");
        }
    }, tokenizer_result);

    if (tokenizer_error_option.has_value())
    {
        TokenizerError tokenizer_error = tokenizer_error_option.value();
        JsonParseError::Variant error_variant;
        std::visit([&error_variant](auto&& arg)
        {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, TokenizerError::NewlineInString>)
            {
                error_variant = JsonParseError::LineBreakInString();
            }
            else if constexpr (std::is_same_v<T, TokenizerError::EofInString>)
            {
                error_variant = JsonParseError::EofInString();
            }
            else if constexpr (std::is_same_v<T, TokenizerError::IoError>)
            {
                error_variant = JsonParseError::IoError();
            }
            else if constexpr (std::is_same_v<T, TokenizerError::UnknownEscapeSequence>)
            {
                error_variant = JsonParseError::UnknownEscapeSequence();
            }
            else
            {
                static_assert(false, "visitor is not exhaustive!");
            }
        }, tokenizer_error.variant);
        return JsonParseError(error_variant, tokenizer_error.line_hint);
    }

    TokenStream ts = TokenStream(std::move(tokens));
    JsonParseResult parse_result = parse(ts);
    if (std::holds_alternative<JsonParseError>(parse_result))
    {
        return parse_result;
    }

    if (!ts.has_finished())
    {
        Token& token = *ts.peek();
        return JsonParseError(JsonParseError::MultipleValues(), token.line_hint);
    }

    return parse_result;
}

void JsonValue::serialize(std::wostream& os) const
{
    std::visit([&os](auto&& arg)
    {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, JsonValue::String>)
        {
            os << std::quoted(arg.value);
        }
        else if constexpr (std::is_same_v<T, JsonValue::Number>)
        {
            os << arg.value;
        }
        else if constexpr (std::is_same_v<T, JsonValue::Boolean>)
        {
            os << (arg.value ? L"true" : L"false");
        }
        else if constexpr (std::is_same_v<T, JsonValue::Null>)
        {
            os << L"null";
        }
        else if constexpr (std::is_same_v<T, JsonValue::Object>)
        {
            os << L'{';
            bool is_first = true;
            for (auto& it : arg.value)
            {
                auto [key, value] = it;
                if (!is_first)
                {
                    os << L',';
                }
                else
                {
                    is_first = false;
                }
                os << std::quoted(key) << L':';
                value.serialize(os);
            }
            os << L'}';
        }
        else if constexpr (std::is_same_v<T, JsonValue::Array>)
        {
            os << L'[';
            bool is_first = true;
            for (const JsonValue& value : arg.value)
            {
                if (!is_first)
                {
                    os << L',';
                }
                else
                {
                    is_first = false;
                }
                value.serialize(os);
            }
            os << L']';
        }
        else
        {
            static_assert(false, "visitor is not exhaustive!");
        }
    }, variant);
}
