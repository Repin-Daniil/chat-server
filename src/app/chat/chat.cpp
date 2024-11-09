#include "chat.h"

namespace bifrost::app {
Chat::Chat(auth::AuthManager& auth, registry::UserRegistry& registry) : auth_(auth), registry_(registry) {
}

bool Chat::Verify() {
    return true;
}

bool Chat::Send(const Login& recipient, Message message) {
    return registry_.Send(recipient, std::move(message));
}

std::shared_ptr<Queue> Chat::Register(const Login& new_recipient) {
    return registry_.AddReceiver(new_recipient);
}
}