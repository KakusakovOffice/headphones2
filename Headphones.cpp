#include "Headphones.hpp"

std::string equalizer_mode_to_string(EqualizerMode equalizer_mode)
{
    switch (equalizer_mode)
    {
    case EqualizerMode::Normal:
        return "Обычный";
    case EqualizerMode::Bass:
        return "Низкие частоты";
    case EqualizerMode::Treble:
        return "Высокие частоты";
    case EqualizerMode::Vocal:
        return "Вокал";
    default:
        return "[Неизвестный режим эквалайзера]";
    }
}

std::optional<EqualizerMode> equalizer_mode_from_string(std::string string)
{
    if (string == "Обычный")
    {
        return EqualizerMode::Normal;
    }
    if (string == "Низкие частоты")
    {
        return EqualizerMode::Bass;
    }
    if (string == "Высокие частоты")
    {
        return EqualizerMode::Treble;
    }
    if (string == "Вокал")
    {
        return EqualizerMode::Vocal;
    }
    return std::nullopt;
}

Headphones::Headphones() :
    Headphones(
        "N/A",
        "N/A",
        "N/A",
        1.0,
        false,
        false,
        EqualizerMode::Normal
    )
{}

Headphones::Headphones(
    std::string producer_name,
    std::string model_name,
    std::string price,
    double volume,
    bool is_noise_canceling_enabled,
    bool is_microphone_enabled,
    EqualizerMode equalizer_mode
) :
    m_producer_name(producer_name),
    m_model_name(model_name),
    m_price(price),
    m_volume(volume),
    m_is_noise_canceling_enabled(is_noise_canceling_enabled),
    m_is_microphone_enabled(is_microphone_enabled),
    m_equalizer_mode(equalizer_mode)
{}

std::string Headphones::get_producer_name() const
{
    return m_producer_name;
}
std::string Headphones::get_model_name() const
{
    return m_model_name;
}
std::string Headphones::get_price() const
{
    return m_price;
}
double Headphones::get_volume() const
{
    return m_volume;
}
bool Headphones::is_noise_canceling_enabled() const
{
    return m_is_noise_canceling_enabled;
}
bool Headphones::is_microphone_enabled() const
{
    return m_is_microphone_enabled;
}
EqualizerMode Headphones::get_equalizer_mode() const
{
    return m_equalizer_mode;
}

void Headphones::set_producer_name(std::string producer_name)
{
    m_producer_name = producer_name;
}
void Headphones::set_model_name(std::string model_name)
{
    m_model_name = model_name;
}
void Headphones::set_price(std::string price)
{
    m_price = price;
}
void Headphones::set_volume(double volume)
{
    m_volume = volume;
}
void Headphones::toggle_noise_canceling()
{
    m_is_noise_canceling_enabled = !m_is_noise_canceling_enabled;
}
void Headphones::toggle_microphone()
{
    m_is_microphone_enabled = !m_is_microphone_enabled;
}
void Headphones::set_equalizer_mode(EqualizerMode equalizer_mode)
{
    m_equalizer_mode = equalizer_mode;
}

std::ostream& operator<<(std::ostream& os, const Headphones& headphones)
{
    os << "Список параметров наушников:" << "\n";
    os << "  Производитель: " << headphones.get_producer_name() << "\n";
    os << "  Название модели: " << headphones.get_model_name() << "\n";
    os << "  Цена: " << headphones.get_price() << "\n";
    os << "  Громкость: " << headphones.get_volume() << "\n";
    os << "  Шумоподавление: " << (headphones.is_noise_canceling_enabled() ? "Вкл" : "Выкл") << "\n";
    os << "  Микрофон: " << (headphones.is_microphone_enabled() ? "Вкл" : "Выкл") << "\n";
    os << "  Режим эквалайзера: " << equalizer_mode_to_string(headphones.get_equalizer_mode()) << "\n";
    return os;
}
