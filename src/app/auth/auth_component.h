#pragma once
#include "userver/components/component_base.hpp"
#include <userver/components/component_context.hpp>

#include "stand_alone/auth_manager.h"

namespace bifrost::app::auth {

class AuthComponent : public userver::components::LoggableComponentBase {
public:
    static constexpr std::string_view kName = "auth-manager";

    AuthComponent(const userver::components::ComponentConfig& config,
                const userver::components::ComponentContext& context);

    AuthManager& GetAuthManager();

    ~AuthComponent() override = default;

private:
    AuthManager auth_manager_;
};

}