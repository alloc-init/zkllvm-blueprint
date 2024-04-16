//---------------------------------------------------------------------------//
// Copyright (c) 2021 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2021 Nikita Kaskov <nbering@nil.foundation>
// Copyright (c) 2022 Ilia Shirobokov <i.shirobokov@nil.foundation>
// Copyright (c) 2022 Alisa Cherniaeva <a.cherniaeva@nil.foundation>
// Copyright (c) 2022 Polina Chernyshova <pockvokhbtra@nil.foundation>
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
// @file Declaration of interfaces for PLONK field element division component.
// If divider is zero, component's result is zero either.
//---------------------------------------------------------------------------//

#ifndef CRYPTO3_BLUEPRINT_COMPONENTS_PLONK_FIELD_DIVISION_OR_ZERO_HPP
#define CRYPTO3_BLUEPRINT_COMPONENTS_PLONK_FIELD_DIVISION_OR_ZERO_HPP

#include <nil/crypto3/zk/snark/arithmetization/plonk/constraint_system.hpp>

#include <nil/blueprint/blueprint/plonk/assignment.hpp>
#include <nil/blueprint/blueprint/plonk/circuit.hpp>
#include <nil/blueprint/component.hpp>
#include <nil/blueprint/manifest.hpp>

namespace nil {
    namespace blueprint {
        namespace components {

            // Input: x, y \in Fp
            // Output: z = x / y, if y != 0, else 0 z \in F_p
            template<typename ArithmetizationType, typename FieldType>
            class division_or_zero;

            template<typename BlueprintFieldType>
            class division_or_zero<crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType>,
                                   BlueprintFieldType>
                : public plonk_component<BlueprintFieldType> {

            public:
                using component_type = plonk_component<BlueprintFieldType>;

                class gate_manifest_type : public component_gate_manifest {
                public:
                    std::uint32_t gates_amount() const override {
                        return division_or_zero::gates_amount;
                    }
                };

                static gate_manifest get_gate_manifest(std::size_t witness_amount) {
                    static gate_manifest manifest = gate_manifest(gate_manifest_type());
                    return manifest;
                }

                constexpr static std::size_t get_rows_amount(std::size_t witness_amount) {
                    return 1;
                }
                constexpr static std::size_t get_empty_rows_amount() {
                    return 1;
                }

                constexpr static const std::size_t gates_amount = 1;
                const std::size_t rows_amount = get_rows_amount(this->witness_amount());
                const std::size_t empty_rows_amount = get_empty_rows_amount();
                const std::string component_name = "native field division or zero";

                using var = typename component_type::var;
                using manifest_type = plonk_component_manifest;

                static manifest_type get_manifest() {
                    static manifest_type manifest = manifest_type(
                        std::shared_ptr<manifest_param>(new manifest_single_value_param(5)),
                        false
                    );
                    return manifest;
                }

                struct input_type {
                    var x = var(0, 0, false);
                    var y = var(0, 0, false);

                    input_type(const std::vector<var>& input_vect) {
                        if (input_vect.size() != 2) {
                            throw std::out_of_range("Vector size does not match input size");
                        }
                        x = input_vect[0];
                        y = input_vect[1];
                    }

                    std::vector<std::reference_wrapper<var>> all_vars() {
                        return {x, y};
                    }
                };

                struct result_type {
                    var output = var(0, 0, false);
                    result_type(const division_or_zero &component, std::uint32_t start_row_index) {
                        output = var(component.W(2), start_row_index, false, var::column_type::witness);
                    }

                    std::vector<std::reference_wrapper<var>> all_vars() {
                        return {output};
                    }
                };

                template <typename ContainerType>
                division_or_zero(ContainerType witness):
                    component_type(witness, {}, {}, get_manifest()){};

                template <typename WitnessContainerType, typename ConstantContainerType,
                    typename PublicInputContainerType>
                division_or_zero(WitnessContainerType witness, ConstantContainerType constant,
                        PublicInputContainerType public_input):
                    component_type(witness, constant, public_input, get_manifest()){};

                division_or_zero(std::initializer_list<
                        typename component_type::witness_container_type::value_type> witnesses,
                               std::initializer_list<
                        typename component_type::constant_container_type::value_type> constants,
                               std::initializer_list<
                        typename component_type::public_input_container_type::value_type> public_inputs):
                    component_type(witnesses, constants, public_inputs, get_manifest()){};

                static typename BlueprintFieldType::value_type calculate(typename BlueprintFieldType::value_type x,
                                                                         typename BlueprintFieldType::value_type y) {
                    return (y == 0) ? 0 : x / y;
                }
            };

            template<typename BlueprintFieldType>
            using plonk_division_or_zero =
                division_or_zero<crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType>,
                BlueprintFieldType>;

