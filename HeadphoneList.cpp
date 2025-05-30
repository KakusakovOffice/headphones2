#include "HeadphonesList.hpp"
#include <utility>

Headphones& HeadphonesListNode::value()
{
    return m_value;
}
HeadphonesListNode::node_ptr HeadphonesListNode::get_next() const
{
    return m_next;
}
HeadphonesListNode::node_ptr HeadphonesListNode::get_prev() const
{
    return m_prev;
}

void HeadphonesListNode::set_next(node_ptr next)
{
    m_next = next;
}
void HeadphonesListNode::set_prev(node_ptr prev)
{
    m_prev = prev;
}
void HeadphonesListNode::disconnect()
{
    set_next(nullptr);
    set_prev(nullptr);
}

HeadphonesList::Iterator::Iterator(
    std::shared_ptr<HeadphonesListNode> ptr
) :
    m_ptr(ptr)
{}
HeadphonesList::Iterator::reference HeadphonesList::Iterator::operator*()
{
    return m_ptr;
}
HeadphonesList::Iterator::pointer HeadphonesList::Iterator::operator->()
{
    return &m_ptr;
}
HeadphonesList::Iterator& HeadphonesList::Iterator::operator++()
{
    m_ptr = m_ptr->get_next();
    return *this;
}
HeadphonesList::Iterator HeadphonesList::Iterator::operator++(int)
{
    HeadphonesList::Iterator result = *this;
    ++(*this);
    return result;
}
HeadphonesList::Iterator& HeadphonesList::Iterator::operator--()
{
    m_ptr = m_ptr->get_prev();
    return *this;
}
HeadphonesList::Iterator HeadphonesList::Iterator::operator--(int)
{
    HeadphonesList::Iterator result = *this;
    --(*this);
    return result;
}
bool operator== (const HeadphonesList::Iterator& a, const HeadphonesList::Iterator& b)
{
    return a.m_ptr == b.m_ptr;
}
bool operator!= (const HeadphonesList::Iterator& a, const HeadphonesList::Iterator& b)
{
    return !(a == b);
}
void swap(HeadphonesList::Iterator& a, HeadphonesList::Iterator& b)
{
    std::swap(a.m_ptr, b.m_ptr);
}

HeadphonesList::HeadphonesList() :
    m_head(nullptr),
    m_tail(nullptr),
    m_count(0)
{}

HeadphonesList::Iterator HeadphonesList::head()
{
    return m_head;
}
HeadphonesList::Iterator HeadphonesList::tail()
{
    return m_tail;
}
std::ptrdiff_t HeadphonesList::count() const
{
    return m_count;
}
bool HeadphonesList::is_empty() const
{
    return count() == 0;
}
bool HeadphonesList::is_not_empty() const
{
    return !is_empty();
}

void HeadphonesList::remove(HeadphonesList::Iterator it)
{
    HeadphonesListNode::node_ptr node = *it;
    if (!node)
    {
        return;
    }

    HeadphonesListNode::node_ptr next = node->get_next();
    HeadphonesListNode::node_ptr prev = node->get_prev();

    if (next)
    {
        next->set_prev(prev);
    }
    else
    {
        m_tail = prev;
    }

    if (prev)
    {
        prev->set_next(next);
    }
    else
    {
        m_head = next;
    }

    node->disconnect();
    m_count--;
}

HeadphonesList::Iterator HeadphonesList::insert_internal(
    HeadphonesListNode::node_ptr node,
    HeadphonesListNode::node_ptr next,
    HeadphonesListNode::node_ptr prev
)
{
    node->set_next(next);
    node->set_prev(prev);

    if (next)
    {
        next->set_prev(node);
    }
    else
    {
        m_tail = node;
    }

    if (prev)
    {
        prev->set_next(node);
    }
    else
    {
        m_head = node;
    }

    m_count++;
    return Iterator(node);
}
