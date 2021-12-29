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
 * \brief picolibrary::Vector interface.
 */

#ifndef PICOLIBRARY_VECTOR_H
#define PICOLIBRARY_VECTOR_H

#include <cstddef>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <new>
#include <type_traits>
#include <utility>

#include "picolibrary/algorithm.h"
#include "picolibrary/array.h"
#include "picolibrary/error.h"
#include "picolibrary/result.h"
#include "picolibrary/void.h"

namespace picolibrary {

/**
 * \brief Fixed capacity vector.
 *
 * \tparam T The vector element type.
 * \tparam N The maximum number of elements in the vector.
 */
template<typename T, std::size_t N>
class Fixed_Capacity_Vector {
  public:
    /**
     * \brief The vector element type.
     */
    using Value = T;

    /**
     * \brief The number of elements in the vector.
     */
    using Size = std::size_t;

    /**
     * \brief A vector element position.
     */
    using Position = std::size_t;

    /**
     * \brief A reference to a vector element.
     */
    using Reference = Value &;

    /**
     * \brief A reference to a const vector element.
     */
    using Const_Reference = Value const &;

    /**
     * \brief A pointer to a vector element.
     */
    using Pointer = Value *;

    /**
     * \brief A pointer to a const vector element.
     */
    using Const_Pointer = Value const *;

    /**
     * \brief A vector iterator.
     */
    using Iterator = Pointer;

    /**
     * \brief A const vector iterator.
     */
    using Const_Iterator = Const_Pointer;

    /**
     * \brief A vector reverse iterator.
     */
    using Reverse_Iterator = std::reverse_iterator<Iterator>;

    /**
     * \brief A cosnt vector reverse iterator.
     */
    using Const_Reverse_Iterator = std::reverse_iterator<Const_Iterator>;

    /**
     * \brief Constructor.
     */
    constexpr Fixed_Capacity_Vector() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \warning This constructor does not validate its arguments. Use
     *          picolibrary::make_fixed_capacity_vector( std::size_t, T const & ) if
     *          argument validation is desired.
     *
     * \param[in] n The number of copies of value to initialize the vector with.
     * \param[in] value The value to initialize the vector with n copies of.
     */
    constexpr Fixed_Capacity_Vector( Size n, Value const & value ) noexcept : m_size{ n }
    {
        while ( n-- ) { new ( &m_storage[ n ] ) Value{ value }; } // while
    }

    /**
     * \brief Constructor.
     *
     * \warning This constructor does not validate its arguments. Use
     *          picolibrary::make_fixed_capacity_vector( std::size_t ) if argument
     *          validation is desired.
     *
     * \param[in] n The number of default constructed values to initialize the vector
     *            with.
     */
    constexpr explicit Fixed_Capacity_Vector( Size n ) noexcept : m_size{ n }
    {
        while ( n-- ) { new ( &m_storage[ n ] ) Value{}; } // while
    }

    /**
     * \brief Constructor.
     *
     * \warning This constructor does not validate its arguments. Use
     *          picolibrary::make_fixed_capacity_vector( Iterator, Iterator ) if argument
     *          validation is desired.
     *
     * \tparam Iterator Range iterator.
     *
     * \param[in] begin The beginning of the range of values to initialize the vector
     *            with.
     * \param[in] end The end of the range of values to initialize the vector with.
     */
    template<typename Iterator>
    constexpr Fixed_Capacity_Vector( Iterator begin, Iterator end ) noexcept :
        m_size{ static_cast<Size>( end - begin ) }
    {
        ::picolibrary::for_each( begin, end, [ storage = &m_storage[ 0 ] ]( auto value ) mutable noexcept {
            new ( storage++ ) Value{ value };
        } );
    }

    /**
     * \brief Constructor.
     *
     * \warning This constructor does not validate its arguments. Use
     *          picolibrary::make_fixed_capacity_vector( std::initializer_list<T> ) if
     *          argument validation is desired.
     *
     * \param[in] initializer_list The initializer list containing the values to
     *            initialize the vector with.
     */
    constexpr Fixed_Capacity_Vector( std::initializer_list<Value> initializer_list ) noexcept :
        m_size{ initializer_list.size() }
    {
        ::picolibrary::for_each(
            initializer_list.begin(),
            initializer_list.end(),
            [ storage = &m_storage[ 0 ] ]( auto value ) mutable noexcept {
                new ( storage++ ) Value{ value };
            } );
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Fixed_Capacity_Vector( Fixed_Capacity_Vector && source ) noexcept :
        m_size{ source.m_size }
    {
        for ( auto i = Size{}; i < m_size; ++i ) {
            new ( &m_storage[ i ] ) Value{ std::move(
                *std::launder( reinterpret_cast<Pointer>( &source.m_storage[ i ] ) ) ) };
        } // for

        source.clear();
    }

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Fixed_Capacity_Vector( Fixed_Capacity_Vector const & original ) noexcept :
        m_size{ original.m_size }
    {
        for ( auto i = Size{}; i < m_size; ++i ) {
            new ( &m_storage[ i ] )
                Value{ *std::launder( reinterpret_cast<Pointer>( &original.m_storage[ i ] ) ) };
        } // for
    }

    /**
     * \brief Destructor.
     */
    ~Fixed_Capacity_Vector() noexcept
    {
        clear();
    }

    /**
     * \brief Assignment operator.
     *
     * \warning This function does not validate its arguments. Use
     *          picolibrary::Fixed_Capacity_Vector::assign( std::initializer_list<Value> )
     *          if argument validation is desired.
     *
     * \param[in] initializer_list The initializer list containing the values to store in
     *            the vector.
     *
     * \return The assigned to object.
     */
    constexpr auto & operator=( std::initializer_list<Value> initializer_list ) noexcept
    {
        clear();

        ::picolibrary::for_each(
            initializer_list.begin(),
            initializer_list.end(),
            [ storage = &m_storage[ 0 ] ]( auto value ) mutable noexcept {
                new ( storage++ ) Value{ value };
            } );

        m_size = initializer_list.size();

        return *this;
    }

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto & operator=( Fixed_Capacity_Vector && expression ) noexcept
    {
        if ( &expression != this ) {
            clear();

            m_size = expression.m_size;

            for ( auto i = Size{}; i < m_size; ++i ) {
                new ( &m_storage[ i ] ) Value{ std::move(
                    *std::launder( reinterpret_cast<Pointer>( &expression.m_storage[ i ] ) ) ) };
            } // for

            expression.clear();
        } // if

        return *this;
    }

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto & operator=( Fixed_Capacity_Vector const & expression ) noexcept
    {
        if ( &expression != this ) {
            clear();

            m_size = expression.m_size;

            for ( auto i = Size{}; i < m_size; ++i ) {
                new ( &m_storage[ i ] )
                    Value{ *std::launder( reinterpret_cast<Pointer>( &expression.m_storage[ i ] ) ) };
            } // for
        }     // if

        return *this;
    }

    /**
     * \brief Replace the vector's contents with n copies of a value.
     *
     * \param[in] n The number of copies of value to store in the vector.
     * \param[in] value The value to store n copies of in the vector.
     *
     * \return Nothing if n is less than or equal to N.
     * \return picolibrary::Generic_Error::INSUFFICIENT if n is greater than N.
     */
    constexpr auto assign( Size n, Value const & value ) noexcept -> Result<Void, Error_Code>
    {
        if ( n > N ) {
            return Generic_Error::INSUFFICIENT_CAPACITY;
        } // if

        clear();

        m_size = n;

        while ( n-- ) { new ( &m_storage[ n ] ) Value{ value }; } // while

        return {};
    }

    /**
     * \brief Replace the vector's contents with those in the provided range.
     *
     * \param[in] begin The beginning of the range of values to store in the vector.
     * \param[in] end The end of the range of values to store in the vector.
     *
     * \return Nothing if the size of the range is less than or equal to N.
     * \return picolibrary::Generic_Error::INSUFFICIENT_CAPACITY if the size of the range
     *         is greater than N.
     */
    template<typename Iterator>
    constexpr auto assign( Iterator begin, Iterator end ) noexcept -> Result<Void, Error_Code>
    {
        if ( end - begin > N ) {
            return Generic_Error::INSUFFICIENT_CAPACITY;
        } // if

        clear();

        ::picolibrary::for_each( begin, end, [ storage = &m_storage[ 0 ] ]( auto value ) mutable noexcept {
            new ( storage++ ) Value{ value };
        } );

        return {};
    }

    /**
     * \brief Replace the vector's contents with those in the provided initializer list.
     *
     * \param[in] initializer_list The initializer list containing the values to store in
     *            the vector.
     *
     * \return Nothing if the size of the initializer list is less than or equal to N.
     * \return picolibrary::Generic_Error::INSUFFICIENT_CAPACITY if the size of the
     *         initializer list is greater than N.
     */
    constexpr auto assign( std::initializer_list<Value> initializer_list ) noexcept
        -> Result<Void, Error_Code>
    {
        if ( initializer_list.size() > N ) {
            Generic_Error::INSUFFICIENT_CAPACITY;
        } // if

        clear();

        ::picolibrary::for_each(
            initializer_list.begin(),
            initializer_list.end(),
            [ storage = &m_storage[ 0 ] ]( auto value ) mutable noexcept {
                new ( storage++ ) Value{ value };
            } );

        return {};
    }

    /**
     * \brief Access the element at the specified position in the vector.
     *
     * \warning Bounds checking is not performed.
     *
     * \param[in] position The position of the vector element to access.
     *
     * \return The element at the specified position in the vector.
     */
    auto operator[]( Position position ) noexcept -> Reference
    {
        return data()[ position ];
    }

    /**
     * \brief Access the element at the specified position in the vector.
     *
     * \warning Bounds checking is not performed.
     *
     * \param[in] position The position of the vector element to access.
     *
     * \return The element at the specified position in the vector.
     */
    auto operator[]( Position position ) const noexcept -> Const_Reference
    {
        return data()[ position ];
    }

    /**
     * \brief Access the element at the specified position in the vector.
     *
     * \param[in] position The position of the vector element to access.
     *
     * \return The element at the specified position in the vector if the specified
     *         position is a valid position.
     * \return picolibrary::Generic_Error::OUT_OF_RANGE if the specified position is not a
     *         valid position.
     */
    constexpr auto at( Position position ) noexcept -> Result<std::reference_wrapper<Value>, Error_Code>
    {
        if ( position >= size() ) {
            return Generic_Error::OUT_OF_RANGE;
        } // if

        return data()[ position ];
    }

    /**
     * \brief Access the element at the specified position in the vector.
     *
     * \param[in] position The position of the vector element to access.
     *
     * \return The element at the specified position in the vector if the specified
     *         position is a valid position.
     * \return picolibrary::Generic_Error::OUT_OF_RANGE if the specified position is not a
     *         valid position.
     */
    constexpr auto at( Position position ) const noexcept
        -> Result<std::reference_wrapper<Value const>, Error_Code>
    {
        if ( position >= size() ) {
            return Generic_Error::OUT_OF_RANGE;
        } // if

        return data()[ position ];
    }

    /**
     * \brief Access the first element of the vector.
     *
     * \warning Calling this function on an empty vector results in undefined behavior.
     *
     * \return The first element of the vector.
     */
    auto front() noexcept -> Reference
    {
        return *begin();
    }

    /**
     * \brief Access the first element of the vector.
     *
     * \warning Calling this function on an empty vector results in undefined behavior.
     *
     * \return The first element of the vector.
     */
    auto front() const noexcept -> Const_Reference
    {
        return *begin();
    }

    /**
     * \brief Access the last element of the vector.
     *
     * \warning Calling this function on an empty vector results in undefined behavior.
     *
     * \return The last element of the vector.
     */
    auto back() noexcept -> Reference
    {
        return *( end() - 1 );
    }

    /**
     * \brief Access the last element of the vector.
     *
     * \warning Calling this function on an empty vector results in undefined behavior.
     *
     * \return The last element of the vector.
     */
    auto back() const noexcept -> Const_Reference
    {
        return *( end() - 1 );
    }

    /**
     * \brief Access the underlying array.
     *
     * \return The underlying array.
     */
    auto data() noexcept -> Pointer
    {
        return std::launder( reinterpret_cast<Pointer>( m_storage.data() ) );
    }

    /**
     * \brief Access the underlying array.
     *
     * \return The underlying array.
     */
    auto data() const noexcept -> Const_Pointer
    {
        return std::launder( reinterpret_cast<Const_Pointer>( m_storage.data() ) );
    }

    /**
     * \brief Get an iterator to the first element of the vector.
     *
     * \return An iterator to the first element of the vector.
     */
    auto begin() noexcept -> Iterator
    {
        return data();
    }

    /**
     * \brief Get an iterator to the first element of the vector.
     *
     * \return An iterator to the first element of the vector.
     */
    auto begin() const noexcept -> Const_Iterator
    {
        return data();
    }

    /**
     * \brief Get an iterator to the first element of the vector.
     *
     * \return An iterator to the first element of the vector.
     */
    auto cbegin() const noexcept -> Const_Iterator
    {
        return data();
    }

    /**
     * \brief Get an iterator to the element following the last element of the vector.
     *
     * \warning Attempting to access the element following the last element of a vector
     *          results in undefined behavior.
     *
     * \return An iterator to the element following the last element of the vector.
     */
    auto end() noexcept -> Iterator
    {
        return begin() + size();
    }

    /**
     * \brief Get an iterator to the element following the last element of the vector.
     *
     * \warning Attempting to access the element following the last element of a vector
     *          results in undefined behavior.
     *
     * \return An iterator to the element following the last element of the vector.
     */
    auto end() const noexcept -> Const_Iterator
    {
        return begin() + size();
    }

    /**
     * \brief Get an iterator to the element following the last element of the vector.
     *
     * \warning Attempting to access the element following the last element of a vector
     *          results in undefined behavior.
     *
     * \return An iterator to the element following the last element of the vector.
     */
    auto cend() const noexcept -> Const_Iterator
    {
        return cbegin() + size();
    }

    /**
     * \brief Get an iterator to the first element of the reversed vector.
     *
     * \return An iterator to the first element of the reversed vector.
     */
    auto rbegin() noexcept -> Reverse_Iterator
    {
        return { end() };
    }

    /**
     * \brief Get an iterator to the first element of the reversed vector.
     *
     * \return An iterator to the first element of the reversed vector.
     */
    auto rbegin() const noexcept -> Const_Reverse_Iterator
    {
        return { end() };
    }

    /**
     * \brief Get an iterator to the first element of the reversed vector.
     *
     * \return An iterator to the first element of the reversed vector.
     */
    auto crbegin() const noexcept -> Const_Reverse_Iterator
    {
        return { cend() };
    }

    /**
     * \brief Get an iterator to the element following the last element of the reversed
     *        vector.
     *
     * \warning Attempting to access the element following the last element of a reversed
     *          vector results in undefined behavior.
     *
     * \return An iterator to the element following the last element of the reversed
     *         vector.
     */
    auto rend() noexcept -> Reverse_Iterator
    {
        return { begin() };
    }

    /**
     * \brief Get an iterator to the element following the last element of the reversed
     *        vector.
     *
     * \warning Attempting to access the element following the last element of a reversed
     *          vector results in undefined behavior.
     *
     * \return An iterator to the element following the last element of the reversed
     *         vector.
     */
    auto rend() const noexcept -> Const_Reverse_Iterator
    {
        return { begin() };
    }

    /**
     * \brief Get an iterator to the element following the last element of the reversed
     *        vector.
     *
     * \warning Attempting to access the element following the last element of a reversed
     *          vector results in undefined behavior.
     *
     * \return An iterator to the element following the last element of the reversed
     *         vector.
     */
    auto crend() const noexcept -> Const_Reverse_Iterator
    {
        return { cbegin() };
    }

    /**
     * \brief Check if the vector is empty.
     *
     * \return true if the vector is empty.
     * \return false if the vector is not empty.
     */
    [[nodiscard]] constexpr auto empty() const noexcept -> bool
    {
        return not size();
    }

    /**
     * \brief Get the number of elements in the vector.
     *
     * \return The number of elements in the vector.
     */
    constexpr auto size() const noexcept -> Size
    {
        return m_size;
    }

    /**
     * \brief Get the maximum number of elements the vector is able to hold.
     *
     * \return The maximum number of elements the vector is able to hold.
     */
    constexpr auto max_size() const noexcept -> Size
    {
        return N;
    }

    /**
     * \brief Get the number of elements the vector has allocated space for.
     *
     * \return The number of elements the vector has allocated space for.
     */
    constexpr auto capacity() const noexcept -> Size
    {
        return N;
    }

    /**
     * \brief Erase all vector elements.
     */
    constexpr void clear() noexcept
    {
        for ( auto i = Size{}; i < m_size; ++i ) {
            std::launder( reinterpret_cast<Pointer>( &m_storage[ i ] ) )->~Value();
        } // for

        m_size = 0;
    }

    /**
     * \brief Insert a value before the specified position in the vector.
     *
     * \param[in] position The position to insert value before.
     * \param[in] value The value to insert.
     *
     * \return An iterator to the inserted element if the vector has sufficient capacity
     *         to store the new element.
     * \return picolibrary::Generic_Error::INSUFFICIENT_CAPACITY if the vector does not
     *         have sufficient capacity to insert the new element.
     */
    constexpr auto insert( Const_Iterator position, Value const & value ) noexcept
        -> Result<Iterator, Error_Code>
    {
        if ( m_size + 1 > N ) {
            return Generic_Error::INSUFFICIENT_CAPACITY;
        } // if

        auto element = --end();

        for ( ; element >= position; --element ) {
            new ( element + 1 ) Value{ std::move( *element ) };

            element->~Value();
        } // for

        auto const inserted_element = element + 1;

        new ( ++element ) Value{ value };

        ++m_size;

        return inserted_element;
    }

    /**
     * \brief Insert a value before the specified position in the vector.
     *
     * \param[in] position The position to insert value before.
     * \param[in] value The value to insert.
     *
     * \return An iterator to the inserted element if the vector has sufficient capacity
     *         to store the new element.
     * \return picolibrary::Generic_Error::INSUFFICIENT_CAPACITY if the vector does not
     *         have sufficient capacity to insert the new element.
     */
    constexpr auto insert( Const_Iterator position, Value && value ) noexcept
        -> Result<Iterator, Error_Code>
    {
        if ( m_size + 1 > N ) {
            return Generic_Error::INSUFFICIENT_CAPACITY;
        } // if

        auto element = --end();

        for ( ; element >= position; --element ) {
            new ( element + 1 ) Value{ std::move( *element ) };

            element->~Value();
        } // for

        auto const inserted_element = element + 1;

        new ( ++element ) Value{ std::move( value ) };

        ++m_size;

        return inserted_element;
    }

    /**
     * \brief Insert n copies of a value before the specified position in the vector.
     *
     * \param[in] position The position to insert n copies of value before.
     * \param[in] n The number of copies of value to insert.
     * \param[in] value The value to insert n copies of.
     *
     * \return An iterator to the inserted elements if the vector has sufficient capacity
     *         to store the new elements.
     * \return picolibrary::Generic_Error::INSUFFICIENT_CAPACITY if the vector does not
     *         have sufficient capacity to store the new elements.
     */
    constexpr auto insert( Const_Iterator position, Size n, Value const & value ) noexcept
        -> Result<Iterator, Error_Code>
    {
        if ( m_size + n > N ) {
            return Generic_Error::INSUFFICIENT_CAPACITY;
        } // if

        auto element = --end();

        for ( ; element >= position; --element ) {
            new ( element + n ) Value{ std::move( *element ) };

            element->~Value();
        } // for

        auto const inserted_elements = element + 1;

        while ( n-- ) { new ( ++element ) Value{ value }; } // while

        m_size += n;

        return inserted_elements;
    }

    /**
     * \brief Insert values from the specified range before the specified position in the
     *        vector.
     *
     * \tparam Iterator Range iterator.
     *
     * \param[in] position The position to insert the values before.
     * \param[in] begin The beginning of the range of values to insert.
     * \param[in] end The end of the range of values to insert.
     *
     * \return An iterator to the inserted elements if the vector has sufficient capacity
     *         to store the new elements.
     * \return picolibrary::Generic_Error::INSUFFICIENT_CAPACITY if the vector does not
     *         have sufficient capacity to store the new elements.
     */
    template<typename Iterator>
    constexpr auto insert( Const_Iterator position, Iterator begin, Iterator end ) noexcept
        -> Result<Iterator, Error_Code>
    {
        // #lizard forgives the length

        auto const n = end - begin;

        if ( m_size + n > N ) {
            return Generic_Error::INSUFFICIENT_CAPACITY;
        } // if

        auto element = --end();

        for ( ; element >= position; --element ) {
            new ( element + n ) Value{ std::move( *element ) };

            element->~Value();
        } // for

        auto const inserted_elements = element + 1;

        ::picolibrary::for_each( begin, end, [ element ]( auto value ) mutable noexcept {
            new ( ++element ) Value{ value };
        } );

        m_size += n;

        return inserted_elements;
    }

    /**
     * \brief Insert values from the specified initializer list before the specified
     *        position in the vector.
     *
     * \param[in] position The position to insert the values before.
     * \param[in] initializer_list The initializer list containing the values to insert.
     *
     * \return An iterator to the inserted elements if the vector has sufficient capacity
     *         to store the new elements.
     * \return picolibrary::Generic_Error::INSUFFICIENT_CAPACITY if the vector does not
     *         have sufficient capacity to store the new elements.
     */
    constexpr auto insert( Const_Iterator position, std::initializer_list<Value> initializer_list ) noexcept
        -> Result<Iterator, Error_Code>
    {
        // #lizard forgives the length

        if ( m_size + initializer_list.size() > N ) {
            return Generic_Error::INSUFFICIENT_CAPACITY;
        } // if

        auto element = --end();

        for ( ; element >= position; --element ) {
            new ( element + initializer_list.size() ) Value{ std::move( *element ) };

            element->~Value();
        } // for

        auto const inserted_elements = element + 1;

        ::picolibrary::for_each(
            initializer_list.begin(), initializer_list.end(), [ element ]( auto value ) mutable noexcept {
                new ( ++element ) Value{ value };
            } );

        m_size += initializer_list.size();

        return inserted_elements;
    }

    /**
     * \brief Emplace a value before the specified position in the vector.
     *
     * \tparam Arguments Value construction argument types.
     *
     * \param[in] position The position to emplace the new value before.
     * \param[in] arguments Value construction arguments.
     *
     * \return An iterator to the emplaced element if the vector has sufficient capacity
     *         to store the new element.
     * \return picolibrary::Generic_Error::INSUFFICIENT_CAPACITY if the vector does not
     *         have sufficient capacity to insert the new element.
     */
    template<typename... Arguments>
    constexpr auto emplace( Const_Iterator position, Arguments &&... arguments ) noexcept
        -> Result<Iterator, Error_Code>
    {
        if ( m_size + 1 > N ) {
            return Generic_Error::INSUFFICIENT_CAPACITY;
        } // if

        auto element = --end();

        for ( ; element >= position; --element ) {
            new ( element + 1 ) Value{ std::move( *element ) };

            element->~Value();
        } // for

        auto const inserted_element = element + 1;

        new ( ++element ) Value{ std::forward<Arguments>( arguments )... };

        ++m_size;

        return inserted_element;
    }

    /**
     * \brief Remove the element at the specified position in the vector.
     *
     * \param[in] position The position of the element to remove.
     *
     * \return An iterator to the element following the removed element.
     */
    auto erase( Const_Iterator position ) noexcept -> Iterator
    {
        auto element = begin() + ( position - begin() );

        ( element++ )->~Value();

        for ( ; element != end(); ++element ) {
            new ( element - 1 ) Value{ std::move( *element ) };

            element->~Value();
        } // for

        --m_size;

        return begin() + ( position - begin() );
    }

    /**
     * \brief Remove the elements at the specified range of positions in the vector.
     *
     * \param[in] begin The beginning of the range of elements to remove.
     * \param[in] end The end of the range of elements to remove.
     *
     * \return An iterator to the element following the removed elements.
     */
    auto erase( Const_Iterator begin, Const_Iterator end ) noexcept -> Iterator
    {
        auto const n = end - begin;

        auto element = this->begin() + ( begin - this->begin() );

        for ( ; element != end; ++element ) { element->~Value(); } // for

        for ( ; element != this->end(); ++element ) {
            new ( element - n ) Value{ std::move( *element ) };

            element->~Value();
        } // for

        m_size -= n;

        return this->begin() + ( begin - this->begin() );
    }

    /**
     * \brief Append a value to the end of the vector.
     *
     * \param[in] value The value to append to the vector.
     *
     * \return Nothing if the vector has sufficient capacity to store the new element.
     * \return picolibrary::Generic_Error::INSUFFICIENT_CAPACITY if the vector does not
     *         have sufficient capacity to store the new element.
     */
    constexpr auto push_back( Value const & value ) noexcept -> Result<Void, Error_Code>
    {
        if ( m_size + 1 > N ) {
            return Generic_Error::INSUFFICIENT_CAPACITY;
        } // if

        new ( &m_storage[ m_size ] ) Value{ value };

        ++m_size;

        return {};
    }

    /**
     * \brief Append a value to the end of the vector.
     *
     * \param[in] value The value to append to the vector.
     *
     * \return Nothing if the vector has sufficient capacity to store the new element.
     * \return picolibrary::Generic_Error::INSUFFICIENT_CAPACITY if the vector does not
     *         have sufficient capacity to store the new element.
     */
    constexpr auto push_back( Value && value ) noexcept -> Result<Void, Error_Code>
    {
        if ( m_size + 1 > N ) {
            return Generic_Error::INSUFFICIENT_CAPACITY;
        } // if

        new ( &m_storage[ m_size ] ) Value{ std::move( value ) };

        ++m_size;

        return {};
    }

    /**
     * \brief Append a value to the end of the vector.
     *
     * \tparam Arguments Value construction argument types.
     *
     * \param[in] arguments Value construction arguments.
     *
     * \return A reference to the appended element if the vector has sufficient capacity
     *         to store the new element.
     * \return picolibrary::Generic_Error::INSUFFICIENT_CAPACITY if the vector does not
     *         have sufficient capacity to store the new element.
     */
    template<typename... Arguments>
    constexpr auto emplace_back( Arguments &&... arguments ) noexcept
        -> Result<std::reference_wrapper<Value>, Error_Code>
    {
        if ( m_size + 1 > N ) {
            return Generic_Error::INSUFFICIENT_CAPACITY;
        } // if

        new ( &m_storage[ m_size ] ) Value{ std::forward<Arguments>( arguments )... };

        ++m_size;

        return back();
    }

    /**
     * \brief Remove the last element of the vector.
     */
    void pop_back() noexcept
    {
        back().~Value();

        --m_size;
    }

    /**
     * \brief Resize the vector to be the specified size.
     *
     * \param[in] size The desired vector size.
     *
     * \return Nothing if resizing the vector succeeded.
     * \return picolibrary::Generic_Error::INSUFFICIENT_CAPACITY if the vector does not
     *         have sufficient capacity to grow to the requested size.
     */
    constexpr auto resize( Size size ) noexcept -> Result<Void, Error_Code>
    {
        if ( size > N ) {
            return Generic_Error::INSUFFICIENT_CAPACITY;
        } // if

        if ( size < m_size ) {
            for ( auto element = begin() + size; element != end(); ++element ) {
                element->~Value();
            } // for
        } else if ( size > m_size ) {
            for ( auto element = end(); element != begin() + size; ++element ) {
                new ( element ) Value{};
            } // for
        }     // else if

        m_size = size;

        return {};
    }

    /**
     * \brief Resize the vector to be the specified size.
     *
     * \param[in] size The desired vector size.
     * \param[in] value The value to append to the vector if the desired vector size is
     *            greater than the current vector size.
     *
     * \return Nothing if resizing the vector succeeded.
     * \return picolibrary::Generic_Error::INSUFFICIENT_CAPACITY if the vector does not
     *         have sufficient capacity to grow to the requested size.
     */
    constexpr auto resize( Size size, Value const & value ) noexcept -> Result<Void, Error_Code>
    {
        if ( size > N ) {
            return Generic_Error::INSUFFICIENT_CAPACITY;
        } // if

        if ( size < m_size ) {
            for ( auto element = begin() + size; element != end(); ++element ) {
                element->~Value();
            } // for
        } else if ( size > m_size ) {
            for ( auto element = end(); element != begin() + size; ++element ) {
                new ( element ) Value{ value };
            } // for
        }     // else if

        m_size = size;

        return {};
    }

  private:
    /**
     * \brief The vector's storage.
     */
    Array<std::aligned_storage_t<sizeof( Value ), alignof( Value )>, N> m_storage{};

    /**
     * \brief The vector's size.
     */
    Size m_size{};
};

/**
 * \brief Get the capacity of a fixed capacity vector.
 *
 * \relatedalso picolibrary::Fixed_Capacity_Vector
 *
 * \tparam T The fixed capacity vector whose capacity is to be gotten.
 */
template<typename T>
struct fixed_capacity_vector_capacity {
};

/**
 * \brief Get the capacity of a fixed capacity vector.
 *
 * \relatedalso picolibrary::Fixed_Capacity_Vector
 *
 * \tparam T The fixed capacity vector whose capacity is to be gotten.
 */
template<typename T>
constexpr auto fixed_capacity_vector_capacity_v = fixed_capacity_vector_capacity<T>::value;

/**
 * \brief Get the capacity of a fixed capacity vector.
 *
 * \relatedalso picolibrary::Fixed_Capacity_Vector
 *
 * \tparam T The vector element type.
 * \tparam N The maximum number of elements in the vector.
 */
template<typename T, std::size_t N>
struct fixed_capacity_vector_capacity<Fixed_Capacity_Vector<T, N>> :
    std::integral_constant<std::size_t, N> {
};

/**
 * \brief Construct a fixed capacity vector.
 *
 * \relatedalso picolibrary::Fixed_Capacity_Vector
 *
 * \tparam T The vector element type.
 * \tparam N The maximum number of elements in the vector.
 *
 * \param[in] n The number of copies of value to initialize the vector with.
 * \param[in] value The value to initialize the vector with n copies of.
 *
 * \return The constructed vector if n is less than or equal to N.
 * \return picolibrary::Generic_Error::INSUFFICIENT_CAPACITY if n is greater than N.
 */
template<typename T, std::size_t N>
constexpr auto make_fixed_capacity_vector( std::size_t n, T const & value ) noexcept
    -> Result<Fixed_Capacity_Vector<T, N>, Error_Code>
{
    if ( n > N ) {
        return Generic_Error::INSUFFICIENT_CAPACITY;
    } // if

    return Fixed_Capacity_Vector<T, N>( n, value );
}

/**
 * \brief Construct a fixed capacity vector.
 *
 * \relatedalso picolibrary::Fixed_Capacity_Vector
 *
 * \tparam T The vector element type.
 * \tparam N The maximum number of elements in the vector.
 *
 * \param[in] n The number of default constructed values to initialize the vector with.
 *
 * \return The constructed vector if n is less than or equal to N.
 * \return picolibrary::Generic_Error::INSUFFICIENT_CAPACITY if n is greater than N.
 */
template<typename T, std::size_t N>
constexpr auto make_fixed_capacity_vector( std::size_t n ) noexcept
    -> Result<Fixed_Capacity_Vector<T, N>, Error_Code>
{
    if ( n > N ) {
        return Generic_Error::INSUFFICIENT_CAPACITY;
    } // if

    return Fixed_Capacity_Vector<T, N>( n );
}

/**
 * \brief Construct a fixed capacity vector.
 *
 * \relatedalso picolibrary::Fixed_Capacity_Vector
 *
 * \tparam T The vector element type.
 * \tparam N The maximum number of elements in the vector.
 * \tparam Iterator Range iterator.
 *
 * \param[in] begin The beginning of the range of values to initialize the vector with.
 * \param[in] end The end of the range of values to initialize the vector with.
 *
 * \return The constructed vector if the size of the range is less than or equal to N.
 * \return picolibrary::Generic_Error::INSUFFICIENT_CAPACITY if the size of the range is
 *         greater than N.
 */
template<typename T, std::size_t N, typename Iterator>
constexpr auto make_fixed_capacity_vector( Iterator begin, Iterator end ) noexcept
    -> Result<Fixed_Capacity_Vector<T, N>, Error_Code>
{
    if ( end - begin > N ) {
        return Generic_Error::INSUFFICIENT_CAPACITY;
    } // if

    return Fixed_Capacity_Vector<T, N>( begin, end );
}

/**
 * \brief Construct a fixed capacity vector.
 *
 * \relatedalso picolibrary::Fixed_Capacity_Vector
 *
 * \tparam T The vector element type.
 * \tparam N The maximum number of elements in the vector.
 *
 * \param[in] initializer_list The initializer list containing the values to initialize
 *            the vector with.
 *
 * \return The constructed  vector if the size of the initializer list is less than or
 *         equal to N.
 * \return picolibrary::Generic_Error::INSUFFICIENT_CAPACITY if the size of the
 *         initializer list is greater than N.
 */
template<typename T, std::size_t N>
constexpr auto make_fixed_capacity_vector( std::initializer_list<T> initializer_list ) noexcept
    -> Result<Fixed_Capacity_Vector<T, N>, Error_Code>
{
    if ( initializer_list.size() > N ) {
        return Generic_Error::INSUFFICIENT_CAPACITY;
    } // if

    return Fixed_Capacity_Vector<T, N>( initializer_list );
}

/**
 * \brief Equality operator.
 *
 * \relatedalso picolibrary::Fixed_Capacity_Vector
 *
 * \tparam T The vector element type.
 * \tparam LHS_N The maximum number of elements in the vector on the left hand size of the
 *         comparison.
 * \tparam RHS_N The maximum number of elements in the vector on the right hand size of
 *         the comparison.
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is equal to rhs.
 * \return false if lhs is not equal to rhs.
 */
template<typename T, std::size_t LHS_N, std::size_t RHS_N>
constexpr auto operator==( Fixed_Capacity_Vector<T, LHS_N> const & lhs, Fixed_Capacity_Vector<T, RHS_N> const & rhs ) noexcept
    -> bool
{
    if ( lhs.size() != rhs.size() ) {
        return false;
    } // if

    return equal( lhs.begin(), lhs.end(), rhs.begin() );
}

/**
 * \brief Inequality operator.
 *
 * \relatedalso picolibrary::Fixed_Capacity_Vector
 *
 * \tparam T The vector element type.
 * \tparam LHS_N The maximum number of elements in the vector on the left hand size of the
 *         comparison.
 * \tparam RHS_N The maximum number of elements in the vector on the right hand size of
 *         the comparison.
 *
 * \param[in] lhs The left hand side of the comparison.
 * \param[in] rhs The right hand side of the comparison.
 *
 * \return true if lhs is not equal to rhs.
 * \return false if lhs is equal to rhs.
 */
template<typename T, std::size_t LHS_N, std::size_t RHS_N>
constexpr auto operator!=( Fixed_Capacity_Vector<T, LHS_N> const & lhs, Fixed_Capacity_Vector<T, RHS_N> const & rhs ) noexcept
    -> bool
{
    return not( lhs == rhs );
}

} // namespace picolibrary

#endif // PICOLIBRARY_VECTOR_H
