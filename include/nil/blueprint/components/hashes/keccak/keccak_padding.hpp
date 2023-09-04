//---------------------------------------------------------------------------//
// Copyright (c) 2023 Polina Chernyshova <pockvokhbtra@nil.foundation>
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

#ifndef CRYPTO3_BLUEPRINT_COMPONENTS_KECCAK_PER_CHUNK_HPP
#define CRYPTO3_BLUEPRINT_COMPONENTS_KECCAK_PER_CHUNK_HPP

#include <nil/crypto3/zk/snark/arithmetization/plonk/constraint_system.hpp>

#include <nil/blueprint/components/hashes/keccak/keccak_round.hpp>

namespace nil {
    namespace blueprint {
        namespace components {
            template<typename ArithmetizationType, std::uint32_t WitnessesAmount>
            class keccak_per_chunk;

            template<typename BlueprintFieldType, typename ArithmetizationParams, std::uint32_t WitnessesAmount>
            class keccak_per_chunk<crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType,
                                                                                 ArithmetizationParams>,
                                     WitnessesAmount> :
                public plonk_component<BlueprintFieldType, ArithmetizationParams, WitnessesAmount, 1, 0> {

                using component_type = plonk_component<BlueprintFieldType, ArithmetizationParams,
                                                       WitnessesAmount, 1, 0>;
                using value_type = typename BlueprintFieldType::value_type;

                std::size_t rows() const {
                    return full_configuration.back().last_coordinate.row + 1;
                } 

            public:

                struct configuration {
                    struct coordinates {
                        std::size_t row;
                        std::size_t column;

                        coordinates() = default;
                        coordinates(std::size_t row_, std::size_t column_) : row(row_), column(column_) {};
                        coordinates(std::pair<std::size_t, std::size_t> pair) : row(pair.first), column(pair.second) {};
                        bool operator== (const coordinates& other) const {
                            return row == other.row && column == other.column;
                        }
                        bool operator< (const coordinates& other) const {
                            return row < other.row || (row == other.row && column < other.column);
                        }
                    };
                    
                    // In constraints we use such notation: constr[0] - result,
                    // constr[1]... - arguments for lookup, linear elements for regular constraints in correct order.
                    coordinates first_coordinate;
                    coordinates last_coordinate;
                    std::vector<coordinates> copy_to;
                    std::vector<std::vector<coordinates>> constraints;
                    std::vector<std::vector<coordinates>> lookups;
                    coordinates copy_from;

                    configuration() = default;
                    configuration(std::pair<std::size_t, std::size_t> first_coordinate_,
                                  std::pair<std::size_t, std::size_t> last_coordinate_,
                                  std::vector<std::pair<std::size_t, std::size_t>> copy_to_,
                                  std::vector<std::vector<std::pair<std::size_t, std::size_t>>> constraints_,
                                  std::vector<std::vector<std::pair<std::size_t, std::size_t>>> lookups_,
                                  std::pair<std::size_t, std::size_t> copy_from_) {
                            first_coordinate = coordinates(first_coordinate_);
                            last_coordinate = coordinates(last_coordinate_);
                            for (std::size_t i = 0; i < copy_to_.size(); ++i) {
                                copy_to.push_back(coordinates(copy_to_[i]));
                            }
                            for (std::size_t i = 0; i < constraints_.size(); ++i) {
                                std::vector<coordinates> constr;
                                for (std::size_t j = 0; j < constraints_[i].size(); ++j) {
                                    constr.push_back(coordinates(constraints_[i][j]));
                                }
                                constraints.push_back(constr);
                            }
                            for (std::size_t i = 0; i < lookups_.size(); ++i) {
                                std::vector<coordinates> lookup;
                                for (std::size_t j = 0; j < lookups_[i].size(); ++j) {
                                    lookup.push_back(coordinates(lookups_[i][j]));
                                }
                                lookups.push_back(lookup);
                            }
                            copy_from = coordinates(copy_from_);
                        };
                    bool operator== (const configuration& other) const {
                        return first_coordinate == other.first_coordinate &&
                               last_coordinate == other.last_coordinate &&
                               copy_to == other.copy_to &&
                               constraints == other.constraints &&
                               lookups == other.lookups &&
                               copy_from == other.copy_from;
                    }
                    bool operator< (const configuration& other) const {
                        return first_coordinate < other.first_coordinate ||
                                (first_coordinate == other.first_coordinate && last_coordinate < other.last_coordinate);
                    }
                };

                struct padding_gate {
                    coordinates relay;
                    std::vector<coordinates> value;
                    std::vector<coordinates> sum;
                    std::vector<coordinates> range_check;
                    std::vector<coordinates> first;
                    std::vector<coordinates> second;

                    padding_gate() = default;
                    padding_gate(coordinates relay_, std::vector<coordinates> value_, std::vector<coordinates> sum_,
                                 std::vector<coordinates> range_check_, std::vector<coordinates> first_,
                                 std::vector<coordinates> second_) :
                        relay(relay_), value(value_), sum(sum_), range_check(range_check_), first(first_), second(second_) {};
                }

                using var = typename component_type::var;
                
                const std::size_t num_blocks;
                const std::size_t num_bits;
                const std::size_t bits_per_block = 64;
                std::size_t shift;

                const std::size_t limit_permutation_column = 7;

                const std::size_t rows_amount;
                std::vector<std::size_t> gates_rows;
                padding_gate last_gate;
                constexpr static const std::size_t gates_amount;

                struct input_type {
                    // initial message = message[0] * 2^(64 * (num_blocks - 1)) + ... + message[num_blocks - 2] * 2^64 + message[num_blocks - 1]
                    // all message[i] are 64-bit for i > 0
                    // message[0] is <= 64-bit
                    std::vector<var> message;
                };

                struct result_type {
                    std::vector<var> padded_message;

                    result_type(const keccak_per_chunk &component, std::size_t start_row_index) {
                        
                    }
                };

                std::size_t get_shift() {
                    return num_blocks * 64 - num_bits;
                }

                template<std::size_t WitnessesAmount>
                padding_gate padding() {
                    throw std::runtime_error("Unsupported number of witnesses");
                    return padding_gate();
                }
                template<9>
                padding_gate padding(std::size_t row = 0) {
                    padding_gate res;
                    res.relay = {-1 + row, 0};
                    res.value = {{-1 + row, 1}, {-1 + row, 3}, {0 + row, 0}, {0 + row, 2}, {1 + row, 0}};
                    res.sum = {{-1 + row, 2}, {-1 + row, 4}, {0 + row, 1}, {0 + row, 3}, {1 + row, 1}};
                    res.first = {{-1 + row, 5}, {-1 + row, 7}, {0 + row, 4}, {0 + row, 6}, {1 + row, 2}};
                    res.second = {{-1 + row, 6}, {-1 + row, 8}, {0 + row, 5}, {0 + row, 7}, {1 + row, 3}};
                    res.range_check = {{1 + row, 4}, {1 + row, 5}, {1 + row, 6}, {1 + row, 7}, {1 + row, 8}};
                    return res;
                }
                template<15>
                padding_gate padding(std::size_t row = 0) {
                    padding_gate res;
                    res.relay = {-1 + row, 11};
                    res.value = {{0 + row,0}, {0 + row,1}, {0 + row,2}, {1 + row,0}, {1 + row,1}, {1 + row,2}};
                    res.sum = {{0 + row,3}, {0 + row,4}, {0 + row,5}, {1 + row,3}, {1 + row,4}, {1 + row,5}};
                    res.first = {{0 + row,6}, {0 + row,7}, {0 + row,8}, {1 + row,6}, {1 + row,7}, {1 + row,8}};   
                    res.second = {{0 + row,9}, {0 + row,10}, {0 + row,11}, {1 + row,9}, {1 + row,10}, {1 + row,11}};
                    res.range_check = {{0 + row,12}, {0 + row,13}, {0 + row,14}, {1 + row,12}, {1 + row,13}, {1 + row,14}};
                    return res;
                }

                std::vector<configuration> configure_batching() {
                    std::vector<configuration> result;

                    std::size_t i = 0;
                    std::size_t row = 1;
                    for (std::size_t j = 0; j < num_blocks; ++j) {
                        auto pg = padding<WitnessesAmount>(row);
                        congifuration conf;
                        if (i == 0) {
                            conf.last_coordinate = pg.second[0];
                            conf.copy_to = {pg.relay, pg.value[0]};
                            conf.constraints = {{pg.value[0], pg.first[0], pg.second[0]},
                                                {pg.sum[0], pg.relay, pg.first[0]},
                                                {pg.range_check[0], pg.relay}};
                            conf.lookups = {pg.range_check[0]};
                            conf.copy_from = pg.sum[0];
                            result.push_back(conf);
                            continue;
                        }
                        conf.last_coordinate = pg.second[i];
                        conf.copy_to = {pg.value[i]};
                        conf.constraints = {{pg.value[i], pg.first[i], pg.second[i]},
                                            {pg.sum[i], pg.second[i-1], pg.first[i]},
                                            {pg.range_check[i], pg.second[i-1]}};
                        conf.lookups = {pg.range_check[i]};
                        conf.copy_from = pg.sum[i];
                        result.push_back(conf);
                        i = (i + 1) % 5;
                        if (i == 0) {
                            if (WitnessesAmount == 9) row += 3;
                            if (WitnessesAmount == 15) row += 2;
                        }
                    }

                    return result;
                }

                configuration configure_padding(std::size_t row, std::size_t column) {
                    // costraint with 0
                    if (column >= limit_permutation_column) {
                        return configuration({row + 1, 1}, {{row + 1, 0}}, {}, {}, {row + 1, 0});
                    }
                    auto last_column = column + 1;
                    auto last_row = row + (last_column / WitnessesAmount);
                    last_column %= WitnessesAmount;
                    return configuration({last_row, last_column}, {{row, column}}, {}, {}, {row, column});
                }

                std::vector<configuration> configure_all() {
                    std::vector<configuration> result;
                    
                    if (WitnessesAmount % 15 == 0) {
                        configuration conf0;
                        conf0.copy_to = {{0, 0}, {0, 1}};
                        conf0.constraints = {{{0, 1}, {0, 5}, {0, 7}},
                                                {{0, 3}, {0, 0}, {0, 5}},
                                                {{0, 8}, {0, 0}}};
                        conf0.lookups = {{0, 8}};
                        conf0.copy_from = {0, 3};
                        result.push_back(conf0);
                        configuration conf1;
                        conf1.copy_to = {{0, 2}};
                        conf1.constraints = {{{0, 2}, {0, 6}, {0, 11}},
                                                {{0, 4}, {0, 7}, {0, 6}},
                                                {{0, 9}, {0, 7}}};
                        conf1.lookups = {{0, 9}};
                        conf1.copy_from = {0, 4};
                        result.push_back(conf1);
                    }

                    auto batch_configs = configure_batching();
                    result.insert(result.end(), batch_configs.begin(), batch_configs.end());

                    auto row = result.back().last_coordinate.row + 1;
                    std::size_t column = 0;
                    std::size_t size = result.size();

                    while (size % 17 != 0) {
                        result.push_back(configure_padding(row, column));
                        row = result.back().last_row;
                        column = result.back().last_column;
                        size++;
                    }
                    return result;
                }

                #define __keccak_padding_init_macro(lookup_rows_, lookup_columns_, num_blocks_, num_bits_) \
                    lookup_rows(lookup_rows_), \
                    lookup_columns(lookup_columns_), \
                    num_blocks(num_blocks_), \
                    num_bits(num_bits_), \
                    shift(get_shift()), \
                    full_configuration(configure_all()), \
                    rows_amount(rows()), \
                    gates_rows(calculate_gates_rows()), \
                    last_gate(calculate_last_gate()), \
                    gates_amount(gates())

                template<typename ContainerType>
                keccak_per_chunk(ContainerType witness, std::size_t lookup_rows_, std::size_t lookup_columns_,
                                                        std::size_t num_blocks_, std::size_t num_bits_) :
                    component_type(witness, {}, {}),
                    __keccak_per_chunk_init_macro(lookup_rows_, lookup_columns_, num_blocks_, num_bits_) {};


                template<typename WitnessContainerType, typename ConstantContainerType,
                         typename PublicInputContainerType>
                keccak_per_chunk(WitnessContainerType witness, ConstantContainerType constant,
                                   PublicInputContainerType public_input,
                                   std::size_t lookup_rows_, std::size_t lookup_columns_,
                                   std::size_t num_blocks_ std::size_t num_bits_):
                    component_type(witness, constant, public_input),
                    __keccak_per_chunk_init_macro(lookup_rows_, lookup_columns_, num_blocks_, num_bits_) {};

                keccak_per_chunk(
                    std::initializer_list<typename component_type::witness_container_type::value_type> witnesses,
                    std::initializer_list<typename component_type::constant_container_type::value_type> constants,
                    std::initializer_list<typename component_type::public_input_container_type::value_type>
                        public_inputs,
                    std::size_t lookup_rows_, std::size_t lookup_columns_,
                    std::size_t num_blocks_, std::size_t num_bits_) :
                        component_type(witnesses, constants, public_inputs),
                        __keccak_per_chunk_init_macro(lookup_rows_, lookup_columns_, num_blocks_, num_bits_)
                {};

                #undef __keccak_padding_init_macro
            };

