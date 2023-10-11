//---------------------------------------------------------------------------//
// Copyright (c) 2023 Alexey Yashunsky <a.yashunsky@nil.foundation>
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
// @file Declaration of interfaces for FRI verification array swapping component.
//---------------------------------------------------------------------------//

#ifndef CRYPTO3_BLUEPRINT_COMPONENTS_PLONK_FRI_ARRAY_SWAP_HPP
#define CRYPTO3_BLUEPRINT_COMPONENTS_PLONK_FRI_ARRAY_SWAP_HPP

#include <nil/crypto3/zk/snark/arithmetization/plonk/constraint_system.hpp>

#include <nil/blueprint/blueprint/plonk/assignment.hpp>
#include <nil/blueprint/blueprint/plonk/circuit.hpp>
#include <nil/blueprint/component.hpp>
#include <nil/blueprint/manifest.hpp>

namespace nil {
    namespace blueprint {
        namespace components {

            // Input: t, array <a1, b1, a2, b2, ..., an, bn> 
            // Output: <a1,b1,a2,b2,....,an,bn> if t == 0, <b1,a1,b2,a2,....,bn,an> if t == 1
            template<typename ArithmetizationType, typename FieldType>
            class fri_array_swap;

            template<typename BlueprintFieldType, typename ArithmetizationParams>
            class fri_array_swap<crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>,
                           BlueprintFieldType>
                : public plonk_component<BlueprintFieldType, ArithmetizationParams, 0, 0> {

            public:
                using component_type = plonk_component<BlueprintFieldType, ArithmetizationParams, 0, 0>;

                using var = typename component_type::var;
                using manifest_type = plonk_component_manifest;

                class gate_manifest_type : public component_gate_manifest {
                public:
                    std::uint32_t gates_amount() const override {
                    //    return fri_array_swap::gates_amount;
                    }
                };

                static gate_manifest get_gate_manifest(std::size_t witness_amount,
                                                       std::size_t lookup_column_amount) {
                    static gate_manifest manifest = gate_manifest(gate_manifest_type());
                    // return manifest;
                }

                static manifest_type get_manifest() {
                    static manifest_type manifest = manifest_type(
                        std::shared_ptr<manifest_param>(new manifest_single_value_param(4)),
                        false
                    );
                    // return manifest;
                }

                constexpr static std::size_t get_rows_amount(std::size_t witness_amount, std::size_t lookup_column_amount) {
                    // return 1;
                }

                constexpr static const std::size_t gates_amount = 1;
                const std::size_t rows_amount = get_rows_amount(this->witness_amount(), 0);

                struct input_type {
                    var t; // swap control bit
	            std::vector<var> arr; // the array with elements to swap

                    std::vector<var> all_vars() const {
                    //    return {x};
                    }
                };

                struct result_type {
		    std::vector<var> output = {}; // the array with possibly swapped elements

                    result_type(const fri_array_swap &component, std::uint32_t start_row_index) {
                    //    output = var(component.W(2), start_row_index, false, var::column_type::witness);
                    }

                    result_type(const fri_array_swap &component, std::size_t start_row_index) {
                    //    output = var(component.W(2), start_row_index, false, var::column_type::witness);
                    }

                    std::vector<var> all_vars() const {
                        return output;
                    }
                };

                template<typename ContainerType>
                explicit fri_array_swap(ContainerType witness, std::size_t n_, typename BlueprintFieldType::value_type omega_) : component_type(witness, {}, {}, get_manifest()) {};

                template<typename WitnessContainerType, typename ConstantContainerType,
                         typename PublicInputContainerType>
                fri_array_swap(WitnessContainerType witness, ConstantContainerType constant,
                         PublicInputContainerType public_input, std::size_t n_, typename BlueprintFieldType::value_type omega_) :
                    component_type(witness, constant, public_input, get_manifest()) {};

                fri_array_swap(std::initializer_list<typename component_type::witness_container_type::value_type> witnesses,
                         std::initializer_list<typename component_type::constant_container_type::value_type>
                             constants,
                         std::initializer_list<typename component_type::public_input_container_type::value_type>
                             public_inputs, std::size_t n_, typename BlueprintFieldType::value_type omega_) :
                    component_type(witnesses, constants, public_inputs, get_manifest()) {};
            };

