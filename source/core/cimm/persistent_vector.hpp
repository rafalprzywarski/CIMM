#pragma once
#include <cstddef>
#include <initializer_list>
#include <memory>
#include <stdexcept>

namespace cimm
{

template <typename T, unsigned short log_num_branches = 5>
class persistent_vector
{
public:

    class const_iterator
    {
    public:
        friend bool operator==(const const_iterator& , const const_iterator& ) { return true; }
    };

    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using const_reference = const T&;
    using reference = const_reference;
    using iterator = const_iterator;

    persistent_vector() = default;
    persistent_vector(const std::initializer_list<T>& elems);

    size_type size() const { return count; }
    bool empty() const { return size() == 0; }
    const_reference at(size_type index) const
    {
        if (index >= count)
            throw std::out_of_range("persistent_vector: out of range");
        return *reinterpret_cast<const value_type *>(static_cast<const leaf&>(*root).elems);
    }
    const_reference operator[](size_type index) const;
    const_reference front() const;
    const_reference back() const;

    const_iterator begin() const { return {}; }
    const_iterator end() const { return {}; }

    persistent_vector insert(const_iterator pos, const T& value) const;
    template <typename InputIterator>
    persistent_vector insert(const_iterator pos, InputIterator first, InputIterator last) const;

    persistent_vector erase(const_iterator pos) const;
    persistent_vector erase(const_iterator first, const_iterator last) const;

    persistent_vector push_back(const T& elem) const
    {
        auto l = std::make_shared<leaf>();
        new(l->elems) value_type(elem);
        return {l, 1};
    }
    persistent_vector pop_back() const;
private:

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
    size_type count = 0;

    persistent_vector(ptr<element> root, size_type count) : root(std::move(root)), count(count) { }
};

}
