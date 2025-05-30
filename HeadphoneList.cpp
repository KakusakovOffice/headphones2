#include "HeadphonesList.hpp"
#include <utility>

Headphones& HeadphonesListNode::value()
{
    return m_value;
}
HeadphonesListNode::node_ptr HeadphonesListNode::get_next() const
{
    return m_next;
}
HeadphonesListNode::node_ptr HeadphonesListNode::get_prev() const
{
    return m_prev;
}

void HeadphonesListNode::set_next(node_ptr next)
{
    m_next = next;
}
void HeadphonesListNode::set_prev(node_ptr prev)
{
    m_prev = prev;
}
void HeadphonesListNode::disconnect()
{
    set_next(nullptr);
    set_prev(nullptr);
}

HeadphonesList::Iterator::Iterator(
    std::shared_ptr<HeadphonesListNode> ptr
) :
    m_ptr(ptr)
{}
HeadphonesList::Iterator::reference HeadphonesList::Iterator::operator*()
{
    return m_ptr;
}
HeadphonesList::Iterator::pointer HeadphonesList::Iterator::operator->()
{
    return &m_ptr;
}
HeadphonesList::Iterator& HeadphonesList::Iterator::operator++()
{
    m_ptr = m_ptr->get_next();
    return *this;
}
HeadphonesList::Iterator HeadphonesList::Iterator::operator++(int)
{
    HeadphonesList::Iterator result = *this;
    ++(*this);
    return result;
}
HeadphonesList::Iterator& HeadphonesList::Iterator::operator--()
{
    m_ptr = m_ptr->get_prev();
    return *this;
}
HeadphonesList::Iterator HeadphonesList::Iterator::operator--(int)
{
    HeadphonesList::Iterator result = *this;
    --(*this);
    return result;
}
bool operator== (const HeadphonesList::Iterator& a, const HeadphonesList::Iterator& b)
{
    return a.m_ptr == b.m_ptr;
}
bool operator!= (const HeadphonesList::Iterator& a, const HeadphonesList::Iterator& b)
{
    return !(a == b);
}
void swap(HeadphonesList::Iterator& a, HeadphonesList::Iterator& b)
{
    std::swap(a.m_ptr, b.m_ptr);
}

HeadphonesList::HeadphonesList() :
    m_head(nullptr),
    m_tail(nullptr),
    m_count(0)
{}

HeadphonesList::Iterator HeadphonesList::head()
{
    return m_head;
}
HeadphonesList::Iterator HeadphonesList::tail()
{
    return m_tail;
}
std::uintptr_t HeadphonesList::count() const
{
    return m_count;
}
bool HeadphonesList::is_empty() const
{
    return count() == 0;
}
bool HeadphonesList::is_not_empty() const
{
    return !is_empty();
}

void HeadphonesList::remove(HeadphonesList::Iterator it)
{
    HeadphonesListNode::node_ptr node = *it;
    if (!node)
    {
        return;
    }

    HeadphonesListNode::node_ptr next = node->get_next();
    HeadphonesListNode::node_ptr prev = node->get_prev();

    if (next)
    {
        next->set_prev(prev);
    }
    else
    {
        m_tail = prev;
    }

    if (prev)
    {
        prev->set_next(next);
    }
    else
    {
        m_head = next;
    }

    node->disconnect();
    m_count--;
}


JsonValue::Array HeadphonesList::to_json()
{
    JsonValue::Array array = JsonValue::Array();
    for (Iterator it = head();; it++)
    {
        const Headphones& headphones = (*it)->value();
        JsonValue::Object object = JsonValue::Object();
        object.value.insert({L"Название Производителя", JsonValue(JsonValue::String(headphones.get_producer_name()))});
        object.value.insert({L"Название Модели", JsonValue(JsonValue::String(headphones.get_model_name()))});
        object.value.insert({L"Цена", JsonValue(JsonValue::String(headphones.get_price()))});
        object.value.insert({L"Громкость", JsonValue(JsonValue::Number(headphones.get_volume()))});
        object.value.insert({L"Шумоподавление", JsonValue(JsonValue::Boolean(headphones.is_noise_canceling_enabled()))});
        object.value.insert({L"Микрофон", JsonValue(JsonValue::Boolean(headphones.is_microphone_enabled()))});
        object.value.insert({L"Режим Эквалайзера", JsonValue(JsonValue::String(equalizer_mode_to_string(headphones.get_equalizer_mode())))});
        array.value.push_back(JsonValue(object));

        if (it == tail()) break;
    }
    return array;
}

template<class JsonValueVariant>
std::variant<
    JsonValueVariant,
    HeadphonesListParseError
