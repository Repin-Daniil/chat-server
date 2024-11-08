#include "single_receiver.h"

#include "userver/logging/log.hpp"

namespace bifrost::app::registry {
SingleReceiver::SingleReceiver(std::shared_ptr<Queue> queue, std::string id) : id_(std::move(id)),
                                                                               queue_(std::move(queue)),
                                                                               producer_(queue_->GetMultiProducer()) {
    LOG_TRACE() << "Create SingleReceiver " << id_;
}

bool SingleReceiver::Send(Message message) {
    if (!producer_.Push(std::move(message))) {
        LOG_ERROR() << "Can't send message from" << message.sender.login << " with text: " << message.text;
        return false;
    }

    LOG_TRACE() << "Send message from" << message.sender.login << " with text: " << message.text;
    LOG_DEBUG() << "Message from" << message.sender.login << " to " << id_ << " was sucessfully pushed to queue";
    return true;
}

SingleReceiver::~SingleReceiver() {
    LOG_TRACE() << "Destruct SingleReceiver " << id_;
}
}