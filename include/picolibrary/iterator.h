/**
 * picolibrary
 *
 * Copyright 2020 Andrew Countryman <apcountryman@gmail.com>
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
 * \brief picolibrary::Iterator interface.
 */

#ifndef PICOLIBRARY_ITERATOR_H
#define PICOLIBRARY_ITERATOR_H

#include <cstddef>
#include <memory>
#include <type_traits>

namespace picolibrary {

/**
 * \brief Input iterator category tag.
 */
struct Input_Iterator_Tag {
};

/**
 * \brief Output iterator category tag.
 */
struct Output_Iterator_Tag {
};

/**
 * \brief Forward iterator category tag.
 */
struct Forward_Iterator_Tag : Input_Iterator_Tag {
};

/**
 * \brief Bidirectional iterator category tag.
 */
struct Bidirectional_Iterator_Tag : Forward_Iterator_Tag {
};

/**
 * \brief Random access iterator category tag.
 */
struct Random_Access_Iterator_Tag : Bidirectional_Iterator_Tag {
};

/**
 * \brief Contiguous iterator category tag.
 */
struct Contiguous_Iterator_Tag : Random_Access_Iterator_Tag {
};

/**
 * \brief Iterator traits.
 *
 * \tparam Iterator The iterator type whose traits will be retrieved.
 */
template<typename Iterator, typename = std::void_t<>>
struct Iterator_Traits {
};

/**
 * \copydoc picolibrary::Iterator_Traits
 */
template<typename Iterator>
struct Iterator_Traits<Iterator, std::void_t<typename Iterator::Difference, typename Iterator::Value, typename Iterator::Pointer, typename Iterator::Reference, typename Iterator::Iterator_Category>> {
    /**
     * \brief Type that can be used to identify the distance between two iterators.
     */
    using Difference = typename Iterator::Difference;

    /**
     * \brief The iterated over type.
     */
    using Value = typename Iterator::Value;

    /**
     * \brief Pointer to the iterated over type.
     */
    using Pointer = typename Iterator::Pointer;

    /**
     * \brief Reference to the iterated over type.
     */
    using Reference = typename Iterator::Reference;

    /**
     * \brief Iterator category tag.
     */
    using Iterator_Category = typename Iterator::Iterator_Category;
};

/**
 * \brief Pointer iterator traits.
 *
 * \tparam T The iterated over type.
 */
template<typename T>
struct Iterator_Traits<T *> {
    /**
     * \brief Type that can be used to identify the distance between two iterators.
     */
    using Difference = std::ptrdiff_t;

    /**
     * \brief The iterated over type.
     */
    using Value = std::remove_cv_t<T>;

    /**
     * \brief Pointer to the iterated over type.
     */
    using Pointer = T *;

    /**
     * \brief Reference to the iterated over type.
     */
    using Reference = T &;

    /**
     * \brief Iterator category tag.
     */
    using Iterator_Category = Contiguous_Iterator_Tag;
};

/**
 * \brief Reverse iterator adapter.
 *
 * The following relationship with the underlying iterator is maintained:
 * \code
 * &*picolibrary::Reverse_Iterator{ iterator } == &*( iterator - 1 )
 * \endcode
 *
 * \tparam Iterator_Type The adapted iterator type.
 */
template<typename Iterator_Type>
class Reverse_Iterator {
  public:
    /**
     * \brief The adapted iterator type.
     */
    using Iterator = Iterator_Type;

    /**
     * \brief Type that can be used to identify the distance between two iterators.
     */
    using Difference = typename Iterator_Traits<Iterator>::Difference;

    /**
     * \brief The iterated over type.
     */
    using Value = typename Iterator_Traits<Iterator>::Value;

    /**
     * \brief Pointer to the iterated over type.
     */
    using Pointer = typename Iterator_Traits<Iterator>::Pointer;

    /**
     * \brief Reference to the iterated over type.
     */
    using Reference = typename Iterator_Traits<Iterator>::Reference;

    /**
     * \brief Iterator category tag.
     */
    using Iterator_Category = typename Iterator_Traits<Iterator>::Iterator_Category;

    /**
     * \brief Element offset.
     */
    using Offset = Difference;

    /**
     * \brief Constructor.
     */
    constexpr Reverse_Iterator() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] iterator The iterator to adapt.
     */
    explicit constexpr Reverse_Iterator( Iterator iterator ) noexcept :
        m_iterator{ iterator }
    {
    }

    /**
     * \brief Constructor.
     *
     * \tparam Other_Iterator An iterator type that can be implicitly converted to
     *         Iterator.
     *
     * \param[in] other The reverse iterator whose adapted iterator will be copied.
     */
    template<typename Other_Iterator, typename std::enable_if_t<not std::is_same_v<std::decay_t<Other_Iterator>, Iterator>>>
    constexpr Reverse_Iterator( Reverse_Iterator<Other_Iterator> const & other ) noexcept :
        m_iterator{ other.m_iterator }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Reverse_Iterator( Reverse_Iterator && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Reverse_Iterator( Reverse_Iterator const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Reverse_Iterator() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \tparam Other_Iterator An iterator type that can be implicitly converted to
     *         Iterator.
     *
     * \param[in] other The reverse iterator whose adapted iterator will be assigned.
     *
     * \return The assigned to object.
     */
    template<typename Other_Iterator, typename std::enable_if_t<not std::is_same_v<std::decay_t<Other_Iterator>, Iterator>>>
    constexpr auto & operator=( Reverse_Iterator<Other_Iterator> const & other ) noexcept
    {
        m_iterator = other.m_iterator;

        return *this;
    }

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Reverse_Iterator && expression ) noexcept -> Reverse_Iterator & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator   =( Reverse_Iterator const & expression ) noexcept
        -> Reverse_Iterator & = default;

    /**
     * \brief Get a copy of the adapted iterator.
     *
     * \return A copy of the adapted iterator.
     */
    constexpr auto base() const noexcept
    {
        return m_iterator;
    }

    /**
     * \brief Access the element at the current iterator position.
     *
     * \return The element at the current iterator position.
     */
    constexpr auto operator*() const noexcept -> Reference
    {
        auto iterator = m_iterator;

        return *--iterator;
    }

    /**
     * \brief Access the element at the current iterator position.
     *
     * \return A pointer to the element at the current iterator position.
     */
    constexpr auto operator-> () const noexcept -> Pointer
    {
        return std::addressof( operator*() );
    }

    /**
     * \brief Access the element at the specified offset from the current iterator
     *        position..
     *
     * \param[in] offset The offset of the element to access.
     *
     * \return The element at the specified offset from the current iterator position.
     */
    constexpr auto operator[]( Offset offset ) const noexcept -> Reference
    {
        return m_iterator[ -offset - 1 ];
    }

    /**
     * \brief Advance the iterator to the next element.
     *
     * \return The advanced iterator.
     */
    constexpr auto & operator++() noexcept
    {
        --m_iterator;

        return *this;
    }

    /**
     * \brief Retreat the iterator to the previous element.
     *
     * \return The retreated iterator.
     */
    constexpr auto & operator--() noexcept
    {
        ++m_iterator;

        return *this;
    }

    /**
     * \brief Advance the iterator to the next element.
     *
     * \return A copy of the iterator that was made before the iterator was advanced.
     */
    constexpr auto operator++( int ) noexcept
    {
        auto iterator = m_iterator;

        --m_iterator;

        return iterator;
    }

    /**
     * \brief Retreat the iterator to the previous element.
     *
     * \return A copy of the iterator that was made before the iterator was retreated.
     */
    constexpr auto operator--( int ) noexcept
    {
        auto iterator = m_iterator;

        ++m_iterator;

        return iterator;
    }

    /**
     * \brief Advance a copy of the iterator n positions.
     *
     * \param[in] n The number of positions to advance the iterator.
     *
     * \return A copy of the iterator that has been advanced n positions.
     */
    constexpr auto operator+( Offset n ) const noexcept
    {
        return Reverse_Iterator{ m_iterator - n };
    }

    /**
     * \brief Retreat a copy of the iterator n positions.
     *
     * \param[in] n The number of positions to retreat the iterator.
     *
     * \return A copy of the iterator that has been retreated n positions.
     */
    constexpr auto operator-( Offset n ) const noexcept
    {
        return Reverse_Iterator{ m_iterator + n };
    }

    /**
     * \brief Advance the iterator n positions.
     *
     * \param[in] n The number of positions to advance the iterator.
     *
     * \return The advanced iterator.
     */
    constexpr auto & operator+=( Offset n ) noexcept
    {
        m_iterator -= n;

        return *this;
    }

  protected:
    /**
     * \brief The adapted iterator.
     */
    Iterator m_iterator{};
};

} // namespace picolibrary

#endif // PICOLIBRARY_ITERATOR_H
