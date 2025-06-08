#pragma once
#include <fmt/format.h>

#include <nlohmann/json.hpp>

namespace dpdk::network {

namespace exceptions {
struct NetNamespaceHandlerEx : std::runtime_error {
    using std::runtime_error::runtime_error;
};
} // namespace exceptions

class InformerDpdk {
   public:
    InformerDpdk() = default;
    virtual ~InformerDpdk() = default;

    InformerDpdk(InformerDpdk const &) = delete;
    InformerDpdk(InformerDpdk &&) = delete;
    InformerDpdk &operator=(InformerDpdk const &) = delete;
    InformerDpdk &operator=(InformerDpdk &&) = delete;

    virtual nlohmann::json get_all_interface() = 0;
    virtual ::nlohmann::json get_interface_info(std::string const &interface_name) = 0;

    static std::unique_ptr<InformerDpdk> create();

   private:
    static InformerDpdk *create(char *error_message) noexcept;
    /**
     * @brief Максимальный размер буфера для сообщений об ошибках.
     */
    static constexpr std::size_t M_MAX_BUFFER_SIZE = 1024;
};

inline std::unique_ptr<InformerDpdk> InformerDpdk::create() {
    auto const message_error = std::make_unique<char[]>(M_MAX_BUFFER_SIZE);

    InformerDpdk *new_object = create(message_error.get());
    if (new_object == nullptr) {
        throw std::runtime_error(message_error.get());
    }

    return std::unique_ptr<InformerDpdk>{new_object};
}

} // namespace dpdk::network
