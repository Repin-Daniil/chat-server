#include "user_registry.hpp"

#include "receivers/single_receiver.hpp"

namespace chat::app::registry {
std::shared_ptr<Queue> UserRegistry::AddReceiver(const Login& new_recipient) {
    auto queue = Queue::Create();

    auto [receiver, is_inserted] =
        login_to_receiver_.Insert(new_recipient, std::make_shared<SingleReceiver>(queue, new_recipient));

    if (!is_inserted) {
        if (!receiver->GetQueue()->NoMoreConsumers()) {
            LOG_WARNING() << "Receiver with id " << new_recipient << " already exists";
            return {};
        }

        LOG_INFO() << "Get queue of user with id " << new_recipient << " from registry";
        return receiver->GetQueue();
    }

    LOG_INFO() << "Add user with id " << new_recipient << " to registry";
    return queue;
}

bool UserRegistry::Send(const Login& recipient, Message message) {
    auto receiver = login_to_receiver_.Get(recipient);

    if (!receiver) {
        LOG_WARNING() << "Registry::Send(): No receiver with login " << recipient;
        return false;
    }

    LOG_DEBUG() << "Registry::Send(): Find receiver with login " << recipient;

    return receiver->Send(std::move(message));
}

std::size_t UserRegistry::GetApproxRegistrySize() const { return login_to_receiver_.SizeApprox(); }

}  // namespace chat::app::registry
