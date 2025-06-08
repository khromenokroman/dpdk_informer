#include "printer.hpp"

#include <arpa/inet.h>
#include <dirent.h>
#include <fcntl.h>
#include <fmt/format.h>
#include <linux/if_arp.h>
#include <netlink/route/neighbour.h>
#include <netlink/route/route.h>

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

} // namespace os::network
