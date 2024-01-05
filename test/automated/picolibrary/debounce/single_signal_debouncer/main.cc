/**
 * picolibrary
 *
 * Copyright 2020-2024, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
 * contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this
 * file except in compliance with the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under
 * the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

/**
 * \file
 * \brief picolibrary::Debounce::Single_Signal_Debouncer automated test program.
 */

#include <cstddef>
#include <cstdint>
#include <ios>
#include <ostream>
#include <utility>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/debounce.h"

namespace {

using ::picolibrary::Debounce::Single_Signal_Debouncer;
using ::testing::TestWithParam;
using ::testing::ValuesIn;

} // namespace

/**
 * \brief picolibrary::Debounce::Single_Signal_Debouncer test case.
 */
struct debouncer_Test_Case {
    /**
     * \brief The initial debounced signal state.
     */
    bool is_high;

    /**
     * \brief New signal samples to update the debouncer with and the associated debounced
     *        signal state.
     */
    std::vector<std::pair<bool, bool>> data;
};

auto operator<<( std::ostream & stream, debouncer_Test_Case const & test_case ) -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".is_high = " << std::boolalpha << test_case.is_high
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::Debounce::Single_Signal_Debouncer test fixture.
 */
class debouncer : public TestWithParam<debouncer_Test_Case> {
};

/**
 * \brief Verify picolibrary::Debounce::Single_Signal_Debouncer works properly.
 */
TEST_P( debouncer, worksProperly )
{
    auto const test_case = GetParam();

    auto signal = Single_Signal_Debouncer<std::uint_fast8_t, 6>{ test_case.is_high };

    EXPECT_EQ( signal.is_high(), test_case.is_high );
    EXPECT_EQ( signal.is_low(), not signal.is_high() );

    for ( auto i = std::size_t{}; i < test_case.data.size(); ++i ) {
        auto const [ signal_sample_is_high, debounced_signal_is_high ] = test_case.data[ i ];

        signal.debounce( signal_sample_is_high );

        EXPECT_EQ( signal.is_high(), debounced_signal_is_high ) << "i: " << i;
        EXPECT_EQ( signal.is_low(), not signal.is_high() );
    } // for
}

/**
 * \brief picolibrary::Debounce::Single_Signal_Debouncer test case.
 */
debouncer_Test_Case const debouncer_TEST_CASES[]{
    // clang-format off

    {
        false,
        {
            { false, false },
            { false, false },
            { false, false },
            { false, false },
            { false, false },
            { false, false },

            { true,  false },
            { false, false },
            { false, false },
            { false, false },
            { false, false },
            { false, false },

            { true,  false },
            { true,  false },
            { true,  false },
            { true,  false },
            { true,  false },
            { false, false },

            { true,  false },
            { false, false },
            { false, false },
            { true,  false },
            { true,  false },
            { false, false },

            { true,  false },
            { true,  false },
            { true,  false },
            { true,  false },
            { true,  false },
            { true,  true  },

            { true,  true  },
            { true,  true  },
            { true,  true  },
            { true,  true  },
            { true,  true  },
            { true,  true  },

            { false, true  },
            { true,  true  },
            { true,  true  },
            { true,  true  },
            { true,  true  },
            { true,  true  },

            { false, true  },
            { false, true  },
            { false, true  },
            { false, true  },
            { false, true  },
            { true,  true  },

            { false, true  },
            { false, true  },
            { true,  true  },
            { true,  true  },
            { false, true  },
            { true,  true  },

            { false, true  },
            { false, true  },
            { false, true  },
            { false, true  },
            { false, true  },
            { false, false },
        }
    },

    {
        true,
        {
            { true,  true  },
            { true,  true  },
            { true,  true  },
            { true,  true  },
            { true,  true  },
            { true,  true  },

            { false, true  },
            { true,  true  },
            { true,  true  },
            { true,  true  },
            { true,  true  },
            { true,  true  },

            { false, true  },
            { false, true  },
            { false, true  },
            { false, true  },
            { false, true  },
            { true,  true  },

            { true,  true  },
            { false, true  },
            { false, true  },
            { true,  true  },
            { false, true  },
            { true,  true  },

            { false, true  },
            { false, true  },
            { false, true  },
            { false, true  },
            { false, true  },
            { false, false },

            { false, false },
            { false, false },
            { false, false },
            { false, false },
            { false, false },
            { false, false },

            { true,  false },
            { false, false },
            { false, false },
            { false, false },
            { false, false },
            { false, false },

            { true,  false },
            { true,  false },
            { true,  false },
            { true,  false },
            { true,  false },
            { false, false },

            { true,  false },
            { true,  false },
            { false, false },
            { true,  false },
            { false, false },
            { false, false },

            { true,  false },
            { true,  false },
            { true,  false },
            { true,  false },
            { true,  false },
            { true,  true  },
        }
    },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, debouncer, ValuesIn( debouncer_TEST_CASES ) );

/**
 * \brief Execute the picolibrary::Debounce::Single_Signal_Debouncer automated tests.
 *
 * \param[in] argc The number of arguments to pass to testing::InitGoogleMock().
 * \param[in] argc The array of arguments to pass to testing::InitGoogleMock().
 *
 * \return See Google Test's RUN_ALL_TESTS().
 */
int main( int argc, char * argv[] )
{
    ::testing::InitGoogleMock( &argc, argv );

    return RUN_ALL_TESTS();
}
