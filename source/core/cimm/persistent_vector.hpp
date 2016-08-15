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
        return get(*root, index, shift);
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
        if (count == 0)
            return {std::make_shared<leaf>(elem), 1, 0};
        if (count < num_branches)
        {
            auto l = static_cast<const leaf&>(*root).push_back_new(elem);
            return {l, count + 1, shift};
        }
        if (count == num_branches)
        {
            return persistent_vector{std::make_shared<node>(root, std::make_shared<leaf>(elem)), count + 1, shift + log_num_branches};
        }
        if (count < num_branches * num_branches)
        {
            auto& root_node = static_cast<const node&>(*root);
            auto node_index = count >> log_num_branches;
            if ((count & index_mask) == 0)
            {
                return persistent_vector{root_node.replace_new(node_index, std::make_shared<leaf>(elem)), count + 1, shift};
            }
            return persistent_vector{root_node.replace_new(node_index, static_cast<const leaf&>(*root_node.elems[node_index]).push_back_new(elem)), count + 1, shift};
        }
        if (count == num_branches * num_branches)
        {
            return persistent_vector{std::make_shared<node>(root, std::make_shared<node>(std::make_shared<leaf>(elem))), count + 1, shift + log_num_branches};
        }
        auto node_index = (count >> log_num_branches) >> log_num_branches;
        auto child_index = (count >> log_num_branches) & index_mask;
        if ((count & index_mask) == 0)
        {
            auto& root_node = static_cast<const node&>(*root);
            auto& child_node = static_cast<const node&>(*root_node.elems[node_index]);
            return persistent_vector{root_node.replace_new(node_index, child_node.replace_new(child_index, std::make_shared<leaf>(elem))), count + 1, shift};
        }
        auto& root_node = static_cast<const node&>(*root);
        auto& child_node = static_cast<const node&>(*root_node.elems[node_index]);
        auto& l = static_cast<const leaf&>(*child_node.elems[child_index]);
        return persistent_vector{root_node.replace_new(node_index, child_node.replace_new(child_index, l.push_back_new(elem))), count + 1, shift};
    }
    persistent_vector pop_back() const;
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
        ptr<element> elems[num_branches];

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
            auto p = std::make_shared<node>();
            auto& new_node = *p;
            for (size_type i = 0; i < index; ++i)
                new_node.elems[i] = elems[i];
            new_node.elems[index] = std::move(n);
            return p;
        }
    };

    struct leaf : element
    {
        typename std::aligned_storage<sizeof(value_type), alignof(value_type)>::type elems[num_branches];
        size_type count = 0;

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

        const value_type& get(size_type index) const { return *reinterpret_cast<const value_type *>(elems + index); }
    };

    ptr<element> root;
    size_type count = 0;
    size_type shift = 0;

    persistent_vector(ptr<element> root, size_type count, size_type shift) : root(std::move(root)), count(count), shift(shift) { }

    const value_type& get(const element& elem, size_type index, size_type shift) const
    {
        if (shift == 0)
            return static_cast<const leaf&>(elem).get(index & index_mask);
        auto node_index = (index >> shift) & index_mask;
        return get(*static_cast<const node&>(elem).elems[node_index], index, shift - log_num_branches);
    }
};

}
