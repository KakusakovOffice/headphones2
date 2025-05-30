#pragma once
#include <string>
#include <iostream>

// Режим эквалайзера
enum EqualizerMode {
    Normal,
    Bass,
    Treble,
    Vocal
};

std::wstring equalizer_mode_to_string(EqualizerMode equalizer_mode);

class Headphones {
public:
    Headphones(
        std::wstring producer_name,
        std::wstring model_name,
        std::wstring price,
        double volume,
        bool is_noise_canceling_enabled,
        bool is_microphone_enabled,
        EqualizerMode equalizer_mode
    );
    ~Headphones() = default;

    Headphones(const Headphones& headphones) = delete;
    Headphones& operator=(const Headphones& headphones) = delete;

    std::wstring get_producer_name() const;
    std::wstring get_model_name() const;
    std::wstring get_price() const;
    double get_volume() const;
    bool is_noise_canceling_enabled() const;
    bool is_microphone_enabled() const;
    EqualizerMode get_equalizer_mode() const;

    void set_producer_name(std::wstring producer_name);
    void set_model_name(std::wstring model_name);
    void set_price(std::wstring price);
    void set_volume(double volume);
    void toggle_noise_canceling();
    void toggle_microphone();
    void set_equalizer_mode(EqualizerMode equalizer_mode);
private:
    std::wstring m_producer_name;
    std::wstring m_model_name;
    std::wstring m_price;
    double m_volume;
    bool m_is_noise_canceling_enabled;
    bool m_is_microphone_enabled;
    EqualizerMode m_equalizer_mode;
};

std::wostream& operator<<(std::wostream& os, const Headphones& headphones);
