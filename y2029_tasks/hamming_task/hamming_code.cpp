#include "hamming_code.h"

#include <iostream>
#include <cstdint>
#include <cstring>
#include <vector>
#include <string>

namespace hamming_code{
    uint8_t HammingCode::GetBit(const std::vector<uint64_t>& value, uint64_t index){
        uint64_t block = index / kULLBait_in_bits;
        uint64_t bit_in_block  = index & (kULLBait_in_bits - kFullBit);

        if (block >= value.size()){
            return 0;
        }

        return (value[block] >> bit_in_block) & kFullBit;
    }

    void HammingCode::SetBit(std::vector<uint64_t>& value, uint64_t index){
        uint64_t block = index / kULLBait_in_bits;
        uint64_t bit_in_block  = index & (kULLBait_in_bits - kFullBit);

        if (block >= value.size()){
            value.resize(block + kFullBit, 0);
        }

        value[block] |= (1ULL << bit_in_block);
    }

    void HammingCode::FlipBit(std::vector<uint64_t>& value, uint64_t index){
        uint64_t block = index / kULLBait_in_bits;
        uint64_t bit_in_block  = index & (kULLBait_in_bits - kFullBit);

        if (block >= value.size()){
            return;
        }

        value[block] ^= (1ULL << bit_in_block);
    }

    std::vector<uint64_t> HammingCode::ConvertToValue(char* bit_vector){
        std::vector<uint64_t> value;
        uint64_t vector_length = 0;

        for (uint64_t i = 0; bit_vector[i] != '\0'; i++){
            vector_length++;
        }

        for (uint64_t i = 0; i < vector_length; i++){
            if (bit_vector[i] == '1'){
                uint64_t index = vector_length - kFullBit - i;
                SetBit(value, index);
            }
        }

        return value;
    }

    std::string HammingCode::ConvertToBitVector(const std::vector<uint64_t>& value, uint64_t length){
        std::string bit_vector{};
        bit_vector.reserve(length);

        for (int64_t i = length - 1; i >= 0; i--){
            bit_vector.push_back(GetBit(value, i) + '0');
        }

        return bit_vector;
    }

    uint16_t HammingCode::GetTestBits(uint64_t data_bits){
        uint16_t test_bits = 0;

        while ((kFullBit << test_bits) < data_bits + test_bits + kFullBit){
            test_bits++;
        }

        return test_bits;
    }

    std::string HammingCode::Encode(uint64_t input_length, uint64_t output_length, char *bit_vector){
        uint64_t data_bits = input_length;
        uint64_t total_bits = output_length;
        uint64_t test_bits = total_bits - data_bits;

        std::vector<uint64_t> value = ConvertToValue(bit_vector);

        std::vector<uint64_t> encoded_value;

        uint64_t data_index = 0;

        for (uint64_t i = 1; i <= total_bits; i++){
            if ((i & (i - 1)) == 0){
                continue;
            }

            uint64_t src_pos = data_bits - 1 - data_index;

            if (GetBit(value, src_pos)){
                SetBit(encoded_value, i - 1);
            }

            data_index++;
        }

        for (uint64_t i = 0; i < test_bits; i++){
            uint64_t test_position = (1ULL << i);
            uint64_t reminder = 0;

            for (uint64_t j = 1; j <= total_bits; j++){
                if (j & test_position){
                    if (GetBit(encoded_value, j - 1)){
                        reminder ^= kFullBit;
                    }
                }
            }

            if (reminder){
                SetBit(encoded_value, test_position - 1);
            }
        }

        return ConvertToBitVector(encoded_value, total_bits);
    }

    std::string HammingCode::Decode(uint64_t input_length, char *bit_vector){
        uint64_t total_bits = input_length;

        std::vector<uint64_t> value = ConvertToValue(bit_vector);

        uint64_t test_bits = 0;

        while ((kFullBit << test_bits) <= total_bits){
            test_bits++;
        }

        uint64_t data_bits = total_bits - test_bits;

        uint64_t mistake = 0;

        for (uint64_t i = 0; i < test_bits; i++){
            uint64_t test_position = (1ULL << i);
            uint64_t reminder = 0;

            for (uint64_t j = 1; j <= total_bits; j++){
                if (j & test_position){
                    if (GetBit(value, j - 1)){
                        reminder ^= kFullBit;
                    }
                }
            }

            if (reminder){
                mistake |= test_position;
            }
        }

        if (mistake != 0 && mistake <= total_bits){
            FlipBit(value, mistake - 1);
        }

        std::vector<uint64_t> decoded_value;
        uint64_t bit_index = 0;

        for (uint64_t i = 1; i <= total_bits; i++){
            if ((i & (i - 1)) == 0){
                continue;
            }

            if (GetBit(value, i - 1)){
                SetBit(decoded_value, data_bits - 1 - bit_index);
            }

            bit_index++;
        }

        return ConvertToBitVector(decoded_value, data_bits);
    }

    bool HammingCode::IsValid(uint64_t input_length, char* bit_vector){
        uint64_t total_bits = input_length;

        std::vector<uint64_t> value = ConvertToValue(bit_vector);

        uint64_t test_bits = 0;
        while ((kFullBit << test_bits) <= total_bits){
            test_bits++;
        }

        uint64_t mistake = 0;

        for (uint64_t i = 0; i < test_bits; i++){
            uint64_t test_position = (1ULL << i);
            uint64_t reminder = 0;

            for (uint64_t j = kFullBit; j <= total_bits; j++){
                if (j & test_position){
                    if (GetBit(value, j - 1)){
                        reminder ^= kFullBit;
                    }
                }
            }

            if (reminder){
                mistake |= test_position;
            }
        }

        return (mistake == 0);
    }
} // namespace hamming_code

int main(int argc, char* argv[]){
    hamming_code::HammingCode code;

    if (strcmp(hamming_code::kEncodeCommand, argv[1]) == 0){
        std::cout << code.Encode(std::atoi(argv[2]), std::atoi(argv[3]), argv[4]) << '\n';
        return 0;
    } else if(strcmp(hamming_code::kDecodeCommand, argv[1]) == 0){
        std::cout << code.Decode(std::atoi(argv[2]), argv[3]) << '\n';
    } else if(strcmp(hamming_code::kValidationCommand, argv[1]) == 0){

        if (code.IsValid(std::atoi(argv[2]), argv[3])){
            std::cout << "1";
        } else{
            std::cout << "0";
        }

        return 0;
    } else{
        return 1;
    }
}