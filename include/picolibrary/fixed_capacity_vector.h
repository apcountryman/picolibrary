/**
 * picolibrary
 *
 * Copyright 2020-2022, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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
 * \brief picolibrary::Fixed_Capacity_Vector interface.
 */

#ifndef PICOLIBRARY_FIXED_CAPACITY_VECTOR_H
#define PICOLIBRARY_FIXED_CAPACITY_VECTOR_H

#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <new>
#include <type_traits>
#include <utility>

#include "picolibrary/algorithm.h"
#include "picolibrary/array.h"
#include "picolibrary/error.h"
#include "picolibrary/precondition.h"

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
     * \brief A const vector reverse iterator.
     */
    using Const_Reverse_Iterator = std::reverse_iterator<Const_Iterator>;

    /**
     * \brief Constructor.
     */
    constexpr Fixed_Capacity_Vector() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] n The number of copies of value to initialize the vector with.
     * \param[in] value The value to initialize the vector with n copies of.
     *
     * \pre n <= picolibrary::Fixed_Capacity_Vector::max_size()
     */
    constexpr Fixed_Capacity_Vector( Size n, Value const & value ) noexcept : m_size{ n }
    {
        expect( n <= max_size(), Generic_Error::INSUFFICIENT_CAPACITY );

        while ( n-- ) { new ( &m_storage[ n ] ) Value{ value }; } // while
    }

    /**
     * \brief Constructor.
     *
     * \param[in] n The number of default constructed values to initialize the vector
     *            with.
     *
     * \pre n <= picolibrary::Fixed_Capacity_Vector::max_size()
     */
    constexpr explicit Fixed_Capacity_Vector( Size n ) noexcept : m_size{ n }
    {
        expect( n <= max_size(), Generic_Error::INSUFFICIENT_CAPACITY );

        while ( n-- ) { new ( &m_storage[ n ] ) Value{}; } // while
    }

    /**
     * \brief Constructor.
     *
     * \tparam Iterator Range iterator.
     *
     * \param[in] begin The beginning of the range of values to initialize the vector
     *            with.
     * \param[in] end The end of the range of values to initialize the vector with.
     *
     * \pre static_cast<picolibrary::Fixed_Capacity_Vector::Size>( end - begin ) <=
     *      picolibrary::Fixed_Capacity_Vector::max_size()
     */
    template<typename Iterator>
    constexpr Fixed_Capacity_Vector( Iterator begin, Iterator end ) noexcept :
        m_size{ static_cast<Size>( end - begin ) }
    {
        expect( static_cast<Size>( end - begin ) <= max_size(), Generic_Error::INSUFFICIENT_CAPACITY );

        ::picolibrary::for_each( begin, end, [ storage = &m_storage[ 0 ] ]( auto value ) mutable noexcept {
            new ( storage++ ) Value{ value };
        } );
    }

    /**
     * \brief Constructor.
     *
     * \param[in] initializer_list The initializer list containing the values to
     *            initialize the vector with.
     *
     * \pre initializer_list.size() <= picolibrary::Fixed_Capacity_Vector::max_size()
     */
    constexpr Fixed_Capacity_Vector( std::initializer_list<Value> initializer_list ) noexcept :
        Fixed_Capacity_Vector{ initializer_list.begin(), initializer_list.end() }
    {
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
     * \param[in] initializer_list The initializer list containing the values to store in
     *            the vector.
     *
     * \pre initializer_list.size() <= picolibrary::Fixed_Capacity_Vector::max_size()
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( std::initializer_list<Value> initializer_list ) noexcept
        -> Fixed_Capacity_Vector &
    {
        assign( initializer_list );

        return *this;
    }

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Fixed_Capacity_Vector && expression ) noexcept -> Fixed_Capacity_Vector &
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
    constexpr auto operator=( Fixed_Capacity_Vector const & expression ) noexcept -> Fixed_Capacity_Vector &
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
     * \pre n <= picolibrary::Fixed_Capacity_Vector::max_size()
     */
    constexpr void assign( Size n, Value const & value ) noexcept
    {
        expect( n <= max_size(), Generic_Error::INSUFFICIENT_CAPACITY );

        clear();

        m_size = n;

        while ( n-- ) { new ( &m_storage[ n ] ) Value{ value }; } // while
    }

    /**
     * \brief Replace the vector's contents with those in the provided range.
     *
     * \tparam Iterator Range iterator.
     *
     * \param[in] begin The beginning of the range of values to store in the vector.
     * \param[in] end The end of the range of values to store in the vector.
     *
     * \pre static_cast<picolibrary::Fixed_Capacity_Vector::Size>( end - begin ) <=
     *      picolibrary::Fixed_Capacity_Vector::max_size()
     */
    template<typename Iterator>
    constexpr void assign( Iterator begin, Iterator end ) noexcept
    {
        auto const n = static_cast<Size>( end - begin );

        expect( n <= max_size(), Generic_Error::INSUFFICIENT_CAPACITY );

        clear();

        m_size = n;

        ::picolibrary::for_each( begin, end, [ storage = &m_storage[ 0 ] ]( auto value ) mutable noexcept {
            new ( storage++ ) Value{ value };
        } );
    }

    /**
     * \brief Replace the vector's contents with those in the provided initializer list.
     *
     * \param[in] initializer_list The initializer list containing the values to store in
     *            the vector.
     *
     * \pre initializer_list.size() <= picolibrary::Fixed_Capacity_Vector::max_size()
     */
    constexpr void assign( std::initializer_list<Value> initializer_list ) noexcept
    {
        assign( initializer_list.begin(), initializer_list.end() );
    }

    /**
     * \brief Access the element at the specified position in the vector.
     *
     * \warning This function does not perform bounds checking. Use
     *          picolibrary::Fixed_Capacity_Vector::at() if bounds checking is desired.
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
     * \warning This function does not perform bounds checking. Use
     *          picolibrary::Fixed_Capacity_Vector::at() if bounds checking is desired.
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
     * \pre position < picolibrary::Fixed_Capacity_Vector::size()
     *
     * \return The element at the specified position in the vector.
     */
    auto at( Position position ) noexcept -> Reference
    {
        expect( position < size(), Generic_Error::OUT_OF_RANGE );

        return data()[ position ];
    }

    /**
     * \brief Access the element at the specified position in the vector.
     *
     * \param[in] position The position of the vector element to access.
     *
     * \pre position < picolibrary::Fixed_Capacity_Vector::size()
     *
     * \return The element at the specified position in the vector.
     */
    auto at( Position position ) const noexcept -> Const_Reference
    {
        expect( position < size(), Generic_Error::OUT_OF_RANGE );

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
        return Reverse_Iterator{ end() };
    }

    /**
     * \brief Get an iterator to the first element of the reversed vector.
     *
     * \return An iterator to the first element of the reversed vector.
     */
    auto rbegin() const noexcept -> Const_Reverse_Iterator
    {
        return Const_Reverse_Iterator{ end() };
    }

    /**
     * \brief Get an iterator to the first element of the reversed vector.
     *
     * \return An iterator to the first element of the reversed vector.
     */
    auto crbegin() const noexcept -> Const_Reverse_Iterator
    {
        return Const_Reverse_Iterator{ cend() };
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
        return Reverse_Iterator{ begin() };
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
        return Const_Reverse_Iterator{ begin() };
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
        return Const_Reverse_Iterator{ cbegin() };
    }

    /**
     * \brief Check if the vector is empty.
     *
     * \return true if the vector is empty.
     * \return false if the vector is not empty.
     */
    [[nodiscard]] constexpr auto empty() const noexcept -> bool
    {
        return size() == 0;
    }

    /**
     * \brief Check if the vector is full.
     *
     * \return true if the vector is full.
     * \return false if the vector is not full.
     */
    [[nodiscard]] constexpr auto full() const noexcept -> bool
    {
        return size() == max_size();
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
        for ( auto i = Size{}; i < m_size; ++i ) { data()[ i ].~Value(); } // for

        m_size = 0;
    }

    /**
     * \brief Insert a value before the specified position in the vector.
     *
     * \pre picolibrary::Fixed_Capacity_Vector::size() + 1 >
     *      picolibrary::Fixed_Capacity_Vector::size() and
     *      picolibrary::Fixed_Capacity_Vector::size() + 1 <=
     *      picolibrary::Fixed_Capacity_Vector::max_size()
     *
     * \param[in] position The position to insert the value before.
     * \param[in] value The value to insert.
     *
     * \return An iterator to the inserted element.
     */
    auto insert( Const_Iterator position, Value const & value ) noexcept -> Iterator
    {
        return insert( position, 1, value );
    }

    /**
     * \brief Insert a value before the specified position in the vector.
     *
     * \pre picolibrary::Fixed_Capacity_Vector::size() + 1 >
     *      picolibrary::Fixed_Capacity_Vector::size() and
     *      picolibrary::Fixed_Capacity_Vector::size() + 1 <=
     *      picolibrary::Fixed_Capacity_Vector::max_size()
     *
     * \param[in] position The position to insert the value before.
     * \param[in] value The value to insert.
     *
     * \return An iterator to the inserted element.
     */
    auto insert( Const_Iterator position, Value && value ) noexcept -> Iterator
    {
        expect( size() + 1 > size() and size() + 1 <= max_size(), Generic_Error::INSUFFICIENT_CAPACITY );

        auto element = --end();

        for ( ; element >= position; --element ) {
            new ( element + 1 ) Value{ std::move( *element ) };

            element->~Value();
        } // for

        auto const inserted_element = element + 1;

        m_size += 1;

        new ( ++element ) Value{ std::move( value ) };

        return inserted_element;
    }

    /**
     * \brief Insert n copies of a value before the specified position in the vector.
     *
     * \param[in] position The position to insert n copies of value before.
     * \param[in] n The number of copies of value to insert.
     * \param[in] value The value to insert n copies of.
     *
     * \pre picolibrary::Fixed_Capacity_Vector::size() + n >
     *      picolibrary::Fixed_Capacity_Vector::size() and
     *      picolibrary::Fixed_Capacity_Vector::size() + n <=
     *      picolibrary::Fixed_Capacity_Vector::max_size()
     *
     * \return An iterator to the inserted elements.
     */
    auto insert( Const_Iterator position, Size n, Value const & value ) noexcept -> Iterator
    {
        expect( size() + n > size() and size() + n <= max_size(), Generic_Error::INSUFFICIENT_CAPACITY );

        auto element = --end();

        for ( ; element >= position; --element ) {
            new ( element + n ) Value{ std::move( *element ) };

            element->~Value();
        } // for

        auto const inserted_elements = element + 1;

        m_size += n;

        while ( n-- ) { new ( ++element ) Value{ value }; } // while

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
     * \pre picolibrary::Fixed_Capacity_Vector::size() +
     *      static_cast<picolibrary::Fixed_Capacity_Vector::Size>( end - begin ) >
     *      picolibrary::Fixed_Capacity_Vector::size() and
     *      picolibrary::Fixed_Capacity_Vector::size() +
     *      static_cast<picolibrary::Fixed_Capacity_Vector::Size>( end - begin ) <=
     *      picolibrary::Fixed_Capacity_Vector::max_size()
     *
     * \return An iterator to the inserted elements.
     */
    template<typename Iterator>
    auto insert( Const_Iterator position, Iterator begin, Iterator end ) noexcept -> Iterator
    {
        auto const n = static_cast<Size>( end - begin );

        expect( size() + n > size() and size() + n <= max_size(), Generic_Error::INSUFFICIENT_CAPACITY );

        auto element = --end();

        for ( ; element >= position; --element ) {
            new ( element + n ) Value{ std::move( *element ) };

            element->~Value();
        } // for

        auto const inserted_elements = element + 1;

        m_size += n;

        ::picolibrary::for_each( begin, end, [ element ]( auto value ) mutable noexcept {
            new ( ++element ) Value{ value };
        } );

        return inserted_elements;
    }

    /**
     * \brief Insert values from the specified initializer list before the specified
     *        position in the vector.
     *
     * \param[in] position The position to insert the values before.
     * \param[in] initializer_list The initializer list containing the values to insert.
     *
     * \pre picolibrary::Fixed_Capacity_Vector::size() + initializer_list.size() >
     *      picolibrary::Fixed_Capacity_Vector::size() and
     *      picolibrary::Fixed_Capacity_Vector::size() + initializer_list.size() <=
     *      picolibrary::Fixed_Capacity_Vector::max_size()
     *
     * \return An iterator to the inserted elements.
     */
    auto insert( Const_Iterator position, std::initializer_list<Value> initializer_list ) noexcept -> Iterator
    {
        return insert( position, initializer_list.begin(), initializer_list.end() );
    }

    /**
     * \brief Emplace a value before the specified position in the vector.
     *
     * \pre picolibrary::Fixed_Capacity_Vector::size() + 1 >
     *      picolibrary::Fixed_Capacity_Vector::size() and
     *      picolibrary::Fixed_Capacity_Vector::size() + 1 <=
     *      picolibrary::Fixed_Capacity_Vector::max_size()
     *
     * \tparam Arguments Value construction argument types.
     *
     * \param[in] position The position to emplace the new value before.
     * \param[in] arguments Value construction arguments.
     *
     * \return An iterator to the emplaced element.
     */
    template<typename... Arguments>
    auto emplace( Const_Iterator position, Arguments &&... arguments ) noexcept -> Iterator
    {
        expect( size() + 1 > size() and size() + 1 <= max_size(), Generic_Error::INSUFFICIENT_CAPACITY );

        auto element = --end();

        for ( ; element >= position; --element ) {
            new ( element + 1 ) Value{ std::move( *element ) };

            element->~Value();
        } // for

        auto const inserted_element = element + 1;

        new ( ++element ) Value{ std::forward<Arguments>( arguments )... };

        m_size += 1;

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

        m_size -= 1;

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
        auto const n = end - begin();

        if ( n ) {
            auto element = this->begin() + ( begin - this->begin() );

            for ( ; element != end; ++element ) { element->~Value(); } // for

            for ( ; element != this->end(); ++element ) {
                new ( element - n ) Value{ std::move( *element ) };

                element->~Value();
            } // for

            m_size -= n;
        } // if

        return this->begin() + ( begin - this->begin() );
    }

    /**
     * \brief Append a value to the vector.
     *
     * \pre picolibrary::Fixed_Capacity_Vector::size() + 1 >
     *      picolibrary::Fixed_Capacity_Vector::size() and
     *      picolibrary::Fixed_Capacity_Vector::size() + 1 <=
     *      picolibrary::Fixed_Capacity_Vector::max_size()
     *
     * \param[in] value The value to append to the vector.
     */
    constexpr void push_back( Value const & value ) noexcept
    {
        expect( size() + 1 > size() and size() + 1 <= max_size(), Generic_Error::INSUFFICIENT_CAPACITY );

        new ( &m_storage[ m_size ] ) Value{ value };

        ++m_size;
    }

    /**
     * \brief Append a value to the vector.
     *
     * \pre picolibrary::Fixed_Capacity_Vector::size() + 1 >
     *      picolibrary::Fixed_Capacity_Vector::size() and
     *      picolibrary::Fixed_Capacity_Vector::size() + 1 <=
     *      picolibrary::Fixed_Capacity_Vector::max_size()
     *
     * \param[in] value The value to append to the vector.
     */
    constexpr void push_back( Value && value ) noexcept
    {
        expect( size() + 1 > size() and size() + 1 <= max_size(), Generic_Error::INSUFFICIENT_CAPACITY );

        new ( &m_storage[ m_size ] ) Value{ std::move( value ) };

        ++m_size;
    }

    /**
     * \brief Append a value to the vector.
     *
     * \pre picolibrary::Fixed_Capacity_Vector::size() + 1 >
     *      picolibrary::Fixed_Capacity_Vector::size() and
     *      picolibrary::Fixed_Capacity_Vector::size() + 1 <=
     *      picolibrary::Fixed_Capacity_Vector::max_size()
     *
     * \tparam Arguments Value construction argument types.
     *
     * \param[in] arguments Value construction arguments.
     *
     * \return A reference to the appended element.
     */
    template<typename... Arguments>
    auto emplace_back( Arguments &&... arguments ) noexcept -> Reference
    {
        expect( size() + 1 > size() and size() + 1 <= max_size(), Generic_Error::INSUFFICIENT_CAPACITY );

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
     * \pre size <= picolibrary::Fixed_Size_Vector::max_size()
     */
    constexpr void resize( Size size ) noexcept
    {
        expect( size <= max_size(), Generic_Error::INSUFFICIENT_CAPACITY );

        if ( size < m_size ) {
            for ( auto element = begin() + size; element != end(); ++element ) {
                element->~Value();
            } // for
        }     // if

        if ( size > m_size ) {
            for ( auto element = end(); element != begin() + size; ++element ) {
                new ( element ) Value{};
            } // for
        }     // if

        m_size = size;
    }

    /**
     * \brief Resize the vector to be the specified size.
     *
     * \param[in] size The desired vector size.
     * \param[in] value The value to append to the vector if the desired vector size is
     *            greater than the current vector size.
     *
     * \pre size <= picolibrary::Fixed_Size_Vector::max_size()
     */
    constexpr void resize( Size size, Value const & value ) noexcept
    {
        expect( size <= max_size(), Generic_Error::INSUFFICIENT_CAPACITY );

        if ( size < m_size ) {
            for ( auto element = begin() + size; element != end(); ++element ) {
                element->~Value();
            } // for
        }     // if

        if ( size > m_size ) {
            for ( auto element = end(); element != begin() + size; ++element ) {
                new ( element ) Value{ value };
            } // for
        }     // if

        m_size = size;
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
 * \tparam T The fixed capacity vector whose size is to be gotten.
 */
template<typename T>
struct fixed_capacity_vector_capacity {
};

/**
 * \brief Get the capacity of a fixed capacity vector.
 *
 * \relatedalso picolibrary::Fixed_Capacity_Vector
 *
 * \tparam T The fixed capacity vector whose size is to be gotten.
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
 * \brief Equality operator.
 *
 * \relatedalso picolibrary::Fixed_Capacity_Vector
 *
 * \tparam T The vector element type.
 * \tparam LHS_N The maximum number of elements in the vector on the left hand side of the
 *         comparison.
 * \tparam RHS_N The maximum number of elements in the vector on the right hand side of
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

    return ::picolibrary::equal( lhs.begin(), lhs.end(), rhs.begin() );
}

/**
 * \brief Inequality operator.
 *
 * \relatedalso picolibrary::Fixed_Capacity_Vector
 *
 * \tparam T The vector element type.
 * \tparam LHS_N The maximum number of elements in the vector on the left hand side of the
 *         comparison.
 * \tparam RHS_N The maximum number of elements in the vector on the right hand side of
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

#endif // PICOLIBRARY_FIXED_CAPACITY_VECTOR_H
