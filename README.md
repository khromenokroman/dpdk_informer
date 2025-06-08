# DPDK Informer

### Описание
Библиотека `dpdk_informer` предоставляет программный интерфейс (API) для получения подробной информации о сетевых 
интерфейсах, управляемых фреймворком DPDK (Data Plane Development Kit) в операционной системе Linux. 
Библиотека взаимодействует с инфраструктурой DPDK для сбора детальных данных о конфигурации и 
состоянии сетевых интерфейсов в форматированном JSON представлении. 

### Возможности
- **Получение структурированной информации о DPDK-интерфейсах**:
    - Базовая информация об интерфейсе (идентификатор порта, имя, драйвер)
    - Аппаратные свойства (MAC-адрес, привязка к NUMA-узлу)
    - Оперативный статус соединения (состояние линка, скорость, режим дуплекса, автосогласование)
    - **Детальная статистика работы интерфейса**:
        - Статистика RX/TX (принятые/отправленные пакеты и байты)
        - Информация об ошибках и пропущенных пакетах
        - Статистика по отдельным очередям приема/передачи
        - Расширенная статистика xstats с детальными счетчиками

- **Информация о возможностях интерфейса**:
    - Поддерживаемые аппаратные ускорения (offloads) для приема и передачи
    - Текущие настройки аппаратных ускорений
    - Максимальные параметры интерфейса (количество очередей, дескрипторов)

- **Программный интерфейс**:
    - Получение списка всех доступных DPDK-интерфейсов
    - Получение полной детализированной информации о конкретном интерфейсе
    - Данные представлены в удобном для обработки формате JSON
    - Исключения для обработки ошибок с информативными сообщениями
    - Реализация с использованием современных возможностей C++20

### Зависимости
- C++20
- DPDK (Data Plane Development Kit)
- fmt
- nlohmann/json

### Опции сборки
Проект предоставляет следующие опции сборки через CMake:
- - выбор типа библиотеки для сборки: **BUILD_SHARED_LIBS**
    - `ON` (по умолчанию) - сборка динамической библиотеки (`.so`)
    - `OFF` - сборка статической библиотеки (`.a`)

- **BUILD_EXAMPLE** - включение/отключение сборки примера:
    - `ON` (по умолчанию) - собирать пример использования библиотеки
    - `OFF` - не собирать пример

### Сборка
Проект использует CMake для сборки:
````bash
mkdir build
cd build
cmake ..
make
````
#### Сборка только статической библиотеки
````bash
mkdir build
cd build
cmake -DBUILD_SHARED_LIBS=OFF ..
make
````
#### Сборка только статической библиотеки без примера
``` bash
mkdir build
cd build
cmake -DBUILD_SHARED_LIBS=OFF -DBUILD_EXAMPLE=OFF ..
make
```
#### Для создания пакета DEB:
``` bash
cd build
make package
```
### Установка
После сборки можно установить библиотеку:
``` bash
sudo make install
```
Или установить созданный DEB-пакет:
``` bash
sudo dpkg -i libdpdk_informer0*.deb
```
При сборке динамической библиотеки создаются два пакета:
- `libdpdk_informer0` - содержит саму динамическую библиотеку
- `libdpdk_informer0-dev` - содержит заголовочные файлы и символьные ссылки для разработки

При сборке статической библиотеки создается только один пакет:
- `libdpdk_informer0-dev` - содержит статическую библиотеку и заголовочные файлы

### Пример использующего кода
``` cpp
#include <informer/dpdk_informer.hpp>
#include <iostream>

int main() {
    try {
        auto const connect = ::dpdk::network::InformerDpdk::create();
        std::cout << "All interface:\n";
        auto const all_interface = connect->get_all_interface();
        std::cout << all_interface.dump(4) << std::endl << std::endl;

        std::cout << "Show info for 0000:98:00.0:\n";
        auto const interface = connect->get_interface_info("0000:98:00.0");
        std::cout << interface.dump(4) << std::endl << std::endl;

        return 0;
    } catch (std::exception const &ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }
}
```

### Пример вывода программы
````
All interface:
[
    "0000:98:00.0",
    "0000:98:00.1",
    "net_tap0",
    "net_tap1",
    "net_tap2",
    "net_tap3"
]