            template<typename BlueprintFieldType>
            typename plonk_division_or_zero<BlueprintFieldType>::result_type
                generate_assignments(
                    const plonk_division_or_zero<BlueprintFieldType> &component,
                    assignment<crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType>> &assignment,
                    const typename plonk_division_or_zero<BlueprintFieldType>::input_type instance_input,
                    const std::uint32_t start_row_index) {

                const std::size_t j = start_row_index;

                assignment.witness(component.W(0), j) = var_value(assignment, instance_input.x);
                assignment.witness(component.W(1), j) = var_value(assignment, instance_input.y);
                if (var_value(assignment, instance_input.y) != 0) {
                    assignment.witness(component.W(2), j) = var_value(assignment, instance_input.x) /
                        var_value(assignment, instance_input.y);
                } else {
                    assignment.witness(component.W(2), j) = 0;
                }
                assignment.witness(component.W(3), j) = (var_value(assignment, instance_input.y) == 0) ?
                    0 : var_value(assignment, instance_input.y).inversed();
                assignment.witness(component.W(4), j) = var_value(assignment, instance_input.y) * assignment.witness(component.W(3), j);

                return typename plonk_division_or_zero<BlueprintFieldType>::result_type(component, start_row_index);
            }

            template<typename BlueprintFieldType>
            typename plonk_division_or_zero<BlueprintFieldType>::result_type
                generate_empty_assignments(
                    const plonk_division_or_zero<BlueprintFieldType> &component,
                    assignment<crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType>> &assignment,
                    const typename plonk_division_or_zero<BlueprintFieldType>::input_type instance_input,
                    const std::uint32_t start_row_index) {

                using component_type = plonk_division_or_zero<BlueprintFieldType>;
                assignment.witness(component.W(2), start_row_index) = component_type::calculate(
                    var_value(assignment, instance_input.x), var_value(assignment, instance_input.y));

                return typename plonk_division_or_zero<BlueprintFieldType>::result_type(component, start_row_index);
            }

            template<typename BlueprintFieldType>
            std::size_t generate_gates(
                const plonk_division_or_zero<BlueprintFieldType> &component,
                circuit<crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType>> &bp,
                assignment<crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType>> &assignment,
                const typename plonk_division_or_zero<BlueprintFieldType>::input_type &instance_input) {

                using var = typename plonk_division_or_zero<BlueprintFieldType>::var;

                auto constraint_1 = var(component.W(1), 0) * var(component.W(3), 0) - var(component.W(4), 0);
                auto constraint_2 = var(component.W(4), 0) * (var(component.W(4), 0) - 1);
                auto constraint_3 = (var(component.W(3), 0) - var(component.W(1), 0)) * (var(component.W(4), 0) - 1);
                auto constraint_4 = var(component.W(0), 0) * var(component.W(3), 0) - var(component.W(2), 0);

                return bp.add_gate({constraint_1, constraint_2, constraint_3, constraint_4});
            }

            template<typename BlueprintFieldType>
            void generate_copy_constraints(
                const plonk_division_or_zero<BlueprintFieldType> &component,
                circuit<crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType>> &bp,
                assignment<crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType>> &assignment,
                const typename plonk_division_or_zero<BlueprintFieldType>::input_type &instance_input,
                const std::size_t start_row_index) {

                using var = typename plonk_division_or_zero<BlueprintFieldType>::var;

                const std::size_t j = start_row_index;
                var component_x = var(component.W(0), static_cast<int>(j), false);
                var component_y = var(component.W(1), static_cast<int>(j), false);
                bp.add_copy_constraint({instance_input.x, component_x});
                bp.add_copy_constraint({component_y, instance_input.y});
            }

            template<typename BlueprintFieldType>
            typename plonk_division_or_zero<BlueprintFieldType>::result_type
                generate_circuit(
                    const plonk_division_or_zero<BlueprintFieldType> &component,
                    circuit<crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType>> &bp,
                    assignment<crypto3::zk::snark::plonk_constraint_system<BlueprintFieldType>> &assignment,
                    const typename plonk_division_or_zero<BlueprintFieldType>::input_type &instance_input,
                    const std::size_t start_row_index){

                std::size_t selector_index = generate_gates(component, bp, assignment, instance_input);

                assignment.enable_selector(selector_index, start_row_index);

                generate_copy_constraints(component, bp, assignment, instance_input, start_row_index);

                return typename plonk_division_or_zero<BlueprintFieldType>::result_type(component, start_row_index);
            }
        }    // namespace components
    }        // namespace blueprint
}    // namespace nil

#endif    // CRYPTO3_BLUEPRINT_COMPONENTS_PLONK_FIELD_DIVISION_OR_ZERO_HPP
