#include "auth_handler.hpp"

namespace bifrost::handlers::auth {
AuthHandler::AuthHandler(const userver::components::ComponentConfig& config,
                         const userver::components::ComponentContext& context)
    : HttpHandlerBase(config, context), chat_(context.FindComponent<app::Application>().GetApp()) {
}

std::string AuthHandler::HandleRequestThrow(const userver::server::http::HttpRequest& request,
                                            userver::server::request::RequestContext& request_context) const {
    const auto& name = request.GetArg("name");
    const auto& password = request.RequestBody(); //FIXME протестировать

    if (name.empty() || password.empty()) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        return "No 'name' or 'password' argument"s;
    }

    if (name.length() > 100 || password.length() > 100) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kBadRequest);
        return "Password or login is too long. Max length: 100";
    }

    auto [token, is_created] = chat_.AuthenticateUser(name, password);

    if (token.empty()) {
        request.SetResponseStatus(userver::server::http::HttpStatus::kUnauthorized);
        return "Wrong password for name "s + name;
    }

    if (is_created) {
        request.SetResponseStatus(userver::server::http::HttpStatus::Created);
    }


    return token;
}
}