#pragma once
#include "Headphones.hpp"
#include <memory>

class HeadphonesListNode {
public:
    using node_ptr = std::shared_ptr<HeadphonesListNode>;

    template<typename... Args>
    HeadphonesListNode(Args&&... args) : m_value(std::forward<Args>(args)...) {}

    Headphones& value();
    node_ptr get_next() const;
    node_ptr get_prev() const;

    void set_next(node_ptr next);
    void set_prev(node_ptr prev);
    void disconnect();
private:
    Headphones m_value;
    node_ptr m_next;
    node_ptr m_prev;
};

class HeadphonesList {
public:
    class Iterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = HeadphonesListNode::node_ptr;
        using pointer           = value_type*;
        using reference         = value_type&;

        Iterator(HeadphonesListNode::node_ptr ptr);

        reference operator*();
        pointer operator->();
        Iterator& operator++();
        Iterator operator++(int);
        Iterator& operator--();
        Iterator operator--(int);
        friend bool operator== (const Iterator& a, const Iterator& b);
        friend bool operator!= (const Iterator& a, const Iterator& b);

        friend void swap(Iterator& a, Iterator& b);
    private:
        HeadphonesListNode::node_ptr m_ptr;
    };

    HeadphonesList();

    Iterator head();
    Iterator tail();
    std::ptrdiff_t count() const;
    bool is_empty() const;
    bool is_not_empty() const;

    template<class UnaryPredicate>
    Iterator find_if(Iterator first_inclusive, Iterator last_inclusive, UnaryPredicate p)
    {
        for (Iterator it = first_inclusive; *it; it++) {
            if (p(it->get()))
            {
                return it;
            }
            if (it == last_inclusive)
            {
                break;
            }
        }

        return Iterator(nullptr);
    }
    template<typename... Args>
    Iterator emplace_before(Iterator it, Args&&... args)
    {
        auto next = *it;
        auto prev = (*it)->get_prev();
        HeadphonesListNode::node_ptr node = std::make_shared(std::forward<Args>(args)...);
        return insert_internal(node, next, prev);
    }
    template<typename... Args>
    Iterator emplace_after(Iterator it, Args&&... args)
    {
        auto prev = *it;
        auto next = (*it)->get_next();
        HeadphonesListNode::node_ptr node = std::make_shared(std::forward<Args>(args)...);
        return insert_internal(node, next, prev);
    }
    void remove(Iterator it);
private:
    HeadphonesListNode::node_ptr m_head;
    HeadphonesListNode::node_ptr m_tail;
    std::ptrdiff_t m_count;

    Iterator insert_internal(
        HeadphonesListNode::node_ptr node,
        HeadphonesListNode::node_ptr next,
        HeadphonesListNode::node_ptr prev
    );
};
