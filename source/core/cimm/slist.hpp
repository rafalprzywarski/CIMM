#pragma once
#include "expression.hpp"

namespace cimm
{

class slist
{
public:
    slist() = default;
    slist(expression e) : node_(std::make_shared<node>(std::move(e))), count_(1) { }
    integer count() const { return count_; }
    expression first() const { return node_ ? node_->value : nil; }
    auto cons(expression e) const { return slist{std::make_shared<node>(std::move(e), node_), count_ + 1}; }
    auto next() const { return node_ ? slist{node_->next, count_ - 1} : slist{}; }

private:

    struct node
    {
        const expression value;
        const std::shared_ptr<node> next;

        node(expression value, std::shared_ptr<node> next = nullptr)
            : value(std::move(value)), next(std::move(next)) { }
    };

    std::shared_ptr<node> node_;
    integer count_ = 0;

    slist(std::shared_ptr<node> node_, integer count_) : node_(node_), count_(count_) { }
    slist(expression e, std::shared_ptr<node> node_) : node_(std::make_shared<node>(e, node_)) { }
};

}
