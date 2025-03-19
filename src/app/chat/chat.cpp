#include "chat.hpp"

namespace bifrost::app {
Chat::Chat(auth::AuthManager& auth, registry::UserRegistry& registry) : auth_(auth), registry_(registry) {}

std::pair<auth::Token, bool> Chat::AuthenticateUser(const Login& login, const std::string& password) {
    return auth_.AuthenticateUser(login, password);
}

bool Chat::Verify(const Login& login, const auth::Token& token) { return auth_.VerifyToken(login, token); }

bool Chat::Send(const Login& recipient, Message message) { return registry_.Send(recipient, std::move(message)); }

std::shared_ptr<Queue> Chat::Register(const Login& new_recipient) { return registry_.AddReceiver(new_recipient); }
}  // namespace bifrost::app