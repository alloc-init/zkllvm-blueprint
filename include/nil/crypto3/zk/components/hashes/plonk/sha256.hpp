//---------------------------------------------------------------------------//
// Copyright (c) 2021 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2021 Nikita Kaskov <nbering@nil.foundation>
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
// @file Declaration of interfaces for auxiliary components for the SHA256 component.
//---------------------------------------------------------------------------//

#ifndef CRYPTO3_ZK_BLUEPRINT_PLONK_SHA256_HPP
#define CRYPTO3_ZK_BLUEPRINT_PLONK_SHA256_HPP

#include <nil/crypto3/zk/components/packing.hpp>
#include <nil/crypto3/zk/components/blueprint_variable.hpp>

namespace nil {
    namespace crypto3 {
        namespace zk {
            namespace components {

                template<typename TBlueprintField>
                class sha256_plonk_sigma_0 : public component<TBlueprintField> {
                    typedef snark::plonk_constraint_system<TBlueprintField> arithmetization_type;
                public:

                    range<arithmetization_type, TBlueprintField> range_proof;

                    sha256_plonk_sigma_0(blueprint<arithmetization_type, TBlueprintField> &bp,
                                          const std::array<typename blueprint::wire_index_type, 4> &w_indexes,
                                          const ... &output) :
                        component<FieldType>(bp), range_proof(input, 2**32) {



                    }

                    void generate_r1cs_constraints(bool ensure_output_bitness = true) {    // TODO: ignored for now
                        padding->generate_r1cs_constraints();
                        for (auto f : blocks_components) {
                            f->generate_r1cs_constraints();
                        }
                    }

                    void generate_r1cs_witness() {
                        padding->generate_r1cs_witness();
                        for (auto f : blocks_components) {
                            f->generate_r1cs_witness();
                        }
                    }
                };

            }    // namespace components
        }        // namespace zk
    }            // namespace crypto3
}    // namespace nil

#endif    // CRYPTO3_ZK_BLUEPRINT_PLONK_SHA256_HPP
