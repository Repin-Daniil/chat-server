#include "bifrost.h"

namespace bifrost {

struct Stats {
  std::atomic<std::uint64_t> opened_sockets{0};
  std::atomic<std::uint64_t> closed_sockets{0};
  std::atomic<std::uint64_t> bytes_read{0};
  std::atomic<std::uint64_t> dialogs_opened{0};
  std::atomic<std::uint64_t> dialogs_closed{0};
};

namespace {

using namespace userver;

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


const utils::statistics::MetricTag<Stats> kTcpEchoTag{"tcp-echo"};

void DumpMetric(utils::statistics::Writer& writer, const Stats& stats) {
    writer["sockets"]["opened"] = stats.opened_sockets;
    writer["sockets"]["closed"] = stats.closed_sockets;
    writer["bytes"]["read"] = stats.bytes_read;
    writer["dialogs"]["opened"] = stats.dialogs_opened;
    writer["dialogs"]["closed"] = stats.dialogs_closed;
}

void ResetMetric(Stats& stats) {
    stats.opened_sockets = 0;
    stats.closed_sockets = 0;
    stats.bytes_read = 0;
    stats.dialogs_opened = 0;
    stats.dialogs_closed = 0;
}

Bifrost::Bifrost(const components::ComponentConfig& config, const components::ComponentContext& context)
    : TcpAcceptorBase(config, context),
      stats_(context.FindComponent<components::StatisticsStorage>().GetMetricsStorage()->GetMetric(kTcpEchoTag)) {
}

void Bifrost::ProcessSocket(engine::io::Socket&& sock) {
  LOG_INFO() << "New socket: " << stats_.opened_sockets + 1;

  std::shared_ptr<Queue> from_queue;
  std::shared_ptr<Queue> to_queue;

  mutex_.lock();

  if (stats_.opened_sockets % 2 == 0) {
    dialog_.queue_1 = Queue::Create();
    dialog_.queue_2 = Queue::Create();

    to_queue = dialog_.queue_1;
    from_queue = dialog_.queue_2;
  } else {
    to_queue =  dialog_.queue_2;
    from_queue = dialog_.queue_1;

    dialog_.queue_1.reset();
    dialog_.queue_2.reset();

    LOG_INFO() << "New dialog: " << ++stats_.dialogs_opened;
  }

  const auto sock_num = ++stats_.opened_sockets;

  utils::FastScopeGuard guard{[this, sock_num]() noexcept {
    LOG_INFO() << "Closing socket: " << sock_num;
    ++stats_.closed_sockets;
    ++stats_.dialogs_closed;
  }};

  mutex_.unlock();

  tracing::Span span{fmt::format("sock_{}", sock_num)};
  span.AddTag("fd", std::to_string(sock.Fd()));

  auto send_task = utils::Async("send", DoSend, std::ref(sock), to_queue->GetConsumer());
  to_queue.reset();
  DoRecv(sock, from_queue->GetProducer(), stats_);
}

}

