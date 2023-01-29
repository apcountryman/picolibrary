# Interrupt Facilities
Interrupt facilities are defined in the
[`include/picolibrary/interrupt.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/interrupt.h)/[`source/picolibrary/interrupt.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/interrupt.cc)
header/source file pair.

## Controller
The `::picolibrary::Interrupt::Controller_Concept` concept class defines the expected
interface of an interrupt controller.
To disable an interrupt, use a controller implementation's `disable_interrupt()` member
function.
To enable an interrupt, use a controller implementation's `enable_interrupt()` member
function.
To save a current interrupt enable state, use a controller implementation's
`save_interrupt_enable_state()` member function.
To restore a previously saved interrupt enable state, use a controller implementation's
`restore_interrupt_enable_state()` member function.

The `::picolibrary::Testing::Automated::Interrupt::Mock_Controller` mock interrupt
controller class is available if the `PICOLIBRARY_ENABLE_AUTOMATED_TESTING` project
configuration option is `ON`.
The mock is defined in the
[`include/picolibrary/testing/automated/interrupt.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/interrupt.h)/[`source/picolibrary/testing/automated/interrupt.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/interrupt.cc)
header/source file pair.

## Critical Section Guard
To create a critical section, use the `::picolibrary::Interrupt::Critical_Section_Guard`
RAII critical section guard template class.
Exit actions are selected using the
`::picolibrary::Interrupt::Restore_Interrupt_Enable_State`/`::picolibrary::Interrupt::RESTORE_INTERRUPT_ENABLE_STATE`
and
`::picolibrary::Interrupt::Enable_Interrupt`/`::picolibrary::Interrupt::ENABLE_INTERRUPT`
types/constants.
`::picolibrary::Interrupt::Critical_Section_Guard` automated tests are defined in the
[`test/automated/picolibrary/interrupt/critical_section_guard/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/interrupt/critical_section_guard/main.cc)
source file.
```c++
#include "picolibrary/interrupt.h"

template<typename Controller>
void foo( Controller & controller ) noexcept
{
    {
        // save interrupt enable state and disable interrupt
        auto const guard = ::picolibrary::Interrupt::Critical_Section_Guard{
            controller,
            ::picolibrary::Interrupt::RESTORE_INTERRUPT_ENABLE_STATE };
    }
    // interrupt enable state restored
}

template<typename Controller>
void foo( Controller & controller ) noexcept
{
    {
        // disable interrupt
        auto const guard = ::picolibrary::Interrupt::Critical_Section_Guard{
            controller,
            ::picolibrary::Interrupt::ENABLE_INTERRUPT };
    }
    // interrupt enabled
}
```
