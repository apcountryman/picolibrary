# Circular Buffer
The circular buffer container class (`::picolibrary::Circular_Buffer`) is defined in the
[`include/picolibrary/circular_buffer.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/circular_buffer.h)/[`source/picolibrary/circular_buffer.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/circular_buffer.cc)
header/source file pair.

The `::picolibrary::Circular_Buffer_Use_Case` enum class defines the supported circular
buffer use cases and is used to select the associated circular buffer implementation.
The following use cases are supported (these use cases assume that interrupts that
interact with a circular buffer cannot be interrupted by other interrupts that also
interact with the circular buffer):
- `::picolibrary::Circular_Buffer_Use_Case::MAIN_READS_MAIN_WRITES`:
  The main thread of execution reads from and write to the circular buffer.
  Interrupts do not interact with the circular buffer.
- `::picolibrary::Circular_Buffer_Use_Case::MAIN_READS_INTERRUPTS_WRITE`:
  The main thread of execution reads from the circular buffer.
  One or more interrupts write to the circular buffer.
- `::picolibrary::Circular_Buffer_Use_Case::INTERRUPT_READS_MAIN_WRITES`:
  An interrupt reads from the circular buffer.
  Other interrupts do not interact with the circular buffer.
  The main thread of execution writes to the circular buffer.
- `::picolibrary::Circular_Buffer_Use_Case::INTERRUPT_READS_INTERRUPTS_WRITE`:
  An interrupt reads from the circular buffer.
  One or more other interrupts write to the circular buffer.
  The main thread of execution does not interrupt with the circular buffer.

To access the element at the front of a circular buffer from the main thread of execution,
use the `::picolibrary::Circular_Buffer::front()` member functions.
To access the element at the front of a circular buffer from an interrupt, use the
`::picolibrary::Circular_Buffer::front_from_interrupt()` member functions.

To access the element at the back of a circular buffer from the main thread of execution,
use the `::picolibrary::Circular_Buffer::back()` member functions.
To access the element at the back of a circular buffer from an interrupt, use the
`::picolibrary::Circular_Buffer::back_from_interrupt()` member function.

To check if a circular buffer is empty from the main thread of execution, use the
`::picolibrary::Circular_Buffer::empty()` member function.
To check if a circular buffer is empty from an interrupt, use the
`::picolibrary::Circular_Buffer::empty_from_interrupt()` member function.

To check if a circular buffer is full from the main thread of execution, use the
`::picolibrary::Circular_Buffer::full()` member function.
To check if a circular buffer is full from an interrupt, use the
`::picolibrary::Circular_Buffer::full_from_interrupt()` member function.

To get the number of elements in a circular buffer from the main thread of execution, use
the `::picolibrary::Circular_Buffer::size()` member function.
To get the number of elements in a circular buffer from an interrupt, use the
`::picolibrary::Circular_Buffer::size_from_interrupt()` member function.

To get the maximum number of elements a circular buffer is able to hold from the main
thread of execution, use the `::picolibrary::Circular_Buffer::max_size()` member function.
To get the maximum number of elements a circular buffer is able to hold from an interrupt,
use the `::picolibrary::Circular_Buffer::max_size_from_interrupt()` member function.

To insert a value at the back of a circular buffer from the main thread of execution, use
the `::picolibrary::Circular_Buffer::push()` member functions.
To insert a value at the back of a circular buffer from an interrupt, use the
`::picolibrary::Circular_Buffer::push_from_interrupt()` member functions.

To emplace a value at the back of a circular buffer from the main thread of execution, use
the `::picolibrary::Circular_Buffer::emplace()` member functions.
To emplace a value at the back of a circular buffer from an interrupt, use the
`::picolibrary::Circular_Buffer::emplace_from_interrupt()` member functions.

To remove the element at the front of a circular buffer from the main thread of execution,
use the `::picolibrary::Circular_Buffer::pop()` member function.
To remove the element at the front of a circular buffer from an interrupt, use the
`::picolibrary::Circular_Buffer::pop_from_interrupt()` member function.

To remove all circular buffer elements from the main thread of execution, use the
`::picolibrary::Circular_Buffer::clear()` member function.
To remove all circular buffer elements from an interrupt, use the
`::picolibrary::Circular_Buffer::clear_from_interrupt()` member function.
