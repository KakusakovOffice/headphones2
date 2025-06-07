#include "TextMenu.hpp"
#include "HeadphonesList.hpp"
#include "fstream"
#include "sstream"
#include "cctype"
#include "cassert"
#include "cstdlib"

#include <codecvt>
#include <locale>
#include <windows.h>

int get_input_digit(int max_inclusive)
{
    std::string buffer;
    while (true)
    {
        std::cout << "Пожалуйста введите число от 1 до " << max_inclusive << ": " << std::flush;
        std::getline(std::cin, buffer);
        if (buffer.empty())
        {
            continue;
        }
        char ch = buffer[0];
        if (!std::isdigit(ch))
        {
            continue;
        }
        int num = ch - '0';
        if (num == 0 || num > max_inclusive)
        {
            continue;
        }
        return num;
    }
}

std::string слава_сатане()
{
    const DWORD length = 100;
    wchar_t wmsg[length];
    DWORD used;
    if (!ReadConsole(
            GetStdHandle(STD_INPUT_HANDLE),
            wmsg,
            length - 1,
            &used,
            nullptr
        )
    )
    {
        return "INTERFACING [Impossible: Success] - I'm afraid there is truly noting we can do to make the utf-8 output work, sir.";
    }

    std::wstring wstr(wmsg);
    wstr.pop_back();
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(wstr);
}

void generate_new_entry(HeadphonesList::Node& node)
{
    auto& value = node.value();
    std::string buffer;
    double volume_tmp;
    while (true)
    {
        std::cout
            << "\n"
            << "[Это конструктор записи]\n"
            << value
            << "Какое поле изменить:\n"
            << "  1) Производитель.\n"
            << "  2) Название модели.\n"
            << "  3) Цена.\n"
            << "  4) Громкость.\n"
            << "  5) Шумоподавление (Переключить).\n"
            << "  6) Микрофон (Переключить).\n"
            << "  7) Режим эквалайзера.\n"
            << "  8) Готово.\n"
            << std::flush;
        switch (get_input_digit(8))
        {
        case 1:
            std::cout << "Введите название производителя: " << std::flush;
            // std::getline(std::cin, buffer);
            buffer = слава_сатане();
            value.set_producer_name(buffer);
            break;
        case 2:
            std::cout << "Введите название модели: " << std::flush;
            // std::getline(std::cin, buffer);
            buffer = слава_сатане();
            value.set_model_name(buffer);
            break;
        case 3:
            std::cout << "Введите цену: " << std::flush;
            // std::getline(std::cin, buffer);
            buffer = слава_сатане();
            value.set_price(buffer);
            break;
        case 4:
            std::cout << "Введите громкость: " << std::flush;
            std::getline(std::cin, buffer);

            try
            {
                volume_tmp = std::stod(buffer);
            }
            catch (std::invalid_argument& e)
            {
                std::cout << "Ошибка: громкость должна быть числом.\n";
                break;
            }
            catch (std::out_of_range& e)
            {
                std::cout << "Ошибка: слишком большое (или слишком малое) число.\n";
                break;
            }
            value.set_volume(volume_tmp);
            break;
        case 5:
            value.toggle_noise_canceling();
            break;
        case 6:
            value.toggle_microphone();
            break;
        case 7:
            std::cout
                << "Выберите режим эквалайзера:\n"
                << "  1) " << equalizer_mode_to_string(EqualizerMode::Normal) << ".\n"
                << "  2) " << equalizer_mode_to_string(EqualizerMode::Bass) << ".\n"
                << "  3) " << equalizer_mode_to_string(EqualizerMode::Treble) << ".\n"
                << "  4) " << equalizer_mode_to_string(EqualizerMode::Vocal) << ".\n"
                << std::flush;
            switch (get_input_digit(4))
            {
            case 1:
                value.set_equalizer_mode(EqualizerMode::Normal);
                break;
            case 2:
                value.set_equalizer_mode(EqualizerMode::Bass);
                break;
            case 3:
                value.set_equalizer_mode(EqualizerMode::Treble);
                break;
            case 4:
                value.set_equalizer_mode(EqualizerMode::Vocal);
                break;
            default:
                assert(false);
            }
            break;
        case 8:
            return;
        default:
            assert(false);
        }
    }
}

void load_from_file(HeadphonesList& list, const std::string& filename)
{
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    if (!file.is_open())
    {
        std::cout
            << "Ошибка: не получается открыть файл.\n"
            << "Проверьте, что файл \"" << filename << "\" существует в папке из которой запущена программа.\n"
            << std::flush;
        return;
    }

    auto result = HeadphonesList::deserialize(file);
    if (std::holds_alternative<HeadphonesList::DeserializeError>(result))
    {
        auto error = std::get<HeadphonesList::DeserializeError>(result);
        std::cout
            << "Ошибка: \"" << error.message << "\".\n"
            << std::flush;
        return;
    }
    list = std::get<HeadphonesList>(result);
}

bool save_to_file(const HeadphonesList& list, const std::string& filename)
{
    std::ofstream file(filename, std::ios::out | std::ios::binary);
    if (!file.is_open())
    {
        std::cout
            << "Ошибка: не получается создать или переписать файл \"" << filename << "\" в текущей папке.\n"
            << std::flush;
        return false;
    }

    auto result = list.serialize(file);
    if (std::holds_alternative<HeadphonesList::SerializeError>(result))
    {
        auto error = std::get<HeadphonesList::SerializeError>(result);
        std::cout
            << "Ошибка: \"" << error.message << "\".\n"
            << std::flush;
        return false;
    }

    return true;
}

