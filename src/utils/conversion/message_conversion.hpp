#pragma once

#include <string>
#include "userver/logging/log.hpp"

namespace chat::utils {
std::pair<std::string, std::string> ParseAuthData(std::string message) {
    LOG_DEBUG() << "ParseAuthData() Text: " << message;

    auto at = message.find('@');
    auto delimiter = message.find("\r\n\r\n");

    if (at == std::string::npos || delimiter == std::string::npos || at > delimiter) {
        return {};
    }

    std::string user = message.substr(0, at);
    std::string token = message.substr(at + 1, delimiter - at - 1);

    return {user, token};
}

std::pair<std::string, std::string> ParseMessage(std::string text) {
    LOG_DEBUG() << "ParseMessage() Text: " << text;

    auto at = text.find('@');
    auto delimiter = text.find("\r\n\r\n");

    if (at == std::string::npos || delimiter == std::string::npos || at > delimiter) {
        return {};
    }

    std::string login = text.substr(0, at);
    std::string message_content = text.substr(at + 1, delimiter - at - 1);

    return {login, message_content};
}

std::string SerializeMessage(app::Message msg) { return msg.sender.login + "@" + msg.text + "\r\n\r\n"; }
}  // namespace chat::utils