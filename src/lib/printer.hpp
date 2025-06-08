#pragma once

#include <memory>
#include <string>

#include "informer/dpdk_informer.hpp"

namespace dpdk::network {

// Структура для информации о базовом интерфейсе
struct InterfaceInfo {
    std::string interface{};
    std::string driver{};
    std::string mac{};
    uint16_t port_id{};
    std::string device{};
    int32_t numa_node{-1};
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(InterfaceInfo, interface, driver, mac, port_id, device, numa_node);

// Структура для информации о состоянии соединения
struct LinkInfo {
    bool link_status{false};
    uint32_t link_speed{}; // Мбит/с
    std::string link_duplex{};
    bool link_autoneg{false};
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LinkInfo, link_status, link_speed, link_duplex, link_autoneg);

// Структура для статистики очереди
struct QueueStats {
    uint16_t queue_id{};
    uint64_t rx_packets{};
    uint64_t tx_packets{};
    uint64_t rx_bytes{};
    uint64_t tx_bytes{};
    uint64_t rx_errors{};
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(QueueStats, queue_id, rx_packets, tx_packets, rx_bytes, tx_bytes, rx_errors);

// Структура для базовой статистики порта
struct PortStats {
    uint64_t rx_packets{};
    uint64_t tx_packets{};
    uint64_t rx_bytes{};
    uint64_t tx_bytes{};
    uint64_t rx_missed{};
    uint64_t rx_errors{};
    uint64_t tx_errors{};
    uint64_t rx_no_mbuf{};
    std::vector<QueueStats> queue_stats{};
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PortStats, rx_packets, tx_packets, rx_bytes, tx_bytes, rx_missed, rx_errors, tx_errors, rx_no_mbuf, queue_stats);

// Структура для информации о возможностях порта
struct PortCapabilities {
    uint64_t rx_offload_capa{};
    uint64_t tx_offload_capa{};
    uint64_t rx_offload_conf{};
    uint64_t tx_offload_conf{};
    uint16_t max_rx_queues{};
    uint16_t max_tx_queues{};
    uint16_t max_rx_desc{};
    uint16_t max_tx_desc{};
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PortCapabilities, rx_offload_capa, tx_offload_capa, rx_offload_conf, tx_offload_conf,
                                   max_rx_queues, max_tx_queues, max_rx_desc, max_tx_desc);

// Пара ключ-значение для расширенной статистики
struct XStatItem {
    std::string name{};
    uint64_t value{};
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(XStatItem, name, value);

// Главная структура с полной информацией об интерфейсе
struct DetailedInterfaceInfo {
    InterfaceInfo basic_info{};
    LinkInfo link{};
    PortStats stats{};
    PortCapabilities capabilities{};
    std::vector<XStatItem> xstats{};
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DetailedInterfaceInfo, basic_info, link, stats, capabilities, xstats);


namespace exceptions {

struct DpdkEx : std::runtime_error {
    using std::runtime_error::runtime_error;
};
struct SetupEnvironment final : DpdkEx {
    using DpdkEx::DpdkEx;
};
struct PortNotFind final : DpdkEx {
    using DpdkEx::DpdkEx;
};
} // namespace exceptions

class ShowInfoInterface final : public InformerDpdk {
   public:
    ShowInfoInterface();
    ~ShowInfoInterface() override = default;

    ShowInfoInterface(ShowInfoInterface const &) = delete;
    ShowInfoInterface(ShowInfoInterface &&) = delete;
    ShowInfoInterface &operator=(ShowInfoInterface const &) = delete;
    ShowInfoInterface &operator=(ShowInfoInterface &&) = delete;

    nlohmann::json get_all_interface() override;
    ::nlohmann::json get_interface_info(std::string const &interface_name) override;

   private:
    void init() const;

    // Вспомогательные методы для получения информации
    InterfaceInfo get_basic_info(uint16_t port_id) const;
    LinkInfo get_link_info(uint16_t port_id) const;
    PortStats get_port_stats(uint16_t port_id) const;
    PortCapabilities get_port_capabilities(uint16_t port_id) const;
    std::vector<XStatItem> get_xstats(uint16_t port_id) const;
    DetailedInterfaceInfo get_detailed_info(uint16_t port_id) const;

    static constexpr std::array<char const *, 6> M_DRIVERS{"librte_net_i40e.so.24",    "librte_net_ixgbe.so.24", "librte_net_e1000.so.24",
                                                           "librte_net_vmxnet3.so.24", "librte_net_ice.so.24",   "librte_net_tap.so.24.0"}; // 48
    static constexpr auto M_DPDK_CORE_STATIC_NAME = "dpdk_core";                                                                            // 8
    static constexpr auto M_MEMPOOL_RING_DRIVER = "librte_mempool_ring.so.24";                                                              // 8
    static constexpr auto M_PCAP_DRIVER = "librte_net_pcap.so.24";                                                                          // 8
    uint32_t const m_main_core = 0;                                                                                                         // 4
};

} // namespace dpdk::network
