/**
 * @file printer.hpp
 * @brief Реализация сбора и представления информации об интерфейсах DPDK
 *
 * Этот файл содержит классы и структуры данных, необходимые для
 * получения и представления информации о сетевых интерфейсах,
 * управляемых DPDK. Информация включает базовые данные интерфейса,
 * состояние соединения, статистику, возможности и другие параметры.
 */

#pragma once

#include <memory>
#include <string>

#include "informer/dpdk_informer.hpp"

namespace dpdk::network {

/**
 * @struct InterfaceInfo
 * @brief Структура для хранения базовой информации о сетевом интерфейсе
 */
struct InterfaceInfo {
    std::string interface{}; ///< Имя интерфейса
    std::string driver{};    ///< Имя драйвера
    std::string mac{};       ///< MAC-адрес в строковом формате
    uint16_t port_id{};      ///< Идентификатор порта DPDK
    std::string device{};    ///< Информация об устройстве
    int32_t numa_node{-1};   ///< NUMA-узел, к которому привязан интерфейс (-1 если неизвестно)
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(InterfaceInfo, interface, driver, mac, port_id, device, numa_node);

/**
 * @struct LinkInfo
 * @brief Структура для хранения информации о состоянии сетевого соединения
 */
struct LinkInfo {
    bool link_status{false};   ///< Статус соединения (true - активно, false - неактивно)
    uint32_t link_speed{};     ///< Скорость соединения в Мбит/с
    std::string link_duplex{}; ///< Режим дуплекса ("полный" или "половинный")
    bool link_autoneg{false};  ///< Включен ли автосогласование скорости
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LinkInfo, link_status, link_speed, link_duplex, link_autoneg);

/**
 * @struct QueueStats
 * @brief Структура для хранения статистики по очереди
 */
struct QueueStats {
    uint16_t queue_id{};   ///< Идентификатор очереди
    uint64_t rx_packets{}; ///< Количество принятых пакетов
    uint64_t tx_packets{}; ///< Количество отправленных пакетов
    uint64_t rx_bytes{};   ///< Количество принятых байт
    uint64_t tx_bytes{};   ///< Количество отправленных байт
    uint64_t rx_errors{};  ///< Количество ошибок при приеме
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(QueueStats, queue_id, rx_packets, tx_packets, rx_bytes, tx_bytes, rx_errors);

/**
 * @struct PortStats
 * @brief Структура для хранения базовой статистики порта
 */
struct PortStats {
    uint64_t rx_packets{};                 ///< Количество принятых пакетов
    uint64_t tx_packets{};                 ///< Количество отправленных пакетов
    uint64_t rx_bytes{};                   ///< Количество принятых байт
    uint64_t tx_bytes{};                   ///< Количество отправленных байт
    uint64_t rx_missed{};                  ///< Количество пропущенных пакетов при приеме
    uint64_t rx_errors{};                  ///< Количество ошибок при приеме
    uint64_t tx_errors{};                  ///< Количество ошибок при отправке
    uint64_t rx_no_mbuf{};                 ///< Количество пакетов, отброшенных из-за нехватки буферов mbuf
    std::vector<QueueStats> queue_stats{}; ///< Статистика по отдельным очередям
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PortStats, rx_packets, tx_packets, rx_bytes, tx_bytes, rx_missed, rx_errors, tx_errors, rx_no_mbuf, queue_stats);

/**
 * @struct PortCapabilities
 * @brief Структура для хранения информации о возможностях порта
 */
struct PortCapabilities {
    uint64_t rx_offload_capa{}; ///< Поддерживаемые аппаратные ускорения для приема
    uint64_t tx_offload_capa{}; ///< Поддерживаемые аппаратные ускорения для передачи
    uint64_t rx_offload_conf{}; ///< Настроенные аппаратные ускорения для приема
    uint64_t tx_offload_conf{}; ///< Настроенные аппаратные ускорения для передачи
    uint16_t max_rx_queues{};   ///< Максимальное количество очередей приема
    uint16_t max_tx_queues{};   ///< Максимальное количество очередей передачи
    uint16_t max_rx_desc{};     ///< Максимальное количество дескрипторов в очереди приема
    uint16_t max_tx_desc{};     ///< Максимальное количество дескрипторов в очереди передачи
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PortCapabilities, rx_offload_capa, tx_offload_capa, rx_offload_conf, tx_offload_conf, max_rx_queues, max_tx_queues,
                                   max_rx_desc, max_tx_desc);

/**
 * @struct XStatItem
 * @brief Структура для хранения элемента расширенной статистики
 */
struct XStatItem {
    std::string name{}; ///< Имя статистического параметра
    uint64_t value{};   ///< Значение статистического параметра
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(XStatItem, name, value);

/**
 * @struct DetailedInterfaceInfo
 * @brief Главная структура с полной информацией об интерфейсе
 */
struct DetailedInterfaceInfo {
    InterfaceInfo basic_info{};      ///< Базовая информация об интерфейсе
    LinkInfo link{};                 ///< Информация о соединении
    PortStats stats{};               ///< Статистика порта
    PortCapabilities capabilities{}; ///< Возможности порта
    std::vector<XStatItem> xstats{}; ///< Расширенная статистика
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DetailedInterfaceInfo, basic_info, link, stats, capabilities, xstats);

namespace exceptions {

/**
 * @struct DpdkEx
 * @brief Базовый класс исключений для DPDK операций
 */
struct DpdkEx : std::runtime_error {
    using std::runtime_error::runtime_error;
};

/**
 * @struct SetupEnvironment
 * @brief Исключение, возникающее при проблемах с настройкой окружения
 */
struct SetupEnvironment final : DpdkEx {
    using DpdkEx::DpdkEx;
};

/**
 * @struct PortNotFind
 * @brief Исключение, возникающее когда запрашиваемый порт не найден
 */
struct PortNotFind final : DpdkEx {
    using DpdkEx::DpdkEx;
};

} // namespace exceptions

/**
 * @class ShowInfoInterface
 * @brief Класс для получения и представления информации об интерфейсах DPDK
 *
 * Этот класс реализует интерфейс InformerDpdk и предоставляет методы для
 * получения списка всех доступных интерфейсов DPDK и детальной информации
 * о конкретном интерфейсе.
 */
class ShowInfoInterface final : public InformerDpdk {
   public:
    /**
     * @brief Конструктор
     *
     * Инициализирует окружение DPDK и подготавливает класс к работе.
     * @throw exceptions::SetupEnvironment При ошибке настройки окружения
     * @throw std::runtime_error При ошибке инициализации DPDK
     */
    ShowInfoInterface();
    ~ShowInfoInterface() override = default;

