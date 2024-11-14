#pragma once

#include "userver/components/component_base.hpp"
#include <userver/components/component_context.hpp>
#include "userver/components/component_list.hpp"

#include "chat/chat.hpp"

namespace bifrost::app {
class Application final : public userver::components::LoggableComponentBase {
public:
    static constexpr std::string_view kName = "application";

    Application(const userver::components::ComponentConfig& config,
                const userver::components::ComponentContext& context);

    Chat& GetApp();

private:
    Chat chat_;
};
}