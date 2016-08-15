#pragma once
#include <cstddef>
#include <initializer_list>
#include <memory>

namespace cimm
{

template <typename T>
class persistent_vector
{
public:

    class const_iterator { };

    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using const_reference = const T&;
    using reference = const_reference;
    using iterator = const_iterator;

    persistent_vector() = default;
    persistent_vector(const std::initializer_list<T>& elems);

    size_type size() const;
    bool empty() const;
    const_reference at(size_type index) const;
    const_reference operator[](size_type index) const;
    const_reference front() const;
    const_reference back() const;

    const_iterator begin() const;
    const_iterator end() const;

    persistent_vector insert(const_iterator pos, const T& value) const;
    template <typename InputIterator>
    persistent_vector insert(const_iterator pos, InputIterator first, InputIterator last) const;

    persistent_vector erase(const_iterator pos) const;
    persistent_vector erase(const_iterator first, const_iterator last) const;

    persistent_vector push_back(const T& elem) const;
    persistent_vector pop_back() const;
private:

    static const unsigned short log_num_branches = 5;
    static const std::size_t num_branches = std::size_t(1) << log_num_branches;

    template <typename U>
    using ptr = std::shared_ptr<U>;

    struct element
    {
        virtual ~element() noexcept = default;
    };

    struct node : element
    {
        ptr<element> elems;
    };

    struct leaf : element
    {
        typename std::aligned_storage<sizeof(value_type), alignof(value_type)>::type elems[num_branches];
    };

    ptr<element> root;
};

}