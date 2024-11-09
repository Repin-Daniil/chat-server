#pragma once

#include <string>

#include "userver/clients/http/client.hpp"
#include "userver/clients/http/component.hpp"
#include "userver/components/component_list.hpp"
#include "userver/components/minimal_server_component_list.hpp"
#include "userver/server/handlers/http_handler_base.hpp"

#include "app/application_component.h"

namespace bifrost::handlers::auth {

using namespace std::literals;

class AuthRegistrationHandler final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-auth-registration"sv;

    using HttpHandlerBase::HttpHandlerBase;

    AuthRegistrationHandler(const userver::components::ComponentConfig& config,
                       const userver::components::ComponentContext& context);

    std::string HandleRequestThrow(const userver::server::http::HttpRequest& request,
                                   userver::server::request::RequestContext& context) const override;

private:
    app::Chat& chat_;
};


}