            template<typename BlueprintFieldType, typename ArithmetizationParams, std::uint32_t WitnessesAmount>
            using keccak_pad_component =
                keccak_padding<crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType,
                                                                               ArithmetizationParams>,
                                   WitnessesAmount>;

            template<typename BlueprintFieldType, typename ArithmetizationParams, std::uint32_t WitnessesAmount,
                     std::enable_if_t<WitnessesAmount >= 9, bool> = true>
            void generate_gates(
                const keccak_pad_component<BlueprintFieldType, ArithmetizationParams,
                                               WitnessesAmount>
                    &component,
                circuit<crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType,
                                                                    ArithmetizationParams>>
                    &bp,
                assignment<crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType,
                                                                       ArithmetizationParams>>
                    &assignment,
                const typename keccak_pad_component<BlueprintFieldType, ArithmetizationParams,
                                                        WitnessesAmount>::input_type
                    &instance_input,
                const std::size_t first_selector_index) {
                    
                using component_type = keccak_pad_component<BlueprintFieldType, ArithmetizationParams,
                                                                WitnessesAmount>;
                using var = typename component_type::var;
                using constraint_type = crypto3::zk::snark::plonk_constraint<BlueprintFieldType>;
                using gate_type = typename crypto3::zk::snark::plonk_gate<BlueprintFieldType, constraint_type>;
                using value_type = typename BlueprintFieldType::value_type;
                using integral_type = typename BlueprintFieldType::integral_type;

                
                auto config = component.full_configuration;
                auto gate_config = component.gates_configuration;
                // auto lookup_gate_config = component.lookup_gates_configuration;
                std::size_t config_index = 0;
                std::size_t gate_index = 0;
                // std::size_t lookup_gate_index = 0;

                std::vector<constraint_type> constraints;
                // std::vector<lookup_constraint_type> lookup_constraints;

                // batching
                if (component.shift > 0) {
                    for (int i = 0; i < component.num_block; ++i) {
                        auto cur_config = config[config_index];
                        constraints.push_back(bp.add_constraint(var(cur_config.constraints[0][0].column, cur_config.constraints[0][0].row - gate_config[gate_index])
                                                              - var(cur_config.constraints[0][1].column, cur_config.constraints[0][1].row - gate_config[gate_index]) * (integral_type(1) << component.shift)
                                                              - var(cur_config.constraints[0][2].column, cur_config.constraints[0][2].row - gate_config[gate_index])));
                        gate_index++;
                        constraints.push_back(bp.add_constraint(var(cur_config.constraints[1][1].column, cur_config.constraints[1][1].row - gate_config[gate_index])
                                                            - (integral_type(1) << (64 - component.shift))
                                                            + (integral_type(1) << 64)
                                                            - var(cur_config.constraints[1][0].column, cur_config.constraints[1][0].row - gate_config[gate_index])));
                        gate_index++;
                        if (i > 0) {
                            constraints.push_back(bp.add_constraint(var(cur_config.constraints[2][1].column, cur_config.constraints[2][1].row - gate_config[gate_index])
                                                                + var(cur_config.constraints[2][2].column, cur_config.constraints[2][2].row - gate_config[gate_index]) 
                                                                - var(cur_config.constraints[2][0].column, cur_config.constraints[2][0].row - gate_config[gate_index])));
                            gate_index++;
                        }
                        config_index++;
                    }
                    
                    // padding
                    {
                        auto cur_config = config[config_index];
                        constraints.push_back(bp.add_constraint(var(cur_config.constraints[0][0].column, cur_config.constraints[0][0].row - gate_config[gate_index])
                                                              - var(cur_config.constraints[0][1].column, cur_config.constraints[0][1].row - gate_config[gate_index]) * (integral_type(1) << component.shift)));
                    }
                }
            }

            template<typename BlueprintFieldType, typename ArithmetizationParams, std::uint32_t WitnessesAmount,
                     std::enable_if_t<WitnessesAmount >= 9, bool> = true>
            void generate_copy_constraints(
                const keccak_pad_component<BlueprintFieldType, ArithmetizationParams,
                                               WitnessesAmount>
                    &component,
                circuit<crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType,
                                                                    ArithmetizationParams>>
                    &bp,
                assignment<crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType,
                                                                       ArithmetizationParams>>
                    &assignment,
                const typename keccak_pad_component<BlueprintFieldType, ArithmetizationParams,
                                                        WitnessesAmount>::input_type
                    &instance_input,
                const std::uint32_t start_row_index) {

                using component_type = keccak_pad_component<BlueprintFieldType, ArithmetizationParams,
                                                                WitnessesAmount>;
                using var = typename component_type::var;

                std::size_t config_index = 0;
                if (component.shift > 0) {
                    config_index += component.num_blocks + 1;
                }

                while (congif_index < component.full_configuration.size()) {
                    bp.add_copy_constraint({component.C(0), var(component.W(config[i].copy_to[0].row), config[i].copy_to[0].column, false)});
                    config_index++;
                }
            }

            template<typename BlueprintFieldType, typename ArithmetizationParams, std::uint32_t WitnessesAmount,
                     std::enable_if_t<WitnessesAmount >= 9, bool> = true>
            typename keccak_pad_component<BlueprintFieldType, ArithmetizationParams,
                                              WitnessesAmount>::result_type
            generate_circuit(
                const keccak_pad_component<BlueprintFieldType, ArithmetizationParams,
                                               WitnessesAmount>
                    &component,
                circuit<crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType,
                                                                    ArithmetizationParams>>
                    &bp,
                assignment<crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType,
                                                                       ArithmetizationParams>>
                    &assignment,
                const typename keccak_pad_component<BlueprintFieldType, ArithmetizationParams,
                                                        WitnessesAmount>::input_type
                    &instance_input,
                const std::uint32_t start_row_index) {

                auto selector_iterator = assignment.find_selector(component);
                std::size_t first_selector_index;
                std::size_t row = start_row_index;

                using component_type = keccak_pad_component<BlueprintFieldType, ArithmetizationParams,
                                                                WitnessesAmount>;
                using var = typename component_type::var;
                

                BOOST_ASSERT(row == start_row_index + component.rows_amount);

                generate_copy_constraints(component, bp, assignment, instance_input, start_row_index);
                generate_assignments_constants(component, bp, assignment, instance_input, start_row_index);

                return typename component_type::result_type(component, start_row_index);
            }

            template<typename BlueprintFieldType, typename ArithmetizationParams, std::uint32_t WitnessesAmount,
                     std::enable_if_t<WitnessesAmount >= 9, bool> = true>
            typename keccak_pad_component<BlueprintFieldType, ArithmetizationParams,
                                              WitnessesAmount>::result_type
            generate_assignments(
                const keccak_pad_component<BlueprintFieldType, ArithmetizationParams,
                                               WitnessesAmount>
                    &component,
                assignment<crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType,
                                                                       ArithmetizationParams>>
                    &assignment,
                const typename keccak_pad_component<BlueprintFieldType, ArithmetizationParams,
                                                        WitnessesAmount>::input_type
                    &instance_input,
                const std::uint32_t start_row_index) {

                std::size_t row = start_row_index;

                using component_type = keccak_pad_component<BlueprintFieldType, ArithmetizationParams,
                                                                WitnessesAmount>;
                using value_type = typename BlueprintFieldType::value_type;
                using integral_type = typename BlueprintFieldType::integral_type;

                std::size_t config_index = 0;

                // batching
                if (component.shift != 0) {
                    integral_type relay_chunk = integral_type(var_value(assignment, instance_input.message[0]));
                    for (std::size_t index = 1; index < component.num_blocks; ++index) {
                        value_type chunk = var_value(assignment, instance_input.message[index]);
                        integral_type integral_chunk = integral_type(chunk);
                        integral_type mask = (integral_type(1) << component.shift) - 1;
                        std::array<integral_type, 2> chunk_parts = {integral_chunk >> component.shift, integral_chunk & mask};
                        integral_type first_chunk = (relay_chunk << (64 - component.shift)) + chunk_parts[0];
                        integral_type relay_range_check = relay_chunk - (1 << component.shift) + (1 << 64);

                        auto cur_config = component.full_configuration[config_index];
                        assignment.witness(component.W(cur_config.constraints[0][0].row), cur_config.constraints[0][0].column) = chunk;
                        for (int j = 1; j < 3; ++j) {
                            assignment.witness(component.W(cur_config.constraints[0][j].row), cur_config.constraints[0][j].column) = value_type(chunk_parts[j - 1]);
                        }
                        assignment.witness(component.W(cur_config.constraints[1][0].row), cur_config.constraints[1][0].column) = value_type(relay_range_check);
                        assignment.witness(component.W(cur_config.constraints[2][0].row), cur_config.constraints[2][0].column) = value_type(first_chunk);
                        
                        relay_chunk = chunk_parts[1];
                        config_index++;
                    }
                    // padding
                    {
                        integral_type last_chunk = relay_chunk << (64 - component.shift);
                        integral_type relay_range_check = relay_chunk - (1 << component.shift) + (1 << 64);

                        auto cur_config = component.full_configuration[config_index];
                        assignment.witness(component.W(cur_config.copy_to[0].row), cur_config.copy_to[0].column) = value_type(relay_chunk);
                        assignment.witness(component.W(cur_config.constraints[0][0].row), cur_config.constraints[0][0].column) = value_type(last_chunk);
                        assignment.witness(component.W(cur_config.constraints[1][0].row), cur_config.constraints[1][0].column) = value_type(relay_range_check);
                        config_index++;
                    }
                }
                
                while (congif_index < component.full_configuration.size()) {
                    auto cur_config = component.full_configuration[config_index];
                    assignment.witness(component.W(cur_config.copy_to[0].row), cur_config.copy_to[0].column) = 0;
                    config_index++;
                }

                return typename component_type::result_type(component, start_row_index);
            }

            template<typename BlueprintFieldType, typename ArithmetizationParams, std::uint32_t WitnessesAmount,
                     std::enable_if_t<WitnessesAmount >= 9, bool> = true>
            void generate_assignments_constant(
                const keccak_pad_component<BlueprintFieldType, ArithmetizationParams,
                                               WitnessesAmount>
                    &component,
                circuit<crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType,
                                                                    ArithmetizationParams>>
                    &bp,
                assignment<crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType,
                                                                       ArithmetizationParams>>
                    &assignment,
                const typename keccak_pad_component<BlueprintFieldType, ArithmetizationParams,
                                                        WitnessesAmount>::input_type
                    &instance_input,
                const std::uint32_t start_row_index) {

                using component_type = keccak_pad_component<BlueprintFieldType, ArithmetizationParams,
                                                                WitnessesAmount>;

                assignment.constant(component.C(0), start_row_index) = 0;
            }

        
        }    // namespace components
    }        // namespace blueprint
}   // namespace nil

#endif  // CRYPTO3_BLUEPRINT_COMPONENTS_KECCAK_PER_CHUNK_HPP