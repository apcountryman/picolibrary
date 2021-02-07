/**
 * picolibrary
 *
 * Copyright 2020-2021, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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
 * \brief picolibrary::Error_Code unit test program.
 */

#include <string>
#include <utility>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/error.h"
#include "picolibrary/testing/unit/error.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/utility.h"

namespace {

using ::picolibrary::Error_Code;
using ::picolibrary::Error_ID;
using ::picolibrary::Void;
using ::picolibrary::Testing::Unit::Mock_Error;
using ::picolibrary::Testing::Unit::Mock_Error_Category;
using ::picolibrary::Testing::Unit::random;
using ::picolibrary::Testing::Unit::random_container;
using ::testing::Return;

auto random_unique_ids()
{
    auto const a = random<Error_ID>();
    auto const b = random<Error_ID>();

    return std::pair<Error_ID, Error_ID>{ a, b != a ? b : b ^ random<Error_ID>( 1 ) };
}

} // namespace

/**
 * \brief Verify picolibrary::Error_Code::Error_Code() works properly.
 */
TEST( constructorDefault, worksProperly )
{
    auto const error = Error_Code{};

    EXPECT_FALSE( error );
    EXPECT_NE( &error.category(), nullptr );
    EXPECT_STREQ( error.category().name(), "::picolibrary::Default_Error" );
    EXPECT_EQ( error.id(), 0 );
    EXPECT_STREQ( error.description(), "NONE" );
}

/**
 * \brief Verify picolibrary::Error_Code::Error_Code( picolibrary::Void ) works properly.
 */
TEST( constructorVoid, worksProperly )
{
    auto const error = Error_Code{ Void{} };

    EXPECT_FALSE( error );
    EXPECT_NE( &error.category(), nullptr );
    EXPECT_STREQ( error.category().name(), "::picolibrary::Default_Error" );
    EXPECT_EQ( error.id(), 0 );
    EXPECT_STREQ( error.description(), "NONE" );
}

/**
 * \brief Verify picolibrary::Error_Code::Error_Code( picolibrary::Error_Category const &,
 *        picolibrary::Error_ID ) works properly.
 */
TEST( constuctorCategoryID, worksProperly )
{
    auto const category = Mock_Error_Category{};
    auto const id       = random<Error_ID>();

    auto const error = Error_Code{ category, id };

    auto const description = random_container<std::string>();

    EXPECT_CALL( category, error_description( id ) ).WillOnce( Return( description.c_str() ) );

    EXPECT_TRUE( error );
    EXPECT_EQ( &error.category(), &category );
    EXPECT_EQ( error.id(), id );
    EXPECT_STREQ( error.description(), description.c_str() );
}

/**
 * \brief Verify picolibrary::Error_Code::Error_Code( Error_Code_Enum ) works properly.
 */
TEST( constructorErrorCodeEnum, worksProperly )
{
    auto const id = random<Error_ID>();

    auto const error = Error_Code{ static_cast<Mock_Error>( id ) };

    auto const description = random_container<std::string>();

    EXPECT_CALL( Mock_Error_Category::instance(), error_description( id ) )
        .WillOnce( Return( description.c_str() ) );

    EXPECT_TRUE( error );
    EXPECT_EQ( &error.category(), &Mock_Error_Category::instance() );
    EXPECT_EQ( error.id(), id );
    EXPECT_STREQ( error.description(), description.c_str() );
}

/**
 * \brief Verify picolibrary::operator==( picolibrary::Error_Code const &,
 *        picolibrary::Error_Code const & ) works properly.
 */
TEST( equalityOperator, worksProperly )
{
    {
        auto const category = Mock_Error_Category{};
        auto const id       = random<Error_ID>();

        EXPECT_TRUE( ( Error_Code{ category, id } ) == ( Error_Code{ category, id } ) );
    }

    {
        auto const category           = Mock_Error_Category{};
        auto const [ lhs_id, rhs_id ] = random_unique_ids();

        EXPECT_FALSE( ( Error_Code{ category, lhs_id } ) == ( Error_Code{ category, rhs_id } ) );
    }

    {
        auto const lhs_category = Mock_Error_Category{};
        auto const rhs_category = Mock_Error_Category{};
        auto const id           = random<Error_ID>();

        EXPECT_FALSE( ( Error_Code{ lhs_category, id } ) == ( Error_Code{ rhs_category, id } ) );
    }

    {
        auto const lhs_category       = Mock_Error_Category{};
        auto const rhs_category       = Mock_Error_Category{};
        auto const [ lhs_id, rhs_id ] = random_unique_ids();

        EXPECT_FALSE( ( Error_Code{ lhs_category, lhs_id } ) == ( Error_Code{ rhs_category, rhs_id } ) );
    }
}

/**
 * \brief Verify picolibrary::operator!=( picolibrary::Error_Code const &,
 *        picolibrary::Error_Code const & ) works properly.
 */
TEST( inequalityOperator, worksProperly )
{
    {
        auto const category = Mock_Error_Category{};
        auto const id       = random<Error_ID>();

        EXPECT_FALSE( ( Error_Code{ category, id } ) != ( Error_Code{ category, id } ) );
    }

    {
        auto const category           = Mock_Error_Category{};
        auto const [ lhs_id, rhs_id ] = random_unique_ids();

        EXPECT_TRUE( ( Error_Code{ category, lhs_id } ) != ( Error_Code{ category, rhs_id } ) );
    }

    {
        auto const lhs_category = Mock_Error_Category{};
        auto const rhs_category = Mock_Error_Category{};
        auto const id           = random<Error_ID>();

        EXPECT_TRUE( ( Error_Code{ lhs_category, id } ) != ( Error_Code{ rhs_category, id } ) );
    }

    {
        auto const lhs_category       = Mock_Error_Category{};
        auto const rhs_category       = Mock_Error_Category{};
        auto const [ lhs_id, rhs_id ] = random_unique_ids();

        EXPECT_TRUE( ( Error_Code{ lhs_category, lhs_id } ) != ( Error_Code{ rhs_category, rhs_id } ) );
    }
}

/**
 * \brief Execute the picolibrary::Error_Code unit tests.
 *
 * \param[in] argc The number of arguments to pass to testing::InitGoogleMock().
 * \param[in] argv The array  of arguments to pass to testing::InitGoogleMock().
 *
 * \return See Google Test's RUN_ALL_TESTS().
 */
int main( int argc, char * argv[] )
{
    ::testing::InitGoogleMock( &argc, argv );

    return RUN_ALL_TESTS();
}
