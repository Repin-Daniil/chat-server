#include "single_receiver.h"

#include "userver/logging/log.hpp"

namespace bifrost::app::registry {
//SingleReceiver::SingleReceiver(std::shared_ptr<Queue> queue, std::string id)

bool SingleReceiver::Send(Message message) {
    if (!producer_.Push(std::move(message))) {
//        LOG_ERROR() << "Can't send message from" << message.sender  << " with text: " << message.text;
        return false;
    }

//    LOG_TRACE() << "Send message from" << message.sender  << " with text: " << message.text;
    return true;
}

//SingleReceiver::~SingleReceiver() {
//    LOG_DEBUG() << "Destruct SingleReceiver " << id_;
//}

}