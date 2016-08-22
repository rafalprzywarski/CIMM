#pragma once
#include <cstddef>
#include <initializer_list>
#include <memory>
#include <stdexcept>
#include <array>

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
    persistent_vector(const std::initializer_list<T>& elems)
    {
        persistent_vector<T, log_num_branches> v;
        for (auto& elem : elems)
            v = v.push_back(elem);
        *this = v;
    }

    size_type size() const { return count; }
    bool empty() const { return size() == 0; }
    const_reference at(size_type index) const
    {
        if (index >= count)
            throw std::out_of_range("persistent_vector: out of range");
        return (*this)[index];
    }

    const_reference operator[](size_type index) const
    {
        return get(root, index, shift);
    }

    const_reference front() const
    {
        return (*this)[0];
    }

    const_reference back() const
    {
        return (*this)[count - 1];
    }

    const_iterator begin() const { return {}; }
    const_iterator end() const { return {}; }

    persistent_vector insert(const_iterator pos, const T& value) const;
    template <typename InputIterator>
    persistent_vector insert(const_iterator pos, InputIterator first, InputIterator last) const;

    persistent_vector erase(const_iterator pos) const;
    persistent_vector erase(const_iterator first, const_iterator last) const;

    persistent_vector push_back(const T& elem) const
    {
        if (count == (num_branches << shift))
        {
            return persistent_vector{std::make_shared<node>(root, create_element(elem, shift)), count + 1, shift + log_num_branches};
        }

        return persistent_vector{push_back_leaf(root, elem, count, shift), count + 1, shift};
    }

    persistent_vector pop_back() const
    {
        return persistent_vector{};
    }

private:

    static const size_type num_branches = size_type(1) << log_num_branches;
    static const size_type index_mask = num_branches - 1;

    template <typename U>
    using ptr = std::shared_ptr<U>;

    struct element
    {
        virtual ~element() noexcept = default;
    };

    struct node : element
    {
        std::array<ptr<element>, num_branches> elems;

        node() = default;

        node(ptr<element> e)
        {
            elems[0] = std::move(e);
        }

        node(ptr<element> e0, ptr<element> e1)
        {
            elems[0] = std::move(e0);
            elems[1] = std::move(e1);
        }

        ptr<node> replace_new(size_type index, ptr<element> n) const
        {
            if (index >= elems.size())
                throw std::invalid_argument("replace_new index too large");
            auto p = std::make_shared<node>();
            auto& new_node = *p;
            for (size_type i = 0; i < index; ++i)
                new_node.elems[i] = elems[i];
            new_node.elems[index] = std::move(n);
            return p;
        }
    };

    class leaf : public element
    {
    public:
        leaf() = default;
        leaf(const value_type& elem)
        {
            push_back(elem);
        }

        ~leaf() noexcept
        {
            for (size_type i = 0; i < count; ++i)
                reinterpret_cast<value_type *>(this->elems + i)->~value_type();
        }

        void push_back(const value_type& elem)
        {
            if (count == num_branches)
                throw std::out_of_range("leaf push_back full");
            new(elems + count) value_type(elem);
            ++count;
        }

        ptr<leaf> push_back_new(const value_type& elem) const
        {
            auto p = std::make_shared<leaf>();
            auto& new_leaf = *p;
            for (size_type i = 0; i < count; ++i)
                new_leaf.push_back(get(i));
            new_leaf.push_back(elem);
            return p;
        }

        const value_type& get(size_type index) const
        {
            if (index >= count)
                throw std::out_of_range("leaf get out of range");
            return *reinterpret_cast<const value_type *>(elems + index); }
    private:
        typename std::aligned_storage<sizeof(value_type), alignof(value_type)>::type elems[num_branches];
        size_type count = 0;
    };

    ptr<element> root;
    size_type count = 0;
    size_type shift = 0;

    persistent_vector(ptr<element> root, size_type count, size_type shift) : root(std::move(root)), count(count), shift(shift) { }

    const value_type& get(const ptr<element>& elem, size_type index, size_type shift) const
    {
        if (shift == 0)
            return as_leaf(elem).get(local_index(index));
        return get(as_node(elem).elems.at(local_index(index, shift)), index, shift - log_num_branches);
    }

    static ptr<element> create_element(const value_type& elem, size_type shift)
    {
        if (shift == 0)
            return std::make_shared<leaf>(elem);
        return std::make_shared<node>(create_element(elem, shift - log_num_branches));
    }

    static const leaf& as_leaf(const ptr<element>& e)
    {
        if (!e)
            throw std::invalid_argument("leaf element is null");
        return dynamic_cast<const leaf&>(*e);
    }

    static const node& as_node(const ptr<element>& e)
    {
        if (!e)
            throw std::invalid_argument("node element is null");
        return dynamic_cast<const node&>(*e);
    }

    ptr<element> push_back_leaf(const ptr<element> parent, const T& elem, size_type index, size_type shift) const
    {
        if (!parent)
            return create_element(elem, shift);
        if (shift == 0)
            return as_leaf(parent).push_back_new(elem);
        auto& parent_node = as_node(parent);
        auto node_index = local_index(index, shift);
        return parent_node.replace_new(node_index, push_back_leaf(parent_node.elems.at(node_index), elem, index, shift - log_num_branches));
    }

    static size_type local_index(size_type index, size_type shift = 0)
    {
        return (index >> shift) & index_mask;
    }
};

}
