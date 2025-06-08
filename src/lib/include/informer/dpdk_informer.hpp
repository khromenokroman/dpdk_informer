#pragma once
#include <fmt/format.h>

#include <nlohmann/json.hpp>

namespace dpdk::network {

/**
 * @class InformerDpdk
 * @brief Абстрактный класс для получения информации об интерфейсах DPDK
 *
 * Этот класс предоставляет интерфейс для получения информации о сетевых
 * интерфейсах, управляемых DPDK. Он позволяет получить список всех доступных
 * интерфейсов и детальную информацию о конкретном интерфейсе.
 */
class InformerDpdk {
   public:
    InformerDpdk() = default;
    virtual ~InformerDpdk() = default;

    InformerDpdk(InformerDpdk const &) = delete;
    InformerDpdk(InformerDpdk &&) = delete;
    InformerDpdk &operator=(InformerDpdk const &) = delete;
    InformerDpdk &operator=(InformerDpdk &&) = delete;

    /**
     * @brief Получает список всех доступных интерфейсов DPDK
     *
     * @return JSON-массив, содержащий имена всех доступных интерфейсов DPDK
     * @throw exceptions::PortNotFind Если не найдено ни одного интерфейса
     * @throw std::runtime_error Если произошла ошибка при инициализации DPDK
     */
    virtual nlohmann::json get_all_interface() = 0;
    /**
     * @brief Получает детальную информацию о конкретном интерфейсе DPDK
     *
     * @param interface_name Имя интерфейса или его ID в виде строки
     * @return JSON-объект, содержащий детальную информацию об интерфейсе
     * @throw exceptions::PortNotFind Если указанный интерфейс не найден
     * @throw exceptions::DpdkEx Если произошла ошибка при получении информации
     * @throw std::runtime_error Если произошла ошибка при инициализации DPDK
     */
    virtual ::nlohmann::json get_interface_info(std::string const &interface_name) = 0;
    /**
     * @brief Создает экземпляр класса, реализующего интерфейс InformerDpdk
     *
     * Фабричный метод, который создает и возвращает экземпляр конкретной реализации
     * интерфейса InformerDpdk. Это единственный способ создания объектов данного класса.
     *
     * @return std::unique_ptr<InformerDpdk> Умный указатель на созданный объект
     * @throw std::runtime_error Если не удалось создать объект
     */
    static std::unique_ptr<InformerDpdk> create();

   private:
    /**
     * @brief Внутренний метод для создания объекта с обработкой ошибок
     *
     * @param error_message Буфер для записи сообщения об ошибке в случае неудачи
     * @return InformerDpdk* Указатель на созданный объект или nullptr в случае ошибки
     */
    static InformerDpdk *create(char *error_message) noexcept;
    /**
     * @brief Максимальный размер буфера для сообщений об ошибках.
     */
    static constexpr std::size_t M_MAX_BUFFER_SIZE = 1024;
};
/**
 * @brief Реализация статического метода create() для создания экземпляра InformerDpdk
 *
 * @return std::unique_ptr<InformerDpdk> Умный указатель на созданный объект
 * @throw std::runtime_error Если не удалось создать объект
 */
inline std::unique_ptr<InformerDpdk> InformerDpdk::create() {
    auto const message_error = std::make_unique<char[]>(M_MAX_BUFFER_SIZE);

    InformerDpdk *new_object = create(message_error.get());
    if (new_object == nullptr) {
        throw std::runtime_error(message_error.get());
    }

    return std::unique_ptr<InformerDpdk>{new_object};
}

} // namespace dpdk::network
