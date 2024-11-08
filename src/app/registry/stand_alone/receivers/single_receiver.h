#include "app/registry/stand_alone/receiver.h"

namespace bifrost::app::registry {
class SingleReceiver : public Receiver {
public:
    SingleReceiver(std::shared_ptr<Queue> queue, std::string id) : id_(std::move(id)),
                                                                               queue_(std::move(queue)),
                                                                               producer_(queue_->GetMultiProducer()) {
        //    LOG_DEBUG() << "Create SingleReceiver " << id_;
    }

    bool Send(Message message) override;

//    ~SingleReceiver() = d;

private:
    std::string id_;
    std::shared_ptr<Queue> queue_;
    Queue::MultiProducer producer_;
};
}