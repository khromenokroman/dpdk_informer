#include "printer.hpp"

#include <rte_eal.h>
#include <rte_ethdev.h>
#include <rte_version.h>

#include <iostream>

namespace dpdk::network {

InformerDpdk *InformerDpdk::create(char *error_message) noexcept {
    try {
        auto *new_object = new ShowInfoInterface();
        return new_object;
    } catch (std::exception const &ex) {
        ::snprintf(error_message, M_MAX_BUFFER_SIZE, "Cannot create new object %s", ex.what());
        return nullptr;
    }
}

ShowInfoInterface::ShowInfoInterface() {
    if (getenv("SYSTEMD_EXEC_PID") == nullptr) {
        // if it is running not by systemd
        if (auto const env_res = setenv("RUNTIME_DIRECTORY", "/tmp", 1); env_res < 0) {
            throw exceptions::SetupEnvironment(::fmt::format("Can't setup environment variable 'RUNTIME_DIRECTORY' {}: {}", errno, strerror(errno)));
        }
    }

    init();
}
nlohmann::json ShowInfoInterface::get_all_interface() {
    nlohmann::json result = nlohmann::json::array();
    uint16_t const nb_ports = rte_eth_dev_count_avail();

    if (nb_ports == 0) {
        throw exceptions::PortNotFind("Не найдено портов Ethernet, управляемых DPDK");
    }

    for (uint16_t port_id = 0; port_id < nb_ports; port_id++) {
        char dev_name[RTE_ETH_NAME_MAX_LEN];
        if (rte_eth_dev_get_name_by_port(port_id, dev_name) != 0) {
            std::strcpy(dev_name, "unknown");
        }
        result.emplace_back(dev_name);
    }

    return result;
}
nlohmann::json ShowInfoInterface::get_interface_info(std::string const &interface_name) {
    try {
        uint16_t const nb_ports = rte_eth_dev_count_avail();
        if (nb_ports == 0) {
            throw exceptions::PortNotFind("Не найдено портов Ethernet, управляемых DPDK");
        }

        // Проверяем, является ли входное значение числом (ID порта)
        uint16_t port_id_from_name = RTE_MAX_ETHPORTS;
        bool is_port_id = false;

        try {
            port_id_from_name = std::stoi(interface_name);
            is_port_id = (port_id_from_name < nb_ports);
        } catch (...) {
            // Не является числом, продолжаем поиск по имени
        }

        // Если передан корректный ID порта, используем его напрямую
        if (is_port_id) {
            return get_detailed_info(port_id_from_name);
        }

        // Иначе ищем порт по имени
        for (uint16_t port_id = 0; port_id < nb_ports; port_id++) {
            char dev_name[RTE_ETH_NAME_MAX_LEN];
            if (rte_eth_dev_get_name_by_port(port_id, dev_name) != 0) {
                continue;
            }

            if (interface_name == dev_name) {
                return get_detailed_info(port_id);
            }
        }

        // Если интерфейс не найден
        throw exceptions::PortNotFind(::fmt::format("Интерфейс '{}' не найден среди DPDK-портов", interface_name));

    } catch (const exceptions::DpdkEx& ex) {
        // Перебрасываем исключения DPDK дальше
        throw;
    } catch (const std::exception& ex) {
        // Преобразуем стандартные исключения в DpdkEx
        throw exceptions::DpdkEx(std::string("Ошибка при получении информации об интерфейсе: ") + ex.what());
    }
}
void ShowInfoInterface::init() const {
    std::vector<std::string> eal_params;

    eal_params.emplace_back("./app");

    eal_params.emplace_back("-l");
    eal_params.emplace_back(std::to_string(m_main_core));

    eal_params.emplace_back("-d");
    eal_params.emplace_back(M_MEMPOOL_RING_DRIVER);

    for (auto one_driver : M_DRIVERS) {
        eal_params.emplace_back("-d");
        eal_params.emplace_back(one_driver);
    }

    eal_params.emplace_back("-d");
    eal_params.emplace_back(M_PCAP_DRIVER);

    eal_params.emplace_back("--file-prefix");
    eal_params.emplace_back(M_DPDK_CORE_STATIC_NAME);
    eal_params.emplace_back("--proc-type=secondary");

    std::vector<char *> eal_pntrs;
    for (auto const &one_param : eal_params) {
        std::cout << "\t" << one_param << std::endl;
        eal_pntrs.push_back(const_cast<char *>(one_param.data()));
    }

    {
        auto const init_res = rte_eal_init(static_cast<int>(eal_pntrs.size()), eal_pntrs.data());
        auto const tmp_errno = rte_errno;
        if (init_res < 0) {
            throw std::runtime_error(rte_strerror(tmp_errno));
        }
    }

    if (rte_eal_primary_proc_alive(nullptr) != 1) {
        throw std::runtime_error("EAL master process is dead");
    }
}
InterfaceInfo ShowInfoInterface::get_basic_info(uint16_t port_id) const {
    InterfaceInfo info;
    info.port_id = port_id;

    // Получаем имя интерфейса
    char dev_name[RTE_ETH_NAME_MAX_LEN];
    if (rte_eth_dev_get_name_by_port(port_id, dev_name) == 0) {
        info.interface = dev_name;
    } else {
        info.interface = "port_" + std::to_string(port_id);
    }

    // Получаем информацию о драйвере и устройстве
    struct rte_eth_dev_info dev_info;
    if (rte_eth_dev_info_get(port_id, &dev_info) == 0) {
        info.driver = dev_info.driver_name ? dev_info.driver_name : "неизвестен";

        // Вместо прямого доступа к полям rte_device, будем использовать только доступную информацию
        if (dev_info.device) {
            // Не будем обращаться к полям неполного типа
            info.device = "PCI устройство"; // Общее название без деталей
            // info.numa_node = dev_info.numa_node; // Используем numa_node из dev_info
        }
    } else {
        info.driver = "неизвестен";
    }

    // Получаем MAC-адрес
    struct rte_ether_addr mac_addr;
    if (rte_eth_macaddr_get(port_id, &mac_addr) == 0) {
        char mac_str[18];
        snprintf(mac_str, sizeof(mac_str), "%02X:%02X:%02X:%02X:%02X:%02X",
                 mac_addr.addr_bytes[0], mac_addr.addr_bytes[1],
                 mac_addr.addr_bytes[2], mac_addr.addr_bytes[3],
                 mac_addr.addr_bytes[4], mac_addr.addr_bytes[5]);
        info.mac = mac_str;
    } else {
        info.mac = "неизвестен";
    }

    return info;
}
LinkInfo ShowInfoInterface::get_link_info(uint16_t port_id) const {
    LinkInfo info;
    struct rte_eth_link link;

    if (rte_eth_link_get_nowait(port_id, &link) == 0) {
        info.link_status = link.link_status;
        info.link_speed = link.link_speed;
        info.link_duplex = link.link_duplex == RTE_ETH_LINK_FULL_DUPLEX ? "полный" : "половинный";
        info.link_autoneg = link.link_autoneg == RTE_ETH_LINK_AUTONEG;
    }

    return info;

}
PortStats ShowInfoInterface::get_port_stats(uint16_t port_id) const {
    PortStats stats;
    struct rte_eth_stats eth_stats;

    if (rte_eth_stats_get(port_id, &eth_stats) == 0) {
        stats.rx_packets = eth_stats.ipackets;
        stats.tx_packets = eth_stats.opackets;
        stats.rx_bytes = eth_stats.ibytes;
        stats.tx_bytes = eth_stats.obytes;
        stats.rx_missed = eth_stats.imissed;
        stats.rx_errors = eth_stats.ierrors;
        stats.tx_errors = eth_stats.oerrors;
        stats.rx_no_mbuf = eth_stats.rx_nombuf;

        // Получаем статистику по очередям
        for (int q = 0; q < RTE_ETHDEV_QUEUE_STAT_CNTRS; q++) {
            if (eth_stats.q_ipackets[q] || eth_stats.q_opackets[q]) {
                QueueStats q_stats;
                q_stats.queue_id = q;
                q_stats.rx_packets = eth_stats.q_ipackets[q];
                q_stats.tx_packets = eth_stats.q_opackets[q];
                q_stats.rx_bytes = eth_stats.q_ibytes[q];
                q_stats.tx_bytes = eth_stats.q_obytes[q];
                q_stats.rx_errors = eth_stats.q_errors[q];

                stats.queue_stats.push_back(q_stats);
            }
        }
    }

    return stats;
}
PortCapabilities ShowInfoInterface::get_port_capabilities(uint16_t port_id) const {
    PortCapabilities caps;
    struct rte_eth_dev_info dev_info;

    if (rte_eth_dev_info_get(port_id, &dev_info) == 0) {
        caps.rx_offload_capa = dev_info.rx_offload_capa;
        caps.tx_offload_capa = dev_info.tx_offload_capa;
        caps.rx_offload_conf = dev_info.default_rxconf.offloads;
        caps.tx_offload_conf = dev_info.default_txconf.offloads;
        caps.max_rx_queues = dev_info.max_rx_queues;
        caps.max_tx_queues = dev_info.max_tx_queues;
        caps.max_rx_desc = dev_info.rx_desc_lim.nb_max;
        caps.max_tx_desc = dev_info.tx_desc_lim.nb_max;
    }

    return caps;
}
std::vector<XStatItem> ShowInfoInterface::get_xstats(uint16_t port_id) const {
    std::vector<XStatItem> xstats_vec;

    // Получаем количество статистик
    int num_xstats = rte_eth_xstats_get(port_id, NULL, 0);
    if (num_xstats <= 0) {
        return xstats_vec;
    }

    // Выделяем память для статистик и их имен
    std::vector<struct rte_eth_xstat> xstats(num_xstats);
    std::vector<struct rte_eth_xstat_name> xstat_names(num_xstats);

    // Получаем статистики и их имена
    if (rte_eth_xstats_get(port_id, xstats.data(), num_xstats) != num_xstats ||
        rte_eth_xstats_get_names(port_id, xstat_names.data(), num_xstats) != num_xstats) {
        return xstats_vec;
    }

    // Заполняем вектор статистик
    for (int i = 0; i < num_xstats; i++) {
        XStatItem item;
        item.name = xstat_names[xstats[i].id].name;
        item.value = xstats[i].value;
        xstats_vec.push_back(item);
    }

    return xstats_vec;
}
DetailedInterfaceInfo ShowInfoInterface::get_detailed_info(uint16_t port_id) const {
    DetailedInterfaceInfo detailed_info;

    detailed_info.basic_info = get_basic_info(port_id);
    detailed_info.link = get_link_info(port_id);
    detailed_info.stats = get_port_stats(port_id);
    detailed_info.capabilities = get_port_capabilities(port_id);
    detailed_info.xstats = get_xstats(port_id);

    return detailed_info;
}
} // namespace dpdk::network
