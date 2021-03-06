#pragma once
#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <array>
#include <atomic>

namespace cimm
{

namespace detail
{

template <typename T, unsigned short log_num_branches>
class persistent_vector_common
{
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using const_reference = const T&;
    using reference = const_reference;
    using pointer = const value_type *;

protected:

    static const size_type num_branches = size_type(1) << log_num_branches;
    static const size_type index_mask = num_branches - 1;

    template <typename U>
    class ptr
    {
    public:
        ptr() = default;

        ptr(std::nullptr_t) { }

        ptr(const ptr<U>& other) : p(other.p) { if (p) ++p->refCount; }

        ptr(ptr<U>&& other) { std::swap(p, other.p); }

        template <typename X>
        ptr(const ptr<X>& other) : p(&*other) { if (p) ++p->refCount; }

        ptr<U>& operator=(ptr<U> other)
        {
            std::swap(p, other.p);
            return *this;
        }

        ~ptr()
        {
            if (p && !--p->refCount)
                delete p;
        }

        template <typename... Args>
        static ptr<U> make(Args&&... args)
        {
            return ptr<U>{new U{std::forward<Args>(args)...}};
        }

        explicit operator bool() const { return p; }

        U& operator*() const { return *p; }

    private:
        explicit ptr(U *p) : p(p) { }

        U *p = nullptr;
    };

    struct element
    {
        std::atomic<size_type> refCount{1};
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
            if (index == 0 && !n)
                return nullptr;
            auto p = ptr<node>::make();
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
            auto p = ptr<leaf>::make();
            auto& new_leaf = *p;
            for (size_type i = 0; i < count; ++i)
                new_leaf.push_back(get(i));
            new_leaf.push_back(elem);
            return p;
        }

        ptr<leaf> pop_back_new() const
        {
            if (count == 0)
                return nullptr;
            auto p = ptr<leaf>::make();
            auto& new_leaf = *p;
            for (size_type i = 0; i < (count - 1); ++i)
                new_leaf.push_back(get(i));
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

    static ptr<element> push_back_leaf(const ptr<element> parent, const T& elem, size_type index, size_type shift)
    {
        if (!parent)
            return create_element(elem, shift);
        if (shift == 0)
            return as_leaf(parent).push_back_new(elem);
        auto& parent_node = as_node(parent);
        auto node_index = local_index(index, shift);
        return parent_node.replace_new(node_index, push_back_leaf(parent_node.elems.at(node_index), elem, index, shift - log_num_branches));
    }

    static ptr<element> pop_back_leaf(const ptr<element> parent, size_type index, size_type shift)
    {
        if (shift == 0)
            return as_leaf(parent).pop_back_new();
        auto& parent_node = as_node(parent);
        auto node_index = local_index(index, shift);
        return parent_node.replace_new(node_index, pop_back_leaf(parent_node.elems.at(node_index), index, shift - log_num_branches));
    }

    static size_type local_index(size_type index, size_type shift = 0)
    {
        return (index >> shift) & index_mask;
    }

    const value_type& get(const ptr<element>& elem, size_type index, size_type shift) const
    {
        if (shift == 0)
            return as_leaf(elem).get(local_index(index));
        return get(as_node(elem).elems.at(local_index(index, shift)), index, shift - log_num_branches);
    }

    static ptr<element> create_element(const value_type& elem, size_type shift)
    {
        if (shift == 0)
            return ptr<leaf>::make(elem);
        return ptr<node>::make(create_element(elem, shift - log_num_branches));
    }
};

}

template <typename T, unsigned short log_num_branches = 5>
class persistent_vector : public detail::persistent_vector_common<T, log_num_branches>
{
    using base = detail::persistent_vector_common<T, log_num_branches>;
public:

    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using const_reference = const T&;
    using reference = const_reference;
    using pointer = const value_type *;