    ShowInfoInterface(ShowInfoInterface const &) = delete;
    ShowInfoInterface(ShowInfoInterface &&) = delete;
    ShowInfoInterface &operator=(ShowInfoInterface const &) = delete;
    ShowInfoInterface &operator=(ShowInfoInterface &&) = delete;

    /**
     * @brief Получает список всех доступных интерфейсов DPDK
     *
     * @return JSON-массив с именами всех доступных интерфейсов
     * @throw exceptions::PortNotFind Если не найдено ни одного интерфейса
     */
    nlohmann::json get_all_interface() override;
    /**
     * @brief Получает детальную информацию о конкретном интерфейсе
     *
     * @param interface_name Имя интерфейса или его идентификатор
     * @return JSON-объект с детальной информацией об интерфейсе
     * @throw exceptions::PortNotFind Если интерфейс не найден
     */
    ::nlohmann::json get_interface_info(std::string const &interface_name) override;

   private:
    /**
     * @brief Инициализирует окружение DPDK
     *
     * @throw std::runtime_error При ошибке инициализации DPDK
     */
    void init() const;

    /**
     * @brief Получает базовую информацию об интерфейсе
     *
     * @param port_id Идентификатор порта
     * @return Структура InterfaceInfo с базовой информацией
     */
    [[nodiscard]] static InterfaceInfo get_basic_info(uint16_t port_id);
    /**
     * @brief Получает информацию о состоянии соединения
     *
     * @param port_id Идентификатор порта
     * @return Структура LinkInfo с информацией о соединении
     */
    [[nodiscard]] static LinkInfo get_link_info(uint16_t port_id);
    /**
     * @brief Получает статистику порта
     *
     * @param port_id Идентификатор порта
     * @return Структура PortStats со статистикой порта
     */
    [[nodiscard]] static PortStats get_port_stats(uint16_t port_id);
    /**
     * @brief Получает информацию о возможностях порта
     *
     * @param port_id Идентификатор порта
     * @return Структура PortCapabilities с информацией о возможностях
     */
    [[nodiscard]] static PortCapabilities get_port_capabilities(uint16_t port_id);
    /**
     * @brief Получает расширенную статистику порта
     *
     * @param port_id Идентификатор порта
     * @return Вектор XStatItem с элементами расширенной статистики
     */
    [[nodiscard]] static std::vector<XStatItem> get_xstats(uint16_t port_id);
    /**
     * @brief Получает полную детальную информацию об интерфейсе
     *
     * @param port_id Идентификатор порта
     * @return Структура DetailedInterfaceInfo с полной информацией
     */
    [[nodiscard]] static DetailedInterfaceInfo get_detailed_info(uint16_t port_id);

    /** @brief Массив строк с именами драйверов DPDK для загрузки */
    static constexpr std::array<char const *, 6> M_DRIVERS{"librte_net_i40e.so.24",    "librte_net_ixgbe.so.24", "librte_net_e1000.so.24",
                                                           "librte_net_vmxnet3.so.24", "librte_net_ice.so.24",   "librte_net_tap.so.24.0"};
    /** @brief Префикс имени файлов для DPDK */
    static constexpr auto M_DPDK_CORE_STATIC_NAME = "dpdk_core";
    /** @brief Имя драйвера кольцевого пула памяти */
    static constexpr auto M_MEMPOOL_RING_DRIVER = "librte_mempool_ring.so.24";
    /** @brief Имя драйвера PCAP */
    static constexpr auto M_PCAP_DRIVER = "librte_net_pcap.so.24";
    /** @brief Основное ядро, используемое для DPDK */
    uint32_t const m_main_core = 0;
};

} // namespace dpdk::network
