//---------------------------------------------------------------------------//
// Copyright (c) 2020-2021 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2020-2021 Nikita Kaskov <nbering@nil.foundation>
// Copyright (c) 2021 Noam Yemini <@NoamDev at GitHub>
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

#ifndef CRYPTO3_ZK_BLUEPRINT_BLUEPRINT_R1CS_HPP
#define CRYPTO3_ZK_BLUEPRINT_BLUEPRINT_R1CS_HPP

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <string>
#include <vector>

#include <nil/crypto3/zk/snark/relations/constraint_satisfaction_problems/r1cs.hpp>

#include <nil/crypto3/zk/blueprint_variable.hpp>
#include <nil/crypto3/zk/blueprint_linear_combination.hpp>

namespace nil {
    namespace crypto3 {
        namespace zk {
            namespace components {

                template<typename TArithmetization>
                class blueprint;

                template<typename TBlueprintField>
                class blueprint<snark::r1cs_constraint_system<TBlueprintField>> {
                    typedef snark::r1cs_constraint_system<TBlueprintField> TArithmetization;

                    snark::r1cs_variable_assignment<TBlueprintField>
                        values; /* values[0] will hold the value of the first
                   allocated variable of the blueprint, *NOT* constant 1 */
                    typename TBlueprintField::value_type constant_term;

                    typename snark::variable<TBlueprintField>::index_type next_free_var;
                    typename blueprint_linear_combination<TArithmetization>::index_type next_free_lc;
                    std::vector<typename TBlueprintField::value_type> lc_values;
                    snark::r1cs_constraint_system<TBlueprintField> constraint_system;

                public:
                    // typedef TBlueprintField field_type;

                    using value_type = blueprint_variable<TArithmetization>;

                    blueprint() {
                        constant_term = TBlueprintField::value_type::one();

                        next_free_var = 1; /* to account for constant 1 term */
                        next_free_lc = 0;
                    }

                    void clear_values() {
                        std::fill(values.begin(), values.end(), TBlueprintField::value_type::zero());
                    }

                    typename TBlueprintField::value_type &val(const value_type &var) {
                        assert(var.index <= values.size());
                        return (var.index == 0 ? constant_term : values[var.index - 1]);
                    }

                    typename TBlueprintField::value_type val(const value_type &var) const {
                        assert(var.index <= values.size());
                        return (var.index == 0 ? constant_term : values[var.index - 1]);
                    }

                    typename TBlueprintField::value_type &
                        lc_val(const blueprint_linear_combination<TArithmetization> &lc) {
                        if (lc.is_variable) {
                            return this->val(value_type(lc.index));
                        } else {
                            assert(lc.index < lc_values.size());
                            return lc_values[lc.index];
                        }
                    }

                    typename TBlueprintField::value_type
                        lc_val(const blueprint_linear_combination<TArithmetization> &lc) const {
                        if (lc.is_variable) {
                            return this->val(value_type(lc.index));
                        } else {
                            assert(lc.index < lc_values.size());
                            return lc_values[lc.index];
                        }
                    }

                    void add_r1cs_constraint(const snark::r1cs_constraint<TBlueprintField> &constr) {
                        constraint_system.constraints.emplace_back(constr);
                    }

                    bool is_satisfied() const {
                        return constraint_system.is_satisfied(primary_input(), auxiliary_input());
                    }

                    std::size_t num_constraints() const {
                        return constraint_system.num_constraints();
                    }

                    std::size_t num_inputs() const {
                        return constraint_system.num_inputs();
                    }

                    std::size_t num_variables() const {
                        return next_free_var - 1;
                    }

                    void set_input_sizes(const std::size_t primary_input_size) {
                        assert(primary_input_size <= num_variables());
                        constraint_system.primary_input_size = primary_input_size;
                        constraint_system.auxiliary_input_size = num_variables() - primary_input_size;
                    }

                    snark::r1cs_variable_assignment<TBlueprintField> full_variable_assignment() const {
                        return values;
                    }

                    snark::r1cs_primary_input<TBlueprintField> primary_input() const {
                        return snark::r1cs_primary_input<TBlueprintField>(values.begin(),
                                                                          values.begin() + num_inputs());
                    }

                    snark::r1cs_auxiliary_input<TBlueprintField> auxiliary_input() const {
                        return snark::r1cs_auxiliary_input<TBlueprintField>(values.begin() + num_inputs(),
                                                                            values.end());
                    }

                    snark::r1cs_constraint_system<TBlueprintField> get_constraint_system() const {
                        return constraint_system;
                    }

                    friend class blueprint_variable<TArithmetization>;
                    friend class blueprint_linear_combination<TArithmetization>;

                private:
                    typename snark::variable<TBlueprintField>::index_type allocate_var_index() {
                        ++constraint_system.auxiliary_input_size;
                        values.emplace_back(TBlueprintField::value_type::zero());
                        return next_free_var++;
                    }

                    typename blueprint_linear_combination<TArithmetization>::index_type allocate_lc_index() {
                        lc_values.emplace_back(TBlueprintField::value_type::zero());
                        return next_free_lc++;
                    }
                };

            }    // namespace components
        }        // namespace zk
    }            // namespace crypto3
}    // namespace nil
#endif    // CRYPTO3_ZK_BLUEPRINT_BLUEPRINT_R1CS_HPP