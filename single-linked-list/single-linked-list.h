#pragma once
#include <cstddef>
#include <string>
#include <utility>
#include <algorithm>
#include <stdexcept>

template<typename Type>
class SingleLinkedList
{
    struct Node
    {
        Node() = default;
        
        Node(const Type& val, Node* next) : value(val), next_node(next) {}
        
        Type value;
        Node* next_node = nullptr;
    };
    
    template<typename ValueType>
    class BasicIterator
    {
        friend class SingleLinkedList;
        
        explicit BasicIterator(Node* node) : node_(node) {}
    
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Type;
        using difference_type = std::ptrdiff_t;
        using pointer = ValueType*;
        using reference = ValueType&;
        
        BasicIterator() = default;
        
        BasicIterator(const BasicIterator<Type>& other) noexcept: node_(other.node_) {}
        
        BasicIterator& operator=(const BasicIterator& rhs) = default;
        
        [[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept
        {
            return (this->node_) == rhs.node_;
        }
        
        [[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept
        {
            //Изменил != на == с отрицанием
            return !((this->node_) == rhs.node_);
        }
        
        [[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept
        {
            return (this->node_) == rhs.node_;
        }
        
        [[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept
        {
            //Изменил != на == с отрицанием
            return !((this->node_) == rhs.node_);
        }
        
        BasicIterator& operator++() noexcept
        {
            node_ = node_->next_node;
            return *this;
        }
        
        BasicIterator operator++(int) noexcept
        {
            BasicIterator basic_iterator(node_);
            node_ = node_->next_node;
            return basic_iterator;
        }
        
        [[nodiscard]] reference operator*() const noexcept
        {
            return node_->value;
        }
        
        [[nodiscard]] pointer operator->() const noexcept
        {
            return &(node_->value);
        }
    
    private:
        Node* node_ = nullptr;
    };

public:
    
    //Using
    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;
    using Iterator = BasicIterator<Type>;
    using ConstIterator = BasicIterator<const Type>;
    
    
    //Iterators
    [[nodiscard]] Iterator begin() noexcept
    {
        Iterator basic_iterator(head_.next_node);
        return basic_iterator;
    }
    
    [[nodiscard]] Iterator end() noexcept
    {
        //Итератор на nullptr
        return Iterator{nullptr};
    }
    
    [[nodiscard]] ConstIterator begin() const noexcept
    {
        return cbegin();
    }
    
    [[nodiscard]] ConstIterator end() const noexcept
    {
        return cend();
    }
    
    [[nodiscard]] ConstIterator cbegin() const noexcept
    {
        ConstIterator basic_iterator(head_.next_node);
        return basic_iterator;
    }
    
    [[nodiscard]] ConstIterator cend() const noexcept
    {
        //Итератор на nullptr
        return Iterator{nullptr};
    }
    
    
    // Возвращает итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] Iterator before_begin() noexcept
    {
        Iterator basic_iterator(&head_);
        return basic_iterator;
    }
    
    // Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] ConstIterator cbefore_begin() const noexcept
    {
        ConstIterator basic_iterator(const_cast<Node*>(&head_));
        return basic_iterator;
    }
    
    // Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] ConstIterator before_begin() const noexcept
    {
        return cbefore_begin();
    }
    
    
    //Constructors and destructor
    SingleLinkedList() : size_(0) {};
    
    SingleLinkedList(std::initializer_list<Type> values) : SingleLinkedList()
    {
        for (auto it = rbegin(values); it != rend(values); ++it) {
            PushFront(*it);
        }
    }
    
    SingleLinkedList(const SingleLinkedList& other)
    {
        if (other.IsEmpty()) {
            size_ = 0;
            return;
        }
        //Изменил конструктор, теперь он заполняет контейнер в один проход.
        ConstIterator it_other = other.begin();
        SingleLinkedList single_linked_list;
        Node* it_tail = new Node();
        single_linked_list.head_.next_node = it_tail;
        
        while (it_other != other.end()) {
            it_tail->value = *it_other;
            it_tail->next_node = it_other->next_node != nullptr ? new Node() : nullptr;
            it_tail = it_tail->next_node;
            ++it_other;
        }
        SingleLinkedList::swap(single_linked_list);
    }
    
    ~SingleLinkedList()
    {
        Clear();
    }
    
    
    //Operators
    SingleLinkedList& operator=(const SingleLinkedList& rhs)
    {
        SingleLinkedList single_linked_list(rhs);
        swap(single_linked_list);
        return *this;
    }
    
    bool operator==(const SingleLinkedList& rhs) const
    {
        if (size_ != rhs.size_) { return false; }
        return std::equal(begin(), end(), rhs.begin(), rhs.end());
    }
    
    bool operator!=(const SingleLinkedList& rhs) const
    {
        return !(rhs == *this);
    }
    
    bool operator<(const SingleLinkedList& rhs) const
    {
        return std::lexicographical_compare(begin(), end(), rhs.begin(), rhs.end());
    }
    
    bool operator>(const SingleLinkedList& rhs) const
    {
        return rhs < *this;
    }
    
    bool operator<=(const SingleLinkedList& rhs) const
    {
        return !(rhs < *this);
    }
    
    bool operator>=(const SingleLinkedList& rhs) const
    {
        return !(*this < rhs);
    }
    
    
    //Methods
    [[nodiscard]] size_t GetSize() const noexcept
    {
        return size_;
    }
    
    [[nodiscard]] bool IsEmpty() const noexcept
    {
        return size_ == 0;
    }
    
    void PushFront(const Type& value)
    {
        head_.next_node = new Node(value, head_.next_node);
        ++size_;
    }
    
    void PopFront() noexcept
    {
        Node* node = head_.next_node;
        head_.next_node = node->next_node;
        --size_;
        delete node;
    }
    
    /*
     * Вставляет элемент value после элемента, на который указывает pos.
     * Возвращает итератор на вставленный элемент
     * Если при создании элемента будет выброшено исключение, список останется в прежнем состоянии
     */
    Iterator InsertAfter(ConstIterator pos, const Type& value)
    {
        if(pos == end()) {
            throw std::invalid_argument("Iterator does not have to be end()");
        }
        Node* node = pos.node_;
        node->next_node = new Node(value, node->next_node);
        ++size_;
        return Iterator(node->next_node);
    }
    
    /*
     * Удаляет элемент, следующий за pos.
     * Возвращает итератор на элемент, следующий за удалённым
     */
    Iterator EraseAfter(ConstIterator pos) noexcept
    {
        if(pos == end()) {
            throw std::invalid_argument("Iterator does not have to be end()");
        }
        Node* p_node = pos.node_->next_node;
        pos.node_->next_node = p_node->next_node;
        --size_;
        delete p_node;
        return Iterator(pos.node_->next_node);
    }
    
    void Clear() noexcept
    {
        while (head_.next_node != nullptr) {
            //Использовал библиотечную функцию exchange
            Node* node = std::exchange(head_.next_node, head_.next_node->next_node);
            delete node;
        }
    }
    
    void swap(SingleLinkedList& other) noexcept
    {
        std::swap(size_, other.size_);
        std::swap(head_.next_node, other.head_.next_node);
    }


private:
    Node head_;
    size_t size_;
};

template<typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept
{
lhs.swap(rhs);
}