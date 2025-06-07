#include "HeadphonesList.hpp"
#include <utility>

Headphones& HeadphonesList::Node::value()
{
    return m_value;
}
const Headphones& HeadphonesList::Node::cvalue() const
{
    return m_value;
}
HeadphonesList::Node::node_ptr HeadphonesList::Node::get_next() const
{
    return m_next;
}
HeadphonesList::Node::node_ptr HeadphonesList::Node::get_prev() const
{
    return m_prev;
}

void HeadphonesList::Node::set_next(node_ptr next)
{
    m_next = next;
}
void HeadphonesList::Node::set_prev(node_ptr prev)
{
    m_prev = prev;
}
void HeadphonesList::Node::disconnect()
{
    set_next(nullptr);
    set_prev(nullptr);
}

HeadphonesList::Iterator::Iterator(
    std::shared_ptr<Node> ptr
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

HeadphonesList::ConstIterator::ConstIterator(
    HeadphonesList::ConstIterator::value_type ptr
) :
    m_ptr(ptr)
{}


HeadphonesList::ConstIterator::ConstIterator(
    HeadphonesList::Iterator iter
) :
    m_ptr(*iter)
{}

HeadphonesList::ConstIterator::reference HeadphonesList::ConstIterator::operator*()
{
    return m_ptr;
}
HeadphonesList::ConstIterator::pointer HeadphonesList::ConstIterator::operator->()
{
    return &m_ptr;
}
HeadphonesList::ConstIterator& HeadphonesList::ConstIterator::operator++()
{
    m_ptr = m_ptr->get_next();
    return *this;
}
HeadphonesList::ConstIterator HeadphonesList::ConstIterator::operator++(int)
{
    HeadphonesList::ConstIterator result = *this;
    ++(*this);
    return result;
}
HeadphonesList::ConstIterator& HeadphonesList::ConstIterator::operator--()
{
    m_ptr = m_ptr->get_prev();
    return *this;
}
HeadphonesList::ConstIterator HeadphonesList::ConstIterator::operator--(int)
{
    HeadphonesList::ConstIterator result = *this;
    --(*this);
    return result;
}
bool operator== (const HeadphonesList::ConstIterator& a, const HeadphonesList::ConstIterator& b)
{
    return a.m_ptr == b.m_ptr;
}
bool operator!= (const HeadphonesList::ConstIterator& a, const HeadphonesList::ConstIterator& b)
{
    return !(a == b);
}
void swap(HeadphonesList::ConstIterator& a, HeadphonesList::ConstIterator& b)
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
    return Iterator(m_head);
}
HeadphonesList::Iterator HeadphonesList::tail()
{
    return Iterator(m_tail);
}
HeadphonesList::ConstIterator HeadphonesList::chead() const
{
    return ConstIterator((Node::const_node_ptr)m_head);
}
HeadphonesList::ConstIterator HeadphonesList::ctail() const
{
    return ConstIterator((Node::const_node_ptr)m_tail);
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
    Node::node_ptr node = *it;
    if (!node)
    {
        return;
    }

    Node::node_ptr next = node->get_next();
    Node::node_ptr prev = node->get_prev();

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

HeadphonesList::DeserializeError::DeserializeError(
    std::string message
) :
    message(message)
{}

HeadphonesList::SerializeError::SerializeError(
    std::string message
) :
    message(message)
{}

HeadphonesList::SerializeResult HeadphonesList::serialize(std::ostream& os) const
{
    auto end_symb = '^';
    auto delim = '|';
    auto io_err = "Ошибка ввода-вывода при записи файла";

    for (ConstIterator it = chead();;it++)
    {
        try
        {
            const auto& value = (*it)->cvalue();
            os << value.get_producer_name().size() << delim << value.get_producer_name();
            os << value.get_model_name().size() << delim << value.get_model_name();
            os << value.get_price().size() << delim << value.get_price();
            std::string buffer;
            buffer = std::to_string(value.get_volume());
            os << buffer.size() << delim << buffer;
            buffer = std::to_string(value.is_noise_canceling_enabled());
            os << buffer.size() << delim << buffer;
            buffer = std::to_string(value.is_microphone_enabled());
            os << buffer.size() << delim << buffer;
            buffer = equalizer_mode_to_string(value.get_equalizer_mode());
            os << buffer.size() << delim << buffer;

            if (it == ctail())
            {
                os << end_symb;
                return std::monostate();
            }
        }
        catch (std::ios_base::failure e)
        {
            return SerializeError(io_err);
        }
    }
}

std::variant<std::string, HeadphonesList::DeserializeError> HeadphonesList::deserialize_read_section(std::istream& is)
{
    const auto delim = '|';
    const auto eof_err = "Файл неожиданно обрывается.";
    const auto io_err = "Ошибка ввода-вывода при чтении файла.";
    const auto ill_err = "Файл поврежден или записан некорректно.";

    std::string buffer;
    while (true)
    {
        std::istream::int_type ch;
        try
        {
            ch = is.get();
        }
        catch (std::ios_base::failure e) {}

        if (is.bad() || is.fail())
        {
            return DeserializeError(io_err);
        }
        if (is.eof())
        {
            return DeserializeError(eof_err);
        }

        if (ch == delim)
        {
            unsigned long long len;
            try
            {
                len = std::stoull(buffer);
            }
            catch (std::invalid_argument e)
            {
                return DeserializeError(ill_err);
            }
            catch (std::out_of_range e)
            {
                return DeserializeError(ill_err);
            }
            if (sizeof(unsigned long long) > sizeof(std::size_t) && len > (unsigned long long)SIZE_MAX)
            {
                return DeserializeError(ill_err);
            }
            buffer.resize((std::size_t)len);
            try
            {
                is.read(buffer.data(), len);
            }
            catch (std::ios_base::failure e) {}

            if (is.bad() || is.fail())
            {
                return DeserializeError(io_err);
            }
            if (is.eof())
            {
                return DeserializeError(eof_err);
            }

            return buffer;
        }
        buffer += ch;
    }
}

HeadphonesList::DeserializeResult HeadphonesList::deserialize(std::istream& is)
{
    const auto end_symb = '^';
    const auto io_err = "Ошибка ввода-вывода при чтении файла.";
    const auto ill_err = "Файл поврежден или записан некорректно.";
    const auto eof_err = "Файл неожиданно обрывается.";
    HeadphonesList list {};

    while (true)
    {
        std::istream::int_type ch;
        try
        {
            ch = is.peek();
        }
        catch (std::ios_base::failure e) {}

        if (is.bad() || is.fail())
        {
            return DeserializeError(io_err);
        }
        if (is.eof())
        {
            return DeserializeError(eof_err);
        }

        if (ch == end_symb)
        {
            return list;
        }

        auto result = deserialize_read_section(is);
        if (std::holds_alternative<HeadphonesList::DeserializeError>(result))
        {
            return std::get<HeadphonesList::DeserializeError>(result);
        }
        std::string producer_name = std::get<std::string>(result);

        result = deserialize_read_section(is);
        if (std::holds_alternative<HeadphonesList::DeserializeError>(result))
        {
            return std::get<HeadphonesList::DeserializeError>(result);
        }
        std::string model_name = std::get<std::string>(result);

        result = deserialize_read_section(is);
        if (std::holds_alternative<HeadphonesList::DeserializeError>(result))
        {
            return std::get<HeadphonesList::DeserializeError>(result);
        }
        std::string price = std::get<std::string>(result);

        result = deserialize_read_section(is);
        if (std::holds_alternative<HeadphonesList::DeserializeError>(result))
        {
            return std::get<HeadphonesList::DeserializeError>(result);
        }
        std::string buffer = std::get<std::string>(result);
        double volume;
        try
        {
            volume = std::stod(buffer);
        }
        catch (std::invalid_argument e)
        {
            return DeserializeError(ill_err);
        }
        catch (std::out_of_range e)
        {
            return DeserializeError(ill_err);
        }

        result = deserialize_read_section(is);
        if (std::holds_alternative<HeadphonesList::DeserializeError>(result))
        {
            return std::get<HeadphonesList::DeserializeError>(result);
        }
        buffer = std::get<std::string>(result);
        bool is_noise_canceling_enabled;
        try
        {
            is_noise_canceling_enabled = (bool)std::stoi(buffer);
        }
        catch (std::invalid_argument e)
        {
            return DeserializeError(ill_err);
        }
        catch (std::out_of_range e)
        {
            return DeserializeError(ill_err);
        }

        result = deserialize_read_section(is);
        if (std::holds_alternative<HeadphonesList::DeserializeError>(result))
        {
            return std::get<HeadphonesList::DeserializeError>(result);
        }
        buffer = std::get<std::string>(result);
        bool is_microphone_enabled;
        try
        {
            is_microphone_enabled = (bool)std::stoi(buffer);
        }
        catch (std::invalid_argument e)
        {
            return DeserializeError(ill_err);
        }
        catch (std::out_of_range e)
        {
            return DeserializeError(ill_err);
        }

        result = deserialize_read_section(is);
        if (std::holds_alternative<HeadphonesList::DeserializeError>(result))
        {
            return std::get<HeadphonesList::DeserializeError>(result);
        }
        buffer = std::get<std::string>(result);
        EqualizerMode equalizer_mode;
        auto equalizer_mode_opt = equalizer_mode_from_string(buffer);
        if (!equalizer_mode_opt)
        {
            return DeserializeError(ill_err);
        }
        equalizer_mode = equalizer_mode_opt.value();

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
}

HeadphonesList::Iterator HeadphonesList::insert_internal(
    Node::node_ptr node,
    Node::node_ptr next,
    Node::node_ptr prev
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
