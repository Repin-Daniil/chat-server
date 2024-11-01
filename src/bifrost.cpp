#include "bifrost.h"

#include <queue>

namespace {
using namespace userver;

}

namespace bifrost {


struct Stats {
    std::atomic<std::uint64_t> opened_sockets{0};
    std::atomic<std::uint64_t> closed_sockets{0};
    std::atomic<std::uint64_t> bytes_read{0};
};

const utils::statistics::MetricTag<Stats> kTcpEchoTag{"tcp-echo"};

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

Bifrost::Bifrost(const components::ComponentConfig& config, const components::ComponentContext& context)
    : TcpAcceptorBase(config, context),
      stats_(context.FindComponent<components::StatisticsStorage>().GetMetricsStorage()->GetMetric(kTcpEchoTag)) {
}


namespace {



void DoSend(engine::io::Socket& sock, Queue::Consumer consumer) {
    std::string data;
    while (consumer.Pop(data)) {
        const auto sent_bytes = sock.SendAll(data.data(), data.size(), {});
        if (sent_bytes != data.size()) {
            LOG_INFO() << "Failed to send all the data";
            return;
        }
    }
}

void DoRecv(engine::io::Socket& sock, Queue::Producer producer, Stats& stats) {
    std::array<char, 1024> buf;  // NOLINT(cppcoreguidelines-pro-type-member-init)
    while (!engine::current_task::ShouldCancel()) {
        const auto read_bytes = sock.ReadSome(buf.data(), buf.size(), {});
        if (!read_bytes) {
            LOG_INFO() << "Failed to read data";
            return;
        }

        stats.bytes_read += read_bytes;
        if (!producer.Push({buf.data(), read_bytes})) {
            return;
        }
    }
}

}  // anonymous namespace

void Bifrost::ProcessSocket(engine::io::Socket&& sock) {
  if (stats_.opened_sockets == 2) {
    return;
  }
    const auto sock_num = ++stats_.opened_sockets;

    tracing::Span span{fmt::format("sock_{}", sock_num)};
    span.AddTag("fd", std::to_string(sock.Fd()));

    utils::FastScopeGuard guard{[this]() noexcept {
        LOG_INFO() << "Closing socket";
        ++stats_.closed_sockets;
      --stats_.opened_sockets; //todo delete!
    }};

    if (mutex_.try_lock()) {
      queue_1_ = Queue::Create();
      queue_2_ = Queue::Create();

      if (!queue_1_->GetProducer().Push("AWAIT\r\n\r\n") ||
      !queue_2_->GetProducer().Push("SEND\r\n\r\n")) {
        return;
      }

      auto send_task = utils::Async("send", DoSend, std::ref(sock), queue_2_->GetConsumer());
      DoRecv(sock, queue_1_->GetProducer(), stats_);
      mutex_.unlock();
    } else {
      auto send_task = utils::Async("send", DoSend, std::ref(sock), queue_1_->GetConsumer());
      DoRecv(sock, queue_2_->GetProducer(), stats_);
    }


}

}  // namespace samples::tcp::echo