> from_json_field_lookup_helper(
    std::size_t index,
    const JsonValue::Object& object,
    const std::wstring& field_name
)
{
    auto it = object.value.find(field_name);
    if (it == object.value.end())
    {
        return HeadphonesListParseError(HeadphonesListParseError::ElementFieldIssue(index, field_name, L"Поле отсутствует"));
    }

    JsonValue value = it->second;
    if (!std::holds_alternative<JsonValueVariant>(value.variant))
    {
        return HeadphonesListParseError(HeadphonesListParseError::ElementFieldIssue(index, field_name, L"Поле имеет неверный тип"));
    }

    return std::get<JsonValueVariant>(value.variant);
}

HeadphonesListParseResult HeadphonesList::from_json(JsonValue::Array array)
{
    HeadphonesList list = HeadphonesList();
    for (std::size_t index = 0; index < array.value.size(); index++)
    {
        JsonValue value = array.value[index];
        if (!std::holds_alternative<JsonValue::Object>(value.variant))
        {
            return HeadphonesListParseError(HeadphonesListParseError::ElementIsNotObject(index));
        }
        JsonValue::Object object = std::get<JsonValue::Object>(value.variant);

        auto lookup_result_str = from_json_field_lookup_helper<JsonValue::String>(index, object, L"Название Производителя");
        if (std::holds_alternative<HeadphonesListParseError>(lookup_result_str))
        {
            return std::get<HeadphonesListParseError>(lookup_result_str);
        }
        std::wstring producer_name = std::get<JsonValue::String>(lookup_result_str).value;

        lookup_result_str = from_json_field_lookup_helper<JsonValue::String>(index, object, L"Название Модели");
        if (std::holds_alternative<HeadphonesListParseError>(lookup_result_str))
        {
            return std::get<HeadphonesListParseError>(lookup_result_str);
        }
        std::wstring model_name = std::get<JsonValue::String>(lookup_result_str).value;

        lookup_result_str = from_json_field_lookup_helper<JsonValue::String>(index, object, L"Цена");
        if (std::holds_alternative<HeadphonesListParseError>(lookup_result_str))
        {
            return std::get<HeadphonesListParseError>(lookup_result_str);
        }
        std::wstring price = std::get<JsonValue::String>(lookup_result_str).value;

        auto lookup_result_num = from_json_field_lookup_helper<JsonValue::Number>(index, object, L"Громкость");
        if (std::holds_alternative<HeadphonesListParseError>(lookup_result_num))
        {
            return std::get<HeadphonesListParseError>(lookup_result_num);
        }
        double volume = std::get<JsonValue::Number>(lookup_result_num).value;

        auto lookup_result_bool = from_json_field_lookup_helper<JsonValue::Boolean>(index, object, L"Шумоподавление");
        if (std::holds_alternative<HeadphonesListParseError>(lookup_result_bool))
        {
            return std::get<HeadphonesListParseError>(lookup_result_bool);
        }
        bool is_noise_canceling_enabled = std::get<JsonValue::Boolean>(lookup_result_bool).value;

        lookup_result_bool = from_json_field_lookup_helper<JsonValue::Boolean>(index, object, L"Микрофон");
        if (std::holds_alternative<HeadphonesListParseError>(lookup_result_bool))
        {
            return std::get<HeadphonesListParseError>(lookup_result_bool);
        }
        bool is_microphone_enabled = std::get<JsonValue::Boolean>(lookup_result_bool).value;

        lookup_result_str = from_json_field_lookup_helper<JsonValue::String>(index, object, L"Режим Эквалайзера");
        if (std::holds_alternative<HeadphonesListParseError>(lookup_result_str))
        {
            return std::get<HeadphonesListParseError>(lookup_result_str);
        }
        auto equalizer_mode_opt = equalizer_mode_from_string(std::get<JsonValue::String>(lookup_result_str).value);
        if (!equalizer_mode_opt)
        {
            return HeadphonesListParseError(
                HeadphonesListParseError::ElementFieldIssue(index, L"Режим Эквалайзера", L"Неизветсный режим эквалайзера")
            );
        }
        EqualizerMode equalizer_mode = equalizer_mode_opt.value();

        list.emplace_after(
            list.tail(),
            producer_name,
            model_name,
            price,
            volume,
            is_noise_canceling_enabled,
            is_microphone_enabled,
            equalizer_mode
        );
    }

    return list;
}

HeadphonesList::Iterator HeadphonesList::insert_internal(
    HeadphonesListNode::node_ptr node,
    HeadphonesListNode::node_ptr next,
    HeadphonesListNode::node_ptr prev
)
{
    node->set_next(next);
    node->set_prev(prev);

    if (next)
    {
        next->set_prev(node);
    }
    else
    {
        m_tail = node;
    }

    if (prev)
    {
        prev->set_next(node);
    }
    else
    {
        m_head = node;
    }

    m_count++;
    return Iterator(node);
}
