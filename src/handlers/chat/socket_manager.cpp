#include "socket_manager.h"

namespace bifrost::handlers::chat {
struct Stats {
    std::atomic<std::uint64_t> opened_sockets{0};
    std::atomic<std::uint64_t> closed_sockets{0};
    std::atomic<std::uint64_t> bytes_read{0};
};

namespace {

std::pair<std::string, std::string> ParseAuthData(std::string message) {
    std::string user = message.substr(0, message.find('@'));
    std::string token = message.substr(message.find('@') + 1, message.length() - 4);

    return std::make_pair(user, token);
}

app::Message ParseMessage(std::string message) {
    app::Message msg;
    //TODO Поставитьв везде обработку исключений!
    msg.sender.login = message.substr(0, message.find('@'));
    msg.text = message.substr(message.find('@') + 1, message.length() - 4);

    return msg;
}

std::string SerializeMessage(app::Message msg) {
    return msg.sender.login + "@" + msg.text;
}


void DoSend(userver::engine::io::Socket& sock, app::Queue::Consumer consumer) {
    app::Message message;
    while (consumer.Pop(message)) {
        std::string data = SerializeMessage(message);
        const auto sent_bytes = sock.SendAll(data.data(), data.size(), {});

        if (sent_bytes != data.size()) {
            LOG_INFO() << "Failed to send all the data";
            return;
        }
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

        if (!chat.Send(recipient.login, {login,message})) {
            // Говорим в сокет, что не получилось отправить. Наверное, надо будет продюсера сюда еще прокинуть
        }
    }
}

std::pair<std::string, std::string> RecieveAuthData(userver::engine::io::Socket& sock) {
    std::array<char, 1024> buf; // NOLINT(cppcoreguidelines-pro-type-member-init)

    if(!engine::current_task::ShouldCancel()) {
        const auto read_bytes = sock.ReadSome(buf.data(), buf.size(), {});

        if (!read_bytes) {
            LOG_INFO() << "Failed to read data";
            return {};
        }

        return ParseAuthData(buf.data());
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
      stats_(context.FindComponent<components::StatisticsStorage>().GetMetricsStorage()->GetMetric(kTcpEchoTag)),
      chat_(context.FindComponent<app::Application>().GetApp()) {
}

void SocketManager::ProcessSocket(engine::io::Socket&& sock) {
    const auto sock_num = ++stats_.opened_sockets;
    LOG_INFO() << "New socket: " << stats_.opened_sockets; // А это само не делается что ли?

    utils::FastScopeGuard guard{
        [this, sock_num]() noexcept {
            LOG_TRACE() << "Close socket: " << sock_num;
            ++stats_.closed_sockets;
        }
    };

    tracing::Span span{fmt::format("sock_{}", sock_num)};
    span.AddTag("fd", std::to_string(sock.Fd()));


    auto [login, token] = RecieveAuthData(sock);

    if (login.empty() || token.empty() || !chat_.Verify()) {
        // Объясняем причину и рвем сокет
        return;
    }

    auto queue = chat_.Register(login);

    if (!queue) {
        // Уже есть очередь и живой консьюмер, объясняем причину и рвем сокет
    }

    //todo вынести метрики в отдельный файлик

    auto send_task = utils::Async("send", DoSend, std::ref(sock), queue->GetConsumer());
    DoRecv(sock, login, chat_, stats_);
}
} // namespace bifrost