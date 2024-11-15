#include "socket_manager.hpp"

#include <userver/server/handlers/server_monitor.hpp>
#include <userver/clients/dns/component.hpp>
#include <userver/components/component.hpp>
#include <userver/utils/statistics/metric_tag.hpp>
#include <userver/components/statistics_storage.hpp>
#include <userver/utils/statistics/metrics_storage.hpp>

#include "utils/conversion/message_conversion.hpp"

namespace bifrost::handlers::chat {

const userver::utils::statistics::MetricTag<utils::Stats> kSocketManagerTag{"socket-manager"};

namespace {

void DoSend(userver::engine::io::Socket& sock, std::string login, app::Queue::Consumer consumer) {
    app::Message message;
    while (!userver::engine::current_task::ShouldCancel() && consumer.Pop(message)) {
        std::string data = utils::SerializeMessage(message);
        LOG_DEBUG() << "DoSend(): Send Message " << data << " from " << message.sender.login;

        const auto sent_bytes = sock.SendAll(data.data(), data.size(), {});

        if (sent_bytes != data.size()) {
            LOG_WARNING() << "Failed to send all the data";
            return;
        }

        LOG_DEBUG() << "DoSend(): Successfully send message from " << message.sender.login << " to " << login;
    }
}

void DoRecv(userver::engine::io::Socket& sock, std::string login, app::Chat& chat, utils::Stats& stats) {
    std::array<char, 1024> buf; // NOLINT(cppcoreguidelines-pro-type-member-init)
    std::stringstream data;

    while (!userver::engine::current_task::ShouldCancel()) {
        std::fill(buf.begin(), buf.end(), 0);

        const auto read_bytes = sock.ReadSome(buf.data(), buf.size(), {});

        if (!read_bytes) {
            LOG_WARNING() << "Failed to read data in DoRecv";
            return;
        }

        stats.bytes_read += read_bytes;

        LOG_DEBUG() << "DoRecv(): Get new message. Buffer: " << buf.data();

        data << std::string{buf.data(), read_bytes};

        if (std::string_view(buf.data(), read_bytes).find("\r\n\r\n") == std::string::npos) { //FIXME Протестировать
            continue;
        }

        auto [recipient, message] = utils::ParseMessage(data.str());
        data.str("");
        LOG_DEBUG() << "DoRecv(): Parse message: {Recipient: " << recipient << "; Message: " << message << "}";

        if (message.empty() || recipient.empty()) {
            LOG_WARNING() << "Empty message or recipient!";
            continue;
        }

        LOG_DEBUG() << "Start sending message from" << login << " to " << recipient;

        if (!chat.Send(recipient, {login, message})) {
            LOG_WARNING() << "Can't send message: " << message << " to " << recipient;
        }
    }
}

std::pair<std::string, std::string> RecieveAuthData(userver::engine::io::Socket& sock) {
    std::array<char, 1024> buf; // NOLINT(cppcoreguidelines-pro-type-member-init)

    LOG_DEBUG() << "RecieveAuthData(): Trying to read from socket";

    if (!userver::engine::current_task::ShouldCancel()) {
        const auto read_bytes = sock.ReadSome(buf.data(), buf.size(), {});

        if (!read_bytes) {
            LOG_WARNING() << "RecieveAuthData(): Failed to read auth data";
            return {};
        }

        LOG_DEBUG() << "RecieveAuthData(): Successfully read from socket, auth data: " << buf.data();

        auto [recipient, token] = utils::ParseAuthData(buf.data());
        LOG_DEBUG() << "RecieveAuthData(): Parse Auth Data. Result{Recipient: " << recipient << "; Token: " << token <<"}";

        return {recipient, token};
    }

    return {};
}
}

SocketManager::SocketManager(const userver::components::ComponentConfig& config,
                             const userver::components::ComponentContext& context)
    : TcpAcceptorBase(config, context),
      chat_(context.FindComponent<app::Application>().GetApp()),
      stats_(context.FindComponent<userver::components::StatisticsStorage>().GetMetricsStorage()->GetMetric(kSocketManagerTag)) {
}

void SocketManager::ProcessSocket(userver::engine::io::Socket&& sock) {
    const auto sock_num = ++stats_.opened_sockets;
    LOG_DEBUG() << "New socket: " << stats_.opened_sockets;

    userver::utils::FastScopeGuard guard{
        [this, sock_num]() noexcept {
            LOG_DEBUG() << "Close socket: " << sock_num;
            ++stats_.closed_sockets;
        }
    };

    userver::tracing::Span span{fmt::format("sock_{}", sock_num)};
    span.AddTag("fd", std::to_string(sock.Fd()));

    auto [login, token] = RecieveAuthData(sock);
    LOG_INFO() << "ProcessSocket(): Get Auth Data{" <<"Login: " << login << "; Token: " << token << "}";

    if (login.empty() || token.empty() || !chat_.Verify(login, token)) {
        LOG_WARNING() << "ProcessSocket(): Token or Login is empty, or wrong token";
        return;
    }


    auto queue = chat_.Register(login);

    if (!queue) {
        LOG_WARNING() << "ProcessSocket(): User with this token already has an active session";
        return;
    }
//TODO нужно чтобы, когда появлялся еще один человек, который ввел правльный пароль и появился новый токен, старый инвалидировался, а человека выкидывали
    LOG_DEBUG() << "ProcessSocket(): Sending OK to client";
    chat_.Send(login, {"Server", "OK"});

    auto send_task = userver::utils::Async("send", DoSend, std::ref(sock), login, queue->GetConsumer());
    LOG_DEBUG() << "ProcessSocket(): Chat is ready, start DoRecv";

    DoRecv(sock, login, chat_, stats_);
    send_task.RequestCancel();
}

}
