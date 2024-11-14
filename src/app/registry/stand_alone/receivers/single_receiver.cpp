#include "single_receiver.hpp"

#include "userver/logging/log.hpp"

namespace bifrost::app::registry {
SingleReceiver::SingleReceiver(std::shared_ptr<Queue> queue, std::string id) : id_(std::move(id)),
                                                                               queue_(std::move(queue)),
                                                                               producer_(queue_->GetMultiProducer()) {
    // LOG_DEBUG() << "Create SingleReceiver " << id_;
}

bool SingleReceiver::Send(Message message) {
    if (!producer_.Push(std::move(message))) {
        LOG_ERROR() << "Receiver::Send(): Can't push message to queue from" << message.sender.login << " with text: " << message.text;
        return false;
    }

    return true;
}

SingleReceiver::~SingleReceiver() {
    // LOG_DEBUG() << "Destruct SingleReceiver " << id_;
}
}