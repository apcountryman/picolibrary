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
 * \brief picolibrary::Interrupt::Critical_Section_Guard automated test program.
 */

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/interrupt.h"
#include "picolibrary/testing/automated/interrupt.h"

namespace {

using ::picolibrary::Interrupt::Critical_Section_Guard;
using ::picolibrary::Interrupt::ENABLE_INTERRUPT;
using ::picolibrary::Interrupt::RESTORE_INTERRUPT_ENABLE_STATE;
using ::picolibrary::Testing::Automated::Interrupt::Mock_Controller;
using ::testing::Return;

} // namespace

/**
 * \brief Verify picolibrary::Interrupt::Critical_Section_Guard works properly when
 *        Exit_Action is picolibrary::Interrupt::Restore_Interrupt_Enable_State.
 */
TEST( criticalSectionGuard, worksProperlyRestoreInterruptEnableState )
{
    auto controller = Mock_Controller{};

    auto const interrupt_enable_state = Mock_Controller::Interrupt_Enable_State{ 0xEA };

    EXPECT_CALL( controller, save_interrupt_enable_state() ).WillOnce( Return( interrupt_enable_state ) );
    EXPECT_CALL( controller, disable_interrupt() );

    auto const guard = Critical_Section_Guard{ controller, RESTORE_INTERRUPT_ENABLE_STATE };

    EXPECT_CALL( controller, restore_interrupt_enable_state( interrupt_enable_state ) );
}

/**
 * \brief Verify picolibrary::Interrupt::Critical_Section_Guard works properly when
 *        Exit_Action is picolibrary::Interrupt::Enable_Interrupt.
 */
TEST( criticalSectionGuard, worksProperlyEnableInterrupt )
{
    auto controller = Mock_Controller{};

    EXPECT_CALL( controller, disable_interrupt() );

    auto const guard = Critical_Section_Guard{ controller, ENABLE_INTERRUPT };

    EXPECT_CALL( controller, enable_interrupt() );
}

/**
 * \brief Execute the picolibrary::Interrupt::Critical_Section_Guard automated tests.
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
