#include "token.hpp"

namespace bifrost::utils {
std::string TokenGenerator::GenerateNewToken() {
    std::stringstream ss;

    while (ss.str().size() != 32) {
        ss.clear();
        ss << std::setfill('0') << std::setw(16) << std::hex << generator1_();
        ss << std::setfill('0') << std::setw(16) << std::hex << generator2_();
    }

    return ss.str();
}
}  // namespace bifrost::utils