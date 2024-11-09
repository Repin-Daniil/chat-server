#include "socket_manager.h"

namespace bifrost::handlers::chat {
struct Stats {
    std::atomic<std::uint64_t> opened_sockets{0};
    std::atomic<std::uint64_t> closed_sockets{0};
    std::atomic<std::uint64_t> bytes_read{0};
};

namespace {
std::pair<std::string, std::string> ParseAuthData(std::string message) {
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
    auto at = text.find('@');
    auto delimiter = text.find("\r\n\r\n");

    if (at == std::string::npos || delimiter == std::string::npos || at > delimiter) {
        return {};
    }

    std::string login = text.substr(0, at);
    std::string message_content = text.substr(at + 1, delimiter - at - 1);

    return {login, message_content};
}

std::string SerializeMessage(app::Message msg) {
    return msg.sender.login + "@" + msg.text + "\r\n\r\n";
}

bool Send(userver::engine::io::Socket& sock, std::string message) {
    LOG_INFO() << "Send to Client from server: " << message;
    const auto sent_bytes = sock.SendAll(message.data(), message.size(), {});

    if (sent_bytes != message.size()) {
        LOG_INFO() << "Failed to send all the message";
        return false;
    }

    return true;
}

void DoSend(userver::engine::io::Socket& sock, std::string login, app::Queue::Consumer consumer) {
    app::Message message;
    while (consumer.Pop(message)) {
        std::string data = SerializeMessage(message);
        const auto sent_bytes = sock.SendAll(data.data(), data.size(), {});

        if (sent_bytes != data.size()) {
            LOG_INFO() << "Failed to send all the data";
            return;
        }

        LOG_DEBUG() << "Send message from" << message.sender.login << " to " << login;
    }
}

void DoRecv(userver::engine::io::Socket& sock, std::string login, app::Chat& chat, Stats& stats) {
    std::array<char, 1024> buf; // NOLINT(cppcoreguidelines-pro-type-member-init)

    while (!engine::current_task::ShouldCancel()) {
        const auto read_bytes = sock.ReadSome(buf.data(), buf.size(), {});

        if (!read_bytes) {
            LOG_INFO() << "Failed to read data";
            return;
        }

        stats.bytes_read += read_bytes;
        auto [recipient, message] = ParseMessage(buf.data());

        if (message.empty() || recipient.empty()) {
            LOG_WARNING() << "Empty message or recipient!";
            continue;
        }

        LOG_DEBUG() << "Start sending message from" << login << " to " << recipient;

        if (!chat.Send(recipient, {login, message})) {
            Send(sock, "Can't send message: " + message + " to " + recipient);
        }
    }
}

std::pair<std::string, std::string> RecieveAuthData(userver::engine::io::Socket& sock) {
    std::array<char, 1024> buf; // NOLINT(cppcoreguidelines-pro-type-member-init)

    if (!engine::current_task::ShouldCancel()) {
        const auto read_bytes = sock.ReadSome(buf.data(), buf.size(), {});

        if (!read_bytes) {
            LOG_INFO() << "Failed to read data";
            return {};
        }

        auto [recipient, token] = ParseAuthData(buf.data());
        LOG_DEBUG() << "Get Auth data. Recipient: " << recipient << "; Token: " << token;

        return {recipient, token};
    }

    return {};
}
} // anonymous namespace

const utils::statistics::MetricTag<Stats> kTcpEchoTag{"socket-manager"};

void DumpMetric(utils::statistics::Writer& writer, const Stats& stats) {
    writer["sockets"]["opened"] = stats.opened_sockets;
    writer["sockets"]["closed"] = stats.closed_sockets;
    writer["bytes"]["read"] = stats.bytes_read;
}

void ResetMetric(Stats& stats) {
    stats.opened_sockets = 0;
    stats.closed_sockets = 0;
    stats.bytes_read = 0;
}

SocketManager::SocketManager(const components::ComponentConfig& config,
                             const components::ComponentContext& context)
    : TcpAcceptorBase(config, context),
      chat_(context.FindComponent<app::Application>().GetApp()),
      stats_(context.FindComponent<components::StatisticsStorage>().GetMetricsStorage()->GetMetric(kTcpEchoTag)) {
}

void SocketManager::ProcessSocket(engine::io::Socket&& sock) {
    const auto sock_num = ++stats_.opened_sockets;
    LOG_DEBUG() << "New socket: " << stats_.opened_sockets; // А это само не делается что ли?

    utils::FastScopeGuard guard{
        [this, sock_num]() noexcept {
            LOG_DEBUG() << "Close socket: " << sock_num;
            ++stats_.closed_sockets;
        }
    };

    tracing::Span span{fmt::format("sock_{}", sock_num)};
    span.AddTag("fd", std::to_string(sock.Fd()));

    auto [login, token] = RecieveAuthData(sock);

    if (login.empty() || token.empty() || !chat_.Verify()) {
        Send(sock, "Socket manager: Wrong token or protocol");
        return;
    }

    LOG_DEBUG() << "Login: " << login << " Token: " << token;

    auto queue = chat_.Register(login);

    if (!queue) {
        Send(sock, "Socket manager: User with this token already has an active session");
        return;
    }

    //todo вынести метрики в отдельный файли
    LOG_DEBUG() << "Socket manager: Sending OK to client";
    chat_.Send(login, {"Server", "OK\r\n\r\n"});

    auto send_task = utils::Async("send", DoSend, std::ref(sock), login, queue->GetConsumer());
    DoRecv(sock, login, chat_, stats_);
}
} // namespace bifrost
