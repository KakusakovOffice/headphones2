#pragma once
#include "Headphones.hpp"
#include <memory>
#include <cstdint>
#include <variant>
#include <optional>

class HeadphonesList {
public:
    class Node {
    public:
        using node_ptr = std::shared_ptr<Node>;
        using const_node_ptr = std::shared_ptr<const Node>;

        template<typename... Args>
        Node(Args&&... args) : m_value(std::forward<Args>(args)...) {}

        Headphones& value();
        const Headphones& cvalue() const;
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

    class Iterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = Node::node_ptr;
        using pointer           = value_type*;
        using reference         = value_type&;

        Iterator(value_type ptr);

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
        value_type m_ptr;
    };

    class ConstIterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = Node::const_node_ptr;
        using pointer           = value_type*;
        using reference         = value_type&;

        ConstIterator(value_type ptr);
        ConstIterator(Iterator iter);

        reference operator*();
        pointer operator->();
        ConstIterator& operator++();
        ConstIterator operator++(int);
        ConstIterator& operator--();
        ConstIterator operator--(int);
        friend bool operator== (const ConstIterator& a, const ConstIterator& b);
        friend bool operator!= (const ConstIterator& a, const ConstIterator& b);

        friend void swap(ConstIterator& a, ConstIterator& b);
    private:
        value_type m_ptr;
    };

    HeadphonesList();

    Iterator head();
    Iterator tail();

    ConstIterator chead() const;
    ConstIterator ctail() const;
    std::uintptr_t count() const;
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
        Node::node_ptr node = std::make_shared<Node>(std::forward<Args>(args)...);
        return insert_before(it, node);
    }
    template<typename... Args>
    Iterator emplace_after(Iterator it, Args&&... args)
    {
        Node::node_ptr node = std::make_shared<Node>(std::forward<Args>(args)...);
        return insert_after(it, node);
    }
    Iterator insert_before(Iterator it, Node::node_ptr node);
    Iterator insert_after(Iterator it, Node::node_ptr node);
    void remove(Iterator it);

    class DeserializeError {
    public:
        std::string message;
        DeserializeError(std::string message);
    };
    class SerializeError {
    public:
        std::string message;
        SerializeError(std::string message);
    };

    using DeserializeResult = std::variant<HeadphonesList, DeserializeError>;
    using SerializeResult = std::variant<std::monostate, SerializeError>;

    SerializeResult serialize(std::ostream& os) const;
    static DeserializeResult deserialize(std::istream& is);
private:
    Node::node_ptr m_head;
    Node::node_ptr m_tail;
    std::uintptr_t m_count;

    Iterator insert_internal(
        Node::node_ptr node,
        Node::node_ptr next,
        Node::node_ptr prev
    );
    static std::variant<std::string, HeadphonesList::DeserializeError> deserialize_read_section(std::istream& is);
};