Show info for 0000:98:00.0:
{
    "basic_info": {
        "device": "PCI устройство",
        "driver": "net_ice",
        "interface": "0000:98:00.0",
        "mac": "1C:FD:08:7E:C6:32",
        "numa_node": -1,
        "port_id": 0
    },
    "capabilities": {
        "max_rx_desc": 4096,
        "max_rx_queues": 256,
        "max_tx_desc": 4096,
        "max_tx_queues": 256,
        "rx_offload_capa": 1664623,
        "rx_offload_conf": 0,
        "tx_offload_capa": 1155007,
        "tx_offload_conf": 0
    },
    "link": {
        "link_autoneg": true,
        "link_duplex": "полный",
        "link_speed": 100000,
        "link_status": true
    },
    "stats": {
        "queue_stats": [],
        "rx_bytes": 1592660904861,
        "rx_errors": 0,
        "rx_missed": 3339358633,
        "rx_no_mbuf": 0,
        "rx_packets": 65375,
        "tx_bytes": 0,
        "tx_errors": 0,
        "tx_packets": 0
    },
    "xstats": [
        {
            "name": "rx_good_packets",
            "value": 65408
        },
        {
            "name": "tx_good_packets",
            "value": 0
        },
        {
            "name": "rx_good_bytes",
            "value": 1592660973021
        },
        {
            "name": "tx_good_bytes",
            "value": 0
        },
        {
            "name": "rx_missed_errors",
            "value": 3339358737
        },
        {
            "name": "rx_errors",
            "value": 0
        },
        {
            "name": "tx_errors",
            "value": 0
        },
        {
            "name": "rx_mbuf_allocation_errors",
            "value": 0
        },
        {
            "name": "rx_q0_packets",
            "value": 0
        },
        {
            "name": "rx_q0_bytes",
            "value": 0
        },
        {
            "name": "rx_q0_errors",
            "value": 0
        },
        {
            "name": "rx_q1_packets",
            "value": 0
        },
        {
            "name": "rx_q1_bytes",
            "value": 0
        },
        {
            "name": "rx_q1_errors",
            "value": 0
        },
        {
            "name": "rx_q2_packets",
            "value": 0
        },
        {
            "name": "rx_q2_bytes",
            "value": 0
        },
        {
            "name": "rx_q2_errors",
            "value": 0
        },
        {
            "name": "rx_q3_packets",
            "value": 0
        },
        {
            "name": "rx_q3_bytes",
            "value": 0
        },
        {
            "name": "rx_q3_errors",
            "value": 0
        },
        {
            "name": "rx_q4_packets",
            "value": 0
        },
        {
            "name": "rx_q4_bytes",
            "value": 0
        },
        {
            "name": "rx_q4_errors",
            "value": 0
        },
        {
            "name": "rx_q5_packets",
            "value": 0
        },
        {
            "name": "rx_q5_bytes",
            "value": 0
        },
        {
            "name": "rx_q5_errors",
            "value": 0
        },
        {
            "name": "rx_q6_packets",
            "value": 0
        },
        {
            "name": "rx_q6_bytes",
            "value": 0
        },
        {
            "name": "rx_q6_errors",
            "value": 0
        },
        {
            "name": "rx_q7_packets",
            "value": 0
        },
        {
            "name": "rx_q7_bytes",
            "value": 0
        },
        {
            "name": "rx_q7_errors",
            "value": 0
        },
        {
            "name": "rx_q8_packets",
            "value": 0
        },
        {
            "name": "rx_q8_bytes",
            "value": 0
        },
        {
            "name": "rx_q8_errors",
            "value": 0
        },
        {
            "name": "rx_q9_packets",
            "value": 0
        },
        {
            "name": "rx_q9_bytes",
            "value": 0
        },
        {
            "name": "rx_q9_errors",
            "value": 0
        },
        {
            "name": "rx_q10_packets",
            "value": 0
        },
        {
            "name": "rx_q10_bytes",
            "value": 0
        },
        {
            "name": "rx_q10_errors",
            "value": 0
        },
        {
            "name": "rx_q11_packets",
            "value": 0
        },
        {
            "name": "rx_q11_bytes",
            "value": 0
        },
        {
            "name": "rx_q11_errors",
            "value": 0
        },
        {
            "name": "rx_q12_packets",
            "value": 0
        },
        {
            "name": "rx_q12_bytes",
            "value": 0
        },
        {
            "name": "rx_q12_errors",
            "value": 0
        },
        {
            "name": "rx_q13_packets",
            "value": 0
        },
        {
            "name": "rx_q13_bytes",
            "value": 0
        },
        {
            "name": "rx_q13_errors",
            "value": 0
        },
        {
            "name": "rx_q14_packets",
            "value": 0
        },
        {
            "name": "rx_q14_bytes",
            "value": 0
        },
        {
            "name": "rx_q14_errors",
            "value": 0
        },
        {
            "name": "rx_q15_packets",
            "value": 0
        },
        {
            "name": "rx_q15_bytes",
            "value": 0
        },
        {
            "name": "rx_q15_errors",
            "value": 0
        },
        {
            "name": "tx_q0_packets",
            "value": 0
        },
        {
            "name": "tx_q0_bytes",
            "value": 0
        },
        {
            "name": "tx_q1_packets",
            "value": 0
        },
        {
            "name": "tx_q1_bytes",
            "value": 0
        },
        {
            "name": "tx_q2_packets",
            "value": 0
        },
        {
            "name": "tx_q2_bytes",
            "value": 0
        },
        {
            "name": "tx_q3_packets",
            "value": 0
        },
        {
            "name": "tx_q3_bytes",
            "value": 0
        },
        {
            "name": "tx_q4_packets",
            "value": 0
        },
        {
            "name": "tx_q4_bytes",
            "value": 0
        },
        {
            "name": "tx_q5_packets",
            "value": 0
        },
        {
            "name": "tx_q5_bytes",
            "value": 0
        },
        {
            "name": "tx_q6_packets",
            "value": 0
        },
        {
            "name": "tx_q6_bytes",
            "value": 0
        },
        {
            "name": "tx_q7_packets",
            "value": 0
        },
        {
            "name": "tx_q7_bytes",
            "value": 0
        },
        {
            "name": "tx_q8_packets",
            "value": 0
        },
        {
            "name": "tx_q8_bytes",
            "value": 0
        },
        {
            "name": "tx_q9_packets",
            "value": 0
        },
        {
            "name": "tx_q9_bytes",
            "value": 0
        },
        {
            "name": "tx_q10_packets",
            "value": 0
        },
        {
            "name": "tx_q10_bytes",
            "value": 0
        },
        {
            "name": "tx_q11_packets",
            "value": 0
        },
        {
            "name": "tx_q11_bytes",
            "value": 0
        },
        {
            "name": "tx_q12_packets",
            "value": 0
        },
        {
            "name": "tx_q12_bytes",
            "value": 0
        },
        {
            "name": "tx_q13_packets",
            "value": 0
        },
        {
            "name": "tx_q13_bytes",
            "value": 0
        },
        {
            "name": "tx_q14_packets",
            "value": 0
        },
        {
            "name": "tx_q14_bytes",
            "value": 0
        },
        {
            "name": "tx_q15_packets",
            "value": 0
        },
        {
            "name": "tx_q15_bytes",
            "value": 0
        },
        {
            "name": "rx_unicast_packets",
            "value": 3339424089
        },
        {
            "name": "rx_multicast_packets",
            "value": 0
        },
        {
            "name": "rx_broadcast_packets",
            "value": 0
        },
        {
            "name": "rx_dropped_packets",
            "value": 0
        },
        {
            "name": "rx_unknown_protocol_packets",
            "value": 0
        },
        {
            "name": "tx_unicast_packets",
            "value": 0
        },
        {
            "name": "tx_multicast_packets",
            "value": 0
        },
        {
            "name": "tx_broadcast_packets",
            "value": 0
        },
        {
            "name": "tx_dropped_packets",
            "value": 0
        },
        {
            "name": "tx_link_down_dropped",
            "value": 0
        },
        {
            "name": "rx_crc_errors",
            "value": 0
        },
        {
            "name": "rx_illegal_byte_errors",
            "value": 0
        },
        {
            "name": "rx_error_bytes",
            "value": 0
        },
        {
            "name": "mac_local_errors",
            "value": 0
        },
        {
            "name": "mac_remote_errors",
            "value": 1
        },
        {
            "name": "rx_len_errors",
            "value": 0
        },
        {
            "name": "tx_xon_packets",
            "value": 0
        },
        {
            "name": "rx_xon_packets",
            "value": 0
        },
        {
            "name": "tx_xoff_packets",
            "value": 0
        },
        {
            "name": "rx_xoff_packets",
            "value": 0
        },
        {
            "name": "rx_size_64_packets",
            "value": 48277562
        },
        {
            "name": "rx_size_65_to_127_packets",
            "value": 1758339990
        },
        {
            "name": "rx_size_128_to_255_packets",
            "value": 32433985
        },
        {
            "name": "rx_size_256_to_511_packets",
            "value": 660762779
        },
        {
            "name": "rx_size_512_to_1023_packets",
            "value": 21954846
        },
        {
            "name": "rx_size_1024_to_1522_packets",
            "value": 817654931
        },
        {
            "name": "rx_size_1523_to_max_packets",
            "value": 0
        },
        {
            "name": "rx_undersized_errors",
            "value": 0
        },
        {
            "name": "rx_oversize_errors",
            "value": 0
        },
        {
            "name": "rx_mac_short_pkt_dropped",
            "value": 0
        },
        {
            "name": "rx_fragmented_errors",
            "value": 0
        },
        {
            "name": "rx_jabber_errors",
            "value": 0
        },
        {
            "name": "tx_size_64_packets",
            "value": 0
        },
        {
            "name": "tx_size_65_to_127_packets",
            "value": 0
        },
        {
            "name": "tx_size_128_to_255_packets",
            "value": 0
        },
        {
            "name": "tx_size_256_to_511_packets",
            "value": 0
        },
        {
            "name": "tx_size_512_to_1023_packets",
            "value": 0
        },
        {
            "name": "tx_size_1024_to_1522_packets",
            "value": 0
        },
        {
            "name": "tx_size_1523_to_max_packets",
            "value": 0
        }
    ]
}

````