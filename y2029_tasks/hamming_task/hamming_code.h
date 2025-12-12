#include <cstdint>
#include <vector>
#include <string>

namespace hamming_code{
    constexpr uint8_t kFullBit = 1;
    constexpr uint8_t kULLBait_in_bits = 64;

    constexpr const char kEncodeCommand[] = "encode";
    constexpr const char kDecodeCommand[] = "decode";
    constexpr const char kValidationCommand[] = "is_valid";

    struct HammingCode{
        uint8_t GetBit(const std::vector<uint64_t>& value, uint64_t index);
    
        void SetBit(std::vector<uint64_t>& value, uint64_t index);
    
        void FlipBit(std::vector<uint64_t>& value, uint64_t index);
    
        std::vector<uint64_t> ConvertToValue(char* bit_vector);
    
        std::string ConvertToBitVector(const std::vector<uint64_t>& value, uint64_t length);
    
        uint16_t GetTestBits(uint64_t data_bits);
    
        std::string Encode(uint64_t input_length, uint64_t output_length, char *bit_vector);
    
        std::string Decode(uint64_t input_length, char *bit_vector);
    
        bool IsValid(uint64_t input_length, char* bit_vector);
    };
} // namespace hamming_code