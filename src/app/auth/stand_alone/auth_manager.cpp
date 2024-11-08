#include "auth_manager.h"

#include <iomanip>
#include <random>
#include <sstream>

namespace bifrost::app::auth {
  class TokenGenerator {
    public:
      std::string GenerateNewToken() {
        std::stringstream ss;

        while (ss.str().size() != 32) {
          ss.clear();
          ss << std::setfill('0') << std::setw(16) << std::hex << generator1_();
          ss << std::setfill('0') << std::setw(16) << std::hex << generator2_();
        }

        return ss.str();
      }

    private:
      std::random_device random_device_;
      std::mt19937_64 generator1_{
        [this] {
          std::uniform_int_distribution<std::mt19937_64::result_type> dist;
          return dist(random_device_);
        }()
      };

      std::mt19937_64 generator2_{
        [this] {
          std::uniform_int_distribution<std::mt19937_64::result_type> dist;
          return dist(random_device_);
        }()
      };
  };


}