void display_list(const HeadphonesList& list)
{
    if (list.is_empty())
    {
        std::cout << "Список пуст.\n";
        return;
    }
    std::cout << "Элементы списка по порядку:\n";
    std::size_t index = 1;
    for (auto iter = list.chead();;iter++)
    {
        const auto& value = (*iter)->cvalue();
        std::cout << index << ") " << value;

        if (iter == list.ctail())
        {
            break;
        }
        index++;
    }
    std::cout << std::flush;
}

void edit_list(HeadphonesList& list)
{
    std::stringstream buffer_ss;
    std::string buffer;
    while (true)
    {
        if (list.is_empty())
        {
            std::cout
                << "Список пуст. Добавить запись?\n"
                << "  1) Да.\n"
                << "  2) Назад.\n"
                << std::flush;
            HeadphonesList::Node::node_ptr node = std::make_shared<HeadphonesList::Node>();
            switch (get_input_digit(2))
            {
            case 1:
                generate_new_entry(*node);
                list.insert_after(list.head(), node);
                break;
            case 2:
                return;
            default:
                assert(false);
            }
            continue;
        }

        std::cout
            << "Редактирование списка. Найдите релевантную запись:\n"
            << "  1) По индексу.\n"
            << "  2) Назад.\n"
            << std::flush;
        std::uintptr_t index;

        auto node_iter = HeadphonesList::Iterator(nullptr);
        switch (get_input_digit(2))
        {
        case 1:
            while (true)
            {
                std::cout << "Введите индекс записи (число от 1 до " << list.count() << " включительно): " << std::flush;
                std::getline(std::cin, buffer);
                buffer_ss.clear();
                buffer_ss << buffer;
                if (!(buffer_ss >> index))
                {
                    std::cout << "Ошибка: индекс записи должен быть целым числом.\n";
                    continue;
                }
                if (index == 0)
                {
                    std::cout << "Ошибка: индексы записей начинаются с единицы а не с нуля.\n";
                    continue;
                }
                auto iter = list.index(index - 1);
                if (!(*iter))
                {
                    std::cout << "Ошибка: индекс превышает количество записей.\n";
                    continue;
                }
                node_iter = *iter;
                break;
            }
            break;
        case 2:
            return;
        default:
            assert(false);
        }

        auto added_node = std::make_shared<HeadphonesList::Node>();
        std::cout
            << "[Редактирование записи]\n"
            << (*node_iter)->value()
            << "Выберите действие:\n"
            << "  1) Редактировать выбранную запись.\n"
            << "  2) Добавить запись до выбранной.\n"
            << "  3) Добавить запись после выбранной.\n"
            << "  4) Удалить выбранную запись.\n"
            << "  5) Назад.\n"
            << std::flush;
        switch (get_input_digit(5))
        {
        case 1:
            generate_new_entry(**node_iter);
            break;
        case 2:
            generate_new_entry(*added_node);
            list.insert_before(node_iter, added_node);
            break;
        case 3:
            generate_new_entry(*added_node);
            list.insert_after(node_iter, added_node);
            break;
        case 4:
            list.remove(node_iter);
            break;
        case 5:
            continue;
        default:
            assert(false);
        }
    }
}

void display_info()
{
    std::cout
        << "Информация о программе:\n"
        << "ФИО = \"Сигаев Иван Романович\"\n"
        << "Курс = 2\n"
        << "Группа = 6\n"
        << "Дата компиляции = " << __DATE__ << "\n"
        << std::flush;
}

void exit_session(const HeadphonesList& list, const std::string& filename)
{
    std::cout
        << "Вы уверены, что хотите выйти?\n"
        << "1) Да.\n"
        << "2) Нет.\n"
        << std::flush;
    switch (get_input_digit(2))
    {
    case 1:
        break;
    case 2:
        return;
    default:
        assert(false);
    }

    std::cout
        << "Хотите сохранить файл перед выходом?\n"
        << "1) Да.\n"
        << "2) Нет.\n"
        << std::flush;
    switch (get_input_digit(2))
    {
    case 1:
        if (!save_to_file(list, filename))
        {
            return;
        }
        break;
    case 2:
        break;
    default:
        assert(false);
    }

    std::cout << "Пока-пока!" << std::endl;

    std::exit(EXIT_SUCCESS);
}

void TextMenu::session()
{
    const std::string save_filename = "headphones.bin";

    HeadphonesList list {};

    while (true)
    {
        std::cout
            << "\n"
            << "Выберите действие:\n"
            << "  1) Загрузить из файла.\n"
            << "  2) Сохранить в файл.\n"
            << "  3) Показать список.\n"
            << "  4) Редактировать список.\n"
            << "  5) О программе.\n"
            << "  6) Выход.\n"
            << std::flush;

        switch (get_input_digit(6))
        {
        case 1:
            load_from_file(list, save_filename);
            break;
        case 2:
            save_to_file(list, save_filename);
            break;
        case 3:
            display_list(list);
            break;
        case 4:
            edit_list(list);
            break;
        case 5:
            display_info();
            break;
        case 6:
            exit_session(list, save_filename);
            break;
        default:
            assert(false);
        }
    }
}
