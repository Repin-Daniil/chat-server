#pragma once
#include "userver/concurrent/queue.hpp"

namespace chat::app {

using Login = std::string;

struct User {
    Login login;
};
// TODO переместить в чат или в dto
struct Message {
    User sender;
    std::string text;

    // User recipient;
    // Time time?
};

using Queue = userver::concurrent::NonFifoMpscQueue<Message>;

namespace registry {

class Receiver {
public:
    virtual bool Send(Message message) = 0;
    virtual std::shared_ptr<Queue> GetQueue() = 0;
    // TODO Destruct(), Flush()??? Добавить какие-нибудь операции для работы с очередью; МБ гет продьюсер, г

    virtual ~Receiver() = default;
};

}  // namespace registry
}  // namespace chat::app