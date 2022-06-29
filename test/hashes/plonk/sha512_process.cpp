//---------------------------------------------------------------------------//
// Copyright (c) 2022 Alisa Cherniaeva <a.cherniaeva@nil.foundation>
//
// MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//---------------------------------------------------------------------------//

#define BOOST_TEST_MODULE plonk_sha512_test

#include <boost/test/unit_test.hpp>

#include <nil/crypto3/algebra/curves/pallas.hpp>
#include <nil/crypto3/algebra/fields/arithmetic_params/pallas.hpp>
#include <nil/crypto3/algebra/random_element.hpp>

#include <nil/crypto3/hash/algorithm/hash.hpp>
#include <nil/crypto3/hash/sha2.hpp>
#include <nil/crypto3/hash/keccak.hpp>

#include <nil/crypto3/zk/snark/arithmetization/plonk/params.hpp>

#include <nil/crypto3/zk/blueprint/plonk.hpp>
#include <nil/crypto3/zk/assignment/plonk.hpp>
#include <nil/crypto3/zk/components/hashes/sha256/plonk/sha512_process.hpp>

#include "../../test_plonk_component.hpp"

using namespace nil::crypto3;

BOOST_AUTO_TEST_SUITE(blueprint_plonk_test_suite)

BOOST_AUTO_TEST_CASE(blueprint_plonk_sha512_process) {

    using curve_type = algebra::curves::pallas;
    using BlueprintFieldType = typename curve_type::base_field_type;
    constexpr std::size_t WitnessColumns = 9;
    constexpr std::size_t PublicInputColumns = 1;
    constexpr std::size_t ConstantColumns = 1;
    constexpr std::size_t SelectorColumns = 3;
    using hash_type = nil::crypto3::hashes::keccak_1600<256>;
    constexpr std::size_t Lambda = 1;

    using ArithmetizationParams = zk::snark::plonk_arithmetization_params<WitnessColumns,
        PublicInputColumns, ConstantColumns, SelectorColumns>;
    using ArithmetizationType = zk::snark::plonk_constraint_system<BlueprintFieldType,
                ArithmetizationParams>;
    using var = zk::snark::plonk_variable<BlueprintFieldType>;
    using AssignmentType = zk::blueprint_assignment_table<ArithmetizationType>;

    using component_type = zk::components::sha512_process<ArithmetizationType, curve_type,
                                                            0, 1, 2, 3, 4, 5, 6, 7, 8>;
    typename BlueprintFieldType::value_type s = typename BlueprintFieldType::value_type(2).pow(59);
    std::array<typename ArithmetizationType::field_type::value_type, 24> public_input = {0x6a09e667f3bcc908_cppui64, 0xbb67ae8584caa73b_cppui64,
     0x3c6ef372fe94f82b_cppui64, 0xa54ff53a5f1d36f1_cppui64, 0x510e527fade682d1_cppui64, 0x9b05688c2b3e6c1f_cppui64,
     0x1f83d9abfb41bd6b_cppui64, 0x5be0cd19137e2179_cppui64, s - 5, s + 5, s - 6, s + 6, s - 7, s + 7, s - 8, s + 8, s - 9, s + 9, s + 10, s - 10, s + 11, s - 11, s + 12, s - 12};
    std::array<var, 8> input_state_var = {var(0, 0, false, var::column_type::public_input),
     var(0, 1, false, var::column_type::public_input), var(0, 2, false, var::column_type::public_input), var(0, 3, false, var::column_type::public_input),
     var(0, 4, false, var::column_type::public_input), var(0, 5, false, var::column_type::public_input), var(0, 6, false, var::column_type::public_input),
     var(0, 7, false, var::column_type::public_input)};

     std::array<typename BlueprintFieldType::value_type, 80>
                        round_constant = {
                            0x428a2f98d728ae22_cppui64, 0x7137449123ef65cd_cppui64, 0xb5c0fbcfec4d3b2f_cppui64, 0xe9b5dba58189dbbc_cppui64,
                            0x3956c25bf348b538_cppui64, 0x59f111f1b605d019_cppui64, 0x923f82a4af194f9b_cppui64, 0xab1c5ed5da6d8118_cppui64,
                            0xd807aa98a3030242_cppui64, 0x12835b0145706fbe_cppui64, 0x243185be4ee4b28c_cppui64, 0x550c7dc3d5ffb4e2_cppui64,
                            0x72be5d74f27b896f_cppui64, 0x80deb1fe3b1696b1_cppui64, 0x9bdc06a725c71235_cppui64, 0xc19bf174cf692694_cppui64,
                            0xe49b69c19ef14ad2_cppui64, 0xefbe4786384f25e3_cppui64, 0x0fc19dc68b8cd5b5_cppui64, 0x240ca1cc77ac9c65_cppui64,
                            0x2de92c6f592b0275_cppui64, 0x4a7484aa6ea6e483_cppui64, 0x5cb0a9dcbd41fbd4_cppui64, 0x76f988da831153b5_cppui64,
                            0x983e5152ee66dfab_cppui64, 0xa831c66d2db43210_cppui64, 0xb00327c898fb213f_cppui64, 0xbf597fc7beef0ee4_cppui64,
                            0xc6e00bf33da88fc2_cppui64, 0xd5a79147930aa725_cppui64, 0x06ca6351e003826f_cppui64, 0x142929670a0e6e70_cppui64,
                            0x27b70a8546d22ffc_cppui64, 0x2e1b21385c26c926_cppui64, 0x4d2c6dfc5ac42aed_cppui64, 0x53380d139d95b3df_cppui64,
                            0x650a73548baf63de_cppui64, 0x766a0abb3c77b2a8_cppui64, 0x81c2c92e47edaee6_cppui64, 0x92722c851482353b_cppui64,
                            0xa2bfe8a14cf10364_cppui64, 0xa81a664bbc423001_cppui64, 0xc24b8b70d0f89791_cppui64, 0xc76c51a30654be30_cppui64,
                            0xd192e819d6ef5218_cppui64, 0xd69906245565a910_cppui64, 0xf40e35855771202a_cppui64, 0x106aa07032bbd1b8_cppui64,
                            0x19a4c116b8d2d0c8_cppui64, 0x1e376c085141ab53_cppui64, 0x2748774cdf8eeb99_cppui64, 0x34b0bcb5e19b48a8_cppui64,
                            0x391c0cb3c5c95a63_cppui64, 0x4ed8aa4ae3418acb_cppui64, 0x5b9cca4f7763e373_cppui64, 0x682e6ff3d6b2b8a3_cppui64,
                            0x748f82ee5defb2fc_cppui64, 0x78a5636f43172f60_cppui64, 0x84c87814a1f0ab72_cppui64, 0x8cc702081a6439ec_cppui64,
                            0x90befffa23631e28_cppui64, 0xa4506cebde82bde9_cppui64, 0xbef9a3f7b2c67915_cppui64, 0xc67178f2e372532b_cppui64,
                            0xca273eceea26619c_cppui64, 0xd186b8c721c0c207_cppui64, 0xeada7dd6cde0eb1e_cppui64, 0xf57d4f7fee6ed178_cppui64,
                            0x06f067aa72176fba_cppui64, 0x0a637dc5a2c898a6_cppui64, 0x113f9804bef90dae_cppui64, 0x1b710b35131c471b_cppui64,
                            0x28db77f523047d84_cppui64, 0x32caab7b40c72493_cppui64, 0x3c9ebe0a15c9bebc_cppui64, 0x431d67c49c100d4c_cppui64,
                            0x4cc5d4becb3e42b6_cppui64, 0x597f299cfc657e2a_cppui64, 0x5fcb6fab3ad6faec_cppui64, 0x6c44198c4a475817_cppui64};

    std::array<var, 16> input_words_var;
    for (int i = 0; i<16; i++) {
        input_words_var[i]= var(0, 8 + i, false, var::column_type::public_input);
    }
    /*std::array<typename BlueprintFieldType::integral_type, 64> message_schedule_array;
    for(std::size_t i = 0; i < 16; i++){
        message_schedule_array[i] = typename BlueprintFieldType::integral_type(public_input[8 + i].data);
    }
    for(std::size_t i = 16; i < 64; i ++){
        typename BlueprintFieldType::integral_type s0 = ((message_schedule_array[i - 15] >> 7)|((message_schedule_array[i - 15] << (32 - 7)) 
        & typename BlueprintFieldType::integral_type((typename BlueprintFieldType::value_type(2).pow(32) - 1).data))) ^
        ((message_schedule_array[i - 15] >> 18)|((message_schedule_array[i - 15] << (32 - 18))
        & typename BlueprintFieldType::integral_type((typename BlueprintFieldType::value_type(2).pow(32) - 1).data)))
         ^ (message_schedule_array[i - 15] >> 3);
        typename BlueprintFieldType::integral_type s1 = ((message_schedule_array[i - 2] >> 17)|((message_schedule_array[i - 2] << (32 - 17))
        & typename BlueprintFieldType::integral_type((typename BlueprintFieldType::value_type(2).pow(32) - 1).data))) ^
        ((message_schedule_array[i - 2] >> 19)|((message_schedule_array[i - 2] << (32 - 19))
        & typename BlueprintFieldType::integral_type((typename BlueprintFieldType::value_type(2).pow(32) - 1).data)))
         ^ (message_schedule_array[i - 2] >> 10);
        message_schedule_array[i] = (message_schedule_array[i - 16] + s0 + s1 + message_schedule_array[i - 7])%
        typename curve_type::base_field_type::integral_type(typename curve_type::base_field_type::value_type(2).pow(32).data); 
    }
    typename ArithmetizationType::field_type::integral_type a = typename ArithmetizationType::field_type::integral_type(public_input[0].data);
    typename ArithmetizationType::field_type::integral_type b = typename ArithmetizationType::field_type::integral_type(public_input[1].data);
    typename ArithmetizationType::field_type::integral_type c = typename ArithmetizationType::field_type::integral_type(public_input[2].data);
    typename ArithmetizationType::field_type::integral_type d = typename ArithmetizationType::field_type::integral_type(public_input[3].data);
    typename ArithmetizationType::field_type::integral_type e = typename ArithmetizationType::field_type::integral_type(public_input[4].data);
    typename ArithmetizationType::field_type::integral_type f = typename ArithmetizationType::field_type::integral_type(public_input[5].data);
    typename ArithmetizationType::field_type::integral_type g = typename ArithmetizationType::field_type::integral_type(public_input[6].data);
    typename ArithmetizationType::field_type::integral_type h = typename ArithmetizationType::field_type::integral_type(public_input[7].data);
    for(std::size_t i = 0; i < 64; i ++){
        typename BlueprintFieldType::integral_type S0 = ((a >> 2)|((a << (32 - 2)) 
        & typename BlueprintFieldType::integral_type((typename BlueprintFieldType::value_type(2).pow(32) - 1).data))) ^
        ((a >> 13)|((a << (32 - 13))
        & typename BlueprintFieldType::integral_type((typename BlueprintFieldType::value_type(2).pow(32) - 1).data)))
         ^ ((a >> 22)|((a << (32 - 22))
        & typename BlueprintFieldType::integral_type((typename BlueprintFieldType::value_type(2).pow(32) - 1).data)));
        typename BlueprintFieldType::integral_type S1 = ((e >> 6)|((e << (32 - 6)) 
        & typename BlueprintFieldType::integral_type((typename BlueprintFieldType::value_type(2).pow(32) - 1).data))) ^
        ((e >> 11)|((e << (32 - 11))
        & typename BlueprintFieldType::integral_type((typename BlueprintFieldType::value_type(2).pow(32) - 1).data)))
         ^ ((e >> 25)|((e << (32 - 25))
        & typename BlueprintFieldType::integral_type((typename BlueprintFieldType::value_type(2).pow(32) - 1).data)));
        typename BlueprintFieldType::integral_type maj = (a & b) ^ (a & c) ^ (b & c);
        typename BlueprintFieldType::integral_type ch = (e & f) ^ ((~e)& g);

        std::vector<bool> e_bits(32);
        for (std::size_t j = 0; j < 32; j++) {
            e_bits[32 - j - 1] = multiprecision::bit_test(e, j);
        }
        std::vector<bool> f_bits(32);
        for (std::size_t j = 0; j < 32; j++) {
            f_bits[32 - j - 1] = multiprecision::bit_test(f, j);
        }
        std::vector<bool> g_bits(32);
        for (std::size_t j = 0; j < 32; j++) {
            g_bits[32 - j - 1] = multiprecision::bit_test(g, j);
        }
        std::vector<std::size_t> sizes = {32};
        std::size_t base = 7;
        std::array<std::vector<typename curve_type::base_field_type::integral_type>, 2> e_s = 
        component_type::split_and_sparse(e_bits, sizes, base);

        std::array<std::vector<typename curve_type::base_field_type::integral_type>, 2> f_s =
        component_type::split_and_sparse(f_bits, sizes, base);

        std::array<std::vector<typename curve_type::base_field_type::integral_type>, 2> g_s =
        component_type::split_and_sparse(g_bits, sizes, base);
        typename BlueprintFieldType::integral_type tmp1 = h + S1 + ch + round_constant[i] + message_schedule_array[i];
        typename BlueprintFieldType::integral_type tmp2 = S0 + maj;
        h = g;
        g = f;
        f = e;
        e = (d + tmp1)%
        typename curve_type::base_field_type::integral_type(typename curve_type::base_field_type::value_type(2).pow(32).data);
        d = c;
        c = b;
        b = a;
        a = (tmp1 + tmp2)%
        typename curve_type::base_field_type::integral_type(typename curve_type::base_field_type::value_type(2).pow(32).data);
    }
    std::array<typename BlueprintFieldType::integral_type, 8> result_state = {(a + typename ArithmetizationType::field_type::integral_type(public_input[0].data))%
        typename curve_type::base_field_type::integral_type(typename curve_type::base_field_type::value_type(2).pow(32).data),
    (b + typename ArithmetizationType::field_type::integral_type(public_input[1].data))%
        typename curve_type::base_field_type::integral_type(typename curve_type::base_field_type::value_type(2).pow(32).data), 
    (c + typename ArithmetizationType::field_type::integral_type(public_input[2].data))%
        typename curve_type::base_field_type::integral_type(typename curve_type::base_field_type::value_type(2).pow(32).data),
    (d + typename ArithmetizationType::field_type::integral_type(public_input[3].data))%
        typename curve_type::base_field_type::integral_type(typename curve_type::base_field_type::value_type(2).pow(32).data),
    (e + typename ArithmetizationType::field_type::integral_type(public_input[4].data))%
        typename curve_type::base_field_type::integral_type(typename curve_type::base_field_type::value_type(2).pow(32).data),
    (f + typename ArithmetizationType::field_type::integral_type(public_input[5].data))%
        typename curve_type::base_field_type::integral_type(typename curve_type::base_field_type::value_type(2).pow(32).data),
    (g + typename ArithmetizationType::field_type::integral_type(public_input[6].data))%
        typename curve_type::base_field_type::integral_type(typename curve_type::base_field_type::value_type(2).pow(32).data),
    (h + typename ArithmetizationType::field_type::integral_type(public_input[7].data))%
        typename curve_type::base_field_type::integral_type(typename curve_type::base_field_type::value_type(2).pow(32).data)};*/
    auto result_check = [](AssignmentType &assignment, 
        component_type::result_type &real_res) {
            /*for (std::size_t i = 0; i < 8; i++) {
                assert(result_state[i] == typename ArithmetizationType::field_type::integral_type(assignment.var_value(real_res.output_state[i]).data)); 
            }*/
    };
    typename component_type::params_type params = {input_state_var, input_words_var};
    test_component<component_type, BlueprintFieldType, ArithmetizationParams, hash_type, Lambda> (params, public_input, result_check);
}

BOOST_AUTO_TEST_SUITE_END()