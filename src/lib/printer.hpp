#pragma once

#include <memory>
#include <string>

#include "informer/dpdk_informer.hpp"

namespace dpdk::network {

struct Json {};
// NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Json, );

namespace exceptions {

struct DpdkEx : std::runtime_error {
    using std::runtime_error::runtime_error;
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

   private:

    Json m_json{};
};

} // namespace os::network