    class const_iterator : public std::iterator<std::random_access_iterator_tag, const value_type, difference_type>
    {
    public:
        const_iterator(const persistent_vector<T, log_num_branches>& container, size_type index)
            : container(&container), index(index) { }
        friend bool operator==(const const_iterator& left, const const_iterator& right) { return left.index == right.index; }
        friend bool operator!=(const const_iterator& left, const const_iterator& right) { return !(left == right); }
        friend difference_type operator-(const const_iterator& left, const const_iterator& right)
        {
            return left.index - right.index;
        }
        const_iterator& operator+=(difference_type n) { index += n; return *this; }
        friend const_iterator operator+(const_iterator left, difference_type right) { return left += right; }
        const_iterator& operator-=(difference_type n) { index -= n; return *this; }
        friend const_iterator operator-(const_iterator left, difference_type right) { return left -= right; }
        const_iterator& operator++() { ++index; return *this; }
        const_iterator operator++(int) { auto prev = *this; ++*this; return prev; }
        const_iterator& operator--() { --index; return *this; }
        const_iterator operator--(int) { auto prev = *this; --*this; return prev; }
        reference operator*() const { return (*container)[index]; }
        pointer operator->() const { return &**this; }
        reference operator[](difference_type index) const { return *(*this + index); }
    private:
        const persistent_vector<T, log_num_branches> *container;
        size_type index;
    };

    using iterator = const_iterator;

    persistent_vector() = default;

    template <typename InputIterator>
    persistent_vector(InputIterator first, InputIterator last)
    {
        persistent_vector<T, log_num_branches> v;
        for (; first != last; ++first)
            v = v.push_back(*first);
        *this = std::move(v);
    }

    persistent_vector(const std::initializer_list<T>& elems) : persistent_vector(std::begin(elems), std::end(elems)) { }

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
        return base::get(root, index, shift);
    }

    const_reference front() const
    {
        return (*this)[0];
    }

    const_reference back() const
    {
        return (*this)[count - 1];
    }

    const_iterator begin() const { return {*this, 0}; }
    const_iterator end() const { return {*this, count}; }

    persistent_vector insert(const_iterator pos, const T& value) const;
    template <typename InputIterator>
    persistent_vector insert(const_iterator pos, InputIterator first, InputIterator last) const;

    persistent_vector erase(const_iterator pos) const;
    persistent_vector erase(const_iterator first, const_iterator last) const;

    persistent_vector push_back(const T& elem) const
    {
        if (count == (num_branches << shift))
        {
            return persistent_vector{ptr<node>::make(root, base::create_element(elem, shift)), count + 1, shift + log_num_branches};
        }

        return persistent_vector{base::push_back_leaf(root, elem, count, shift), count + 1, shift};
    }

    persistent_vector pop_back() const
    {
        if (count == (num_branches << shift) + 1)
            return persistent_vector{base::as_node(root).elems[0], count - 1, shift - log_num_branches};
        return persistent_vector{base::pop_back_leaf(root, count - 1, shift), count - 1, shift};
    }

private:

    static const size_type num_branches = size_type(1) << log_num_branches;
    static const size_type index_mask = num_branches - 1;

    using element = typename base::element;
    using leaf = typename base::leaf;
    using node = typename base::node;
    template <typename U>
    using ptr = typename base::template ptr<U>;

    ptr<element> root;
    size_type count = 0;
    size_type shift = 0;

    persistent_vector(ptr<element> root, size_type count, size_type shift) : root(std::move(root)), count(count), shift(shift) { }
};

template <typename T, unsigned short log_num_branches>
inline bool operator==(const persistent_vector<T, log_num_branches>& left, const persistent_vector<T, log_num_branches>& right)
{
    return left.size() == right.size() && std::equal(left.begin(), left.end(), right.begin());
}

template <typename T, unsigned short log_num_branches>
inline bool operator!=(const persistent_vector<T, log_num_branches>& left, const persistent_vector<T, log_num_branches>& right)
{
    return !(left == right);
}

template <typename T, unsigned short log_num_branches = 5>
class transient_vector : public detail::persistent_vector_common<T, log_num_branches>
{
    using base = detail::persistent_vector_common<T, log_num_branches>;
public:

    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using const_reference = const T&;
    using reference = const_reference;
    using pointer = const value_type *;

    class const_iterator : public std::iterator<std::random_access_iterator_tag, const value_type, difference_type>
    {
    public:
        const_iterator(const transient_vector<T, log_num_branches>& container, size_type index)
            : container(&container), index(index) { }
        friend bool operator==(const const_iterator& left, const const_iterator& right) { return left.index == right.index; }
        friend bool operator!=(const const_iterator& left, const const_iterator& right) { return !(left == right); }
        friend difference_type operator-(const const_iterator& left, const const_iterator& right)
        {
            return left.index - right.index;
        }
        const_iterator& operator+=(difference_type n) { index += n; return *this; }
        friend const_iterator operator+(const_iterator left, difference_type right) { return left += right; }
        const_iterator& operator-=(difference_type n) { index -= n; return *this; }
        friend const_iterator operator-(const_iterator left, difference_type right) { return left -= right; }
        const_iterator& operator++() { ++index; return *this; }
        const_iterator operator++(int) { auto prev = *this; ++*this; return prev; }
        const_iterator& operator--() { --index; return *this; }
        const_iterator operator--(int) { auto prev = *this; --*this; return prev; }
        reference operator*() const { return (*container)[index]; }
        pointer operator->() const { return &**this; }
        reference operator[](difference_type index) const { return *(*this + index); }
    private:
        const transient_vector<T, log_num_branches> *container;
        size_type index;
    };

    using iterator = const_iterator;

    transient_vector() = default;
    transient_vector(const transient_vector& ) = delete;
    transient_vector(transient_vector&& ) = default;

    template <typename InputIterator>
    transient_vector(InputIterator first, InputIterator last)
    {
        for (; first != last; ++first)
            push_back(*first);
    }

    transient_vector(const std::initializer_list<T>& elems) : transient_vector(std::begin(elems), std::end(elems)) { }

    transient_vector& operator=(const transient_vector& ) = delete;
    transient_vector& operator=(transient_vector&& ) = default;

    persistent_vector<T, log_num_branches> persistent() &&;

    size_type size() const { return count; }
    bool empty() const { return size() == 0; }
    const_reference at(size_type index) const
    {
        if (index >= count)
            throw std::out_of_range("transient_vector: out of range");
        return (*this)[index];
    }

    const_reference operator[](size_type index) const
    {
        return base::get(root, index, shift);
    }

    const_reference front() const
    {
        return (*this)[0];
    }

    const_reference back() const
    {
        return (*this)[count - 1];
    }

    const_iterator begin() const { return {*this, 0}; }
    const_iterator end() const { return {*this, count}; }

    transient_vector insert(const_iterator pos, const T& value) const;
    template <typename InputIterator>
    transient_vector insert(const_iterator pos, InputIterator first, InputIterator last) const;

    transient_vector erase(const_iterator pos) const;
    transient_vector erase(const_iterator first, const_iterator last) const;

    void push_back(const T& elem)
    {
        if (count == (num_branches << shift))
        {
            *this = transient_vector{ptr<node>::make(root, base::create_element(elem, shift)), count + 1, shift + log_num_branches};
            return;
        }

        *this = transient_vector{base::push_back_leaf(root, elem, count, shift), count + 1, shift};
    }

    void pop_back()
    {
        if (count == (num_branches << shift) + 1)
        {
            *this = transient_vector{base::as_node(root).elems[0], count - 1, shift - log_num_branches};
            return;
        }
        *this = transient_vector{base::pop_back_leaf(root, count - 1, shift), count - 1, shift};
    }

private:

    static const size_type num_branches = size_type(1) << log_num_branches;
    static const size_type index_mask = num_branches - 1;

    using element = typename base::element;
    using leaf = typename base::leaf;
    using node = typename base::node;
    template <typename U>
    using ptr = typename base::template ptr<U>;

    ptr<element> root;
    size_type count = 0;
    size_type shift = 0;

    transient_vector(ptr<element> root, size_type count, size_type shift) : root(std::move(root)), count(count), shift(shift) { }
};

template <typename T, unsigned short log_num_branches>
inline bool operator==(const transient_vector<T, log_num_branches>& left, const transient_vector<T, log_num_branches>& right)
{
    return left.size() == right.size() && std::equal(left.begin(), left.end(), right.begin());
}

template <typename T, unsigned short log_num_branches>
inline bool operator!=(const transient_vector<T, log_num_branches>& left, const transient_vector<T, log_num_branches>& right)
{
    return !(left == right);
}

}