            template<typename BlueprintFieldType, typename ArithmetizationParams>
            using plonk_fri_array_swap =
                division<crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>,
                         BlueprintFieldType>;

            template<typename BlueprintFieldType, typename ArithmetizationParams>
            typename plonk_fri_array_swap<BlueprintFieldType, ArithmetizationParams>::result_type generate_assignments(
                const plonk_fri_division<BlueprintFieldType, ArithmetizationParams> &component,
                assignment<crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>>
                    &assignment,
                const typename plonk_fri_array_swap<BlueprintFieldType, ArithmetizationParams>::input_type &instance_input,
                const std::uint32_t start_row_index) {
/*
                const std::size_t j = start_row_index;

                assignment.witness(component.W(0), j) = var_value(assignment, instance_input.x);
                assignment.witness(component.W(1), j) = var_value(assignment, instance_input.y);
                assignment.witness(component.W(2), j) =
                    var_value(assignment, instance_input.x) / var_value(assignment, instance_input.y);
                assignment.witness(component.W(3), j) = var_value(assignment, instance_input.y).inversed();

                return typename plonk_division<BlueprintFieldType, ArithmetizationParams>::result_type(
                    component, start_row_index);
*/
	    }

            template<typename BlueprintFieldType, typename ArithmetizationParams>
            std::size_t generate_gates(
                const plonk_fri_array_swap<BlueprintFieldType, ArithmetizationParams> &component,
                circuit<crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>> &bp,
                assignment<crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>>
                    &assignment,
                const typename plonk_fri_array_swap<BlueprintFieldType, ArithmetizationParams>::input_type &instance_input) {

                using var = typename plonk_fri_array_swap<BlueprintFieldType, ArithmetizationParams>::var;
/*
                auto constraint_1 = var(component.W(1), 0) * var(component.W(2), 0) - var(component.W(0), 0);
                auto constraint_2 = var(component.W(1), 0) * var(component.W(3), 0) - 1;

                return bp.add_gate({constraint_1, constraint_2});
*/		
            }

            template<typename BlueprintFieldType, typename ArithmetizationParams>
            void generate_copy_constraints(
                const plonk_fri_array_swap<BlueprintFieldType, ArithmetizationParams> &component,
                circuit<crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>> &bp,
                assignment<crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>>
                    &assignment,
                const typename plonk_fri_array_swap<BlueprintFieldType, ArithmetizationParams>::input_type &instance_input,
                const std::size_t start_row_index) {

                using var = typename plonk_fri_array_swap<BlueprintFieldType, ArithmetizationParams>::var;
/*
                const std::size_t j = start_row_index;
                var component_x = var(component.W(0), static_cast<int>(j), false);
                var component_y = var(component.W(1), static_cast<int>(j), false);
                bp.add_copy_constraint({instance_input.x, component_x});
                bp.add_copy_constraint({component_y, instance_input.y});
*/		
            }

            template<typename BlueprintFieldType, typename ArithmetizationParams>
            typename plonk_fri_array_swap<BlueprintFieldType, ArithmetizationParams>::result_type generate_circuit(
                const plonk_fri_array_swap<BlueprintFieldType, ArithmetizationParams> &component,
                circuit<crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>> &bp,
                assignment<crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType, ArithmetizationParams>>
                    &assignment,
                const typename plonk_fri_array_swap<BlueprintFieldType, ArithmetizationParams>::input_type &instance_input,
                const std::size_t start_row_index) {
/*
                std::size_t selector_index = generate_gates(component, bp, assignment, instance_input);

                assignment.enable_selector(selector_index, start_row_index);

                generate_copy_constraints(component, bp, assignment, instance_input, start_row_index);

                return typename plonk_division<BlueprintFieldType, ArithmetizationParams>::result_type(
                    component, start_row_index);
*/	
            }

        }    // namespace components
    }        // namespace blueprint
}    // namespace nil

#endif    // CRYPTO3_BLUEPRINT_COMPONENTS_PLONK_FRI_ARRAY_SWAP_HPP
