#pragma once
#include "expression.hpp"

namespace cimm
{

class slist
{
public:
    slist() = default;
    slist(expression e) : node_(std::make_shared<node>(std::move(e))), count_(1) { }
    slist(expression e, slist next) : node_(std::make_shared<node>(std::move(e), std::move(next.node_))), count_(next.count() + 1) { }
    integer count() const { return count_; }
    expression first() const { return node_ ? node_->value : nil; }
    auto cons(expression e) const { return slist{std::move(e), *this}; }
    auto next() const { return node_ ? slist{node_->next, count_ - 1} : slist{}; }

    template <typename F>
    auto map(F&& f) const
    {
        if (count() == 0)
            return *this;

        auto dnode = std::make_shared<node>(f(node_->value));
        slist d{dnode, count()};

        for (auto snode = node_->next; snode; snode = snode->next, dnode = dnode->next)
            dnode->next = std::make_shared<node>(f(snode->value));;

        return d;
    }

    friend auto operator==(const slist& left, const slist& right)
    {
        return left.count() == right.count() && (!left.node_ || *left.node_ == *right.node_);
    }

private:

    struct node
    {
        const expression value;
        std::shared_ptr<node> next;

        node(expression value, std::shared_ptr<node> next = nullptr)
            : value(std::move(value)), next(std::move(next)) { }

        auto operator==(const node& other) const -> bool
        {
            return value == other.value && (!next || *next == *other.next);
        }
    };

    std::shared_ptr<node> node_;
    integer count_ = 0;

    slist(std::shared_ptr<node> node_, integer count_) : node_(node_), count_(count_) { }
    slist(expression e, std::shared_ptr<node> node_) : node_(std::make_shared<node>(e, node_)) { }
};

}
