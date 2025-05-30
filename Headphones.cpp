#include "Headphones.hpp"

std::wstring equalizer_mode_to_string(EqualizerMode equalizer_mode)
{
    switch (equalizer_mode)
    {
    case EqualizerMode::Normal:
        return L"Обычный";
    case EqualizerMode::Bass:
        return L"Низкие частоты";
    case EqualizerMode::Treble:
        return L"Высокие частоты";
    case EqualizerMode::Vocal:
        return L"Вокал";
    default:
        return L"[Неизвестный режим эквалайзера]";
    }
}

std::optional<EqualizerMode> equalizer_mode_from_string(std::wstring string)
{
    if (string == L"Обычный")
    {
        return EqualizerMode::Normal;
    }
    if (string == L"Низкие частоты")
    {
        return EqualizerMode::Bass;
    }
    if (string == L"Высокие частоты")
    {
        return EqualizerMode::Treble;
    }
    if (string == L"Вокал")
    {
        return EqualizerMode::Vocal;
    }
    return std::nullopt;
}

Headphones::Headphones(
    std::wstring producer_name,
    std::wstring model_name,
    std::wstring price,
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

std::wstring Headphones::get_producer_name() const
{
    return m_producer_name;
}
std::wstring Headphones::get_model_name() const
{
    return m_model_name;
}
std::wstring Headphones::get_price() const
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

void Headphones::set_producer_name(std::wstring producer_name)
{
    m_producer_name = producer_name;
}
void Headphones::set_model_name(std::wstring model_name)
{
    m_model_name = model_name;
}
void Headphones::set_price(std::wstring price)
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

std::wostream& operator<<(std::wostream& os, const Headphones& headphones)
{
    os << L"Список параметров наушников:" << L"\n";
    os << L"  Производитель: " << headphones.get_producer_name() << L"\n";
    os << L"  Название модели: " << headphones.get_model_name() << L"\n";
    os << L"  Цена: " << headphones.get_price() << L"\n";
    os << L"  Громкость: " << headphones.get_volume() << L"\n";
    os << L"  Шумоподавление: " << (headphones.is_noise_canceling_enabled() ? L"Вкл" : L"Выкл") << L"\n";
    os << L"  Микрофон: " << (headphones.is_microphone_enabled() ? L"Вкл" : L"Выкл") << L"\n";
    os << L"  Режим эквалайзера: " << equalizer_mode_to_string(headphones.get_equalizer_mode()) << L"\n";
    return os;
}
