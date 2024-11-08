#include "app/registry/stand_alone/receiver.h"

namespace bifrost::app::registry {
class SingleReceiver : public Receiver {
public:
    SingleReceiver(std::shared_ptr<Queue> queue, std::string id);

    bool Send(Message message) override;

    ~SingleReceiver();

private:
    std::string id_;
    std::shared_ptr<Queue> queue_;
    Queue::MultiProducer producer_;
};
}