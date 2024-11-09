#include "auth_registration_handler.h"


namespace {
using namespace userver;

void CheckArgument(const std::string& arg, const std::string& arg_name) {
    if (arg.empty()) {
        throw std::invalid_argument("No  '" + arg_name + "' argument!");
    }
}


}  // namespace

namespace bifrost::handlers::auth {

AuthRegistrationHandler::AuthRegistrationHandler(const components::ComponentConfig& config,
                                       const components::ComponentContext& context)
    : HttpHandlerBase(config, context), chat_(context.FindComponent<app::Application>().GetApp()) {
}

std::string AuthRegistrationHandler::HandleRequestThrow(const userver::server::http::HttpRequest& request,
                                                   userver::server::request::RequestContext&) const {
    const std::string& name = request.GetArg("name");
    const std::string& password = request.GetArg("password");

    try {
        CheckArgument(name, "name");
        CheckArgument(name, "password");

    } catch (const std::invalid_argument& e) {
        throw server::handlers::ClientError(server::handlers::ExternalBody{e.what()});
    }

    auto token = chat_.AuthenticateUser(name, password);

    return token;
}


}  // namespace handlers