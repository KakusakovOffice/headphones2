#pragma once
#include <string>
#include <iostream>
#include <optional>

enum class EqualizerMode {
    Normal,
    Bass,
    Treble,
    Vocal
};

std::string equalizer_mode_to_string(EqualizerMode equalizer_mode);
std::optional<EqualizerMode> equalizer_mode_from_string(std::string string);

class Headphones {
public:
    Headphones();
    Headphones(
        std::string producer_name,
        std::string model_name,
        std::string price,
        double volume,
        bool is_noise_canceling_enabled,
        bool is_microphone_enabled,
        EqualizerMode equalizer_mode
    );
    ~Headphones() = default;

    Headphones(const Headphones& headphones) = delete;
    Headphones& operator=(const Headphones& headphones) = delete;

    std::string get_producer_name() const;
    std::string get_model_name() const;
    std::string get_price() const;
    double get_volume() const;
    bool is_noise_canceling_enabled() const;
    bool is_microphone_enabled() const;
    EqualizerMode get_equalizer_mode() const;

    void set_producer_name(std::string producer_name);
    void set_model_name(std::string model_name);
    void set_price(std::string price);
    void set_volume(double volume);
    void toggle_noise_canceling();
    void toggle_microphone();
    void set_equalizer_mode(EqualizerMode equalizer_mode);
private:
    std::string m_producer_name;
    std::string m_model_name;
    std::string m_price;
    double m_volume;
    bool m_is_noise_canceling_enabled;
    bool m_is_microphone_enabled;
    EqualizerMode m_equalizer_mode;
};

std::ostream& operator<<(std::ostream& os, const Headphones& headphones);
