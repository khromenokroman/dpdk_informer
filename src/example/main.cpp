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
