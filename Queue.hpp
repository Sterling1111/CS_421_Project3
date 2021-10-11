//
// Created by Sterling Jeppson on 10/5/2021.
//

#ifndef CS_421_PROJECT3_QUEUE_HPP
#define CS_421_PROJECT3_QUEUE_HPP
#include <algorithm>
#include <initializer_list>

template <class T>
struct Node {
    T data;
    Node<T>* link;
    Node() : data{T()}, link{nullptr} {};
    Node(const T& data, Node<T>* link) : data{data}, link{link} {};
};

class FullQueueError {};
class EmptyQueueError {};

template <class T>
class Queue {
public:
    //default constructor
    Queue();
    //copy constructor
    Queue(const Queue<T>& otherQueue);
    //initializer constructor
    Queue(std::initializer_list<T> l);
    //move constructor
    Queue(Queue<T>&& otherQueue) noexcept ;
    //overloaded assignment operator
    Queue<T>& operator=(const Queue<T>& otherQueue);
    //move assignment operator
    Queue<T>& operator=(Queue<T>&& otherQueue) noexcept;
    inline bool isEmpty() const;
    inline bool isFull() const;
    inline int size() const;
    void destroy();
    void enqueue(const T& insertItem);
    void dequeue();
    T& front() const;
    T& back() const;
    virtual ~Queue();
private:
    void copyQueue(const Queue<T>& otherQueue);
    Node<T>* Front;
    Node<T>* Back;
    int Size;
};

//default constructor
template <class T>
Queue<T>::Queue() {
    Front = nullptr;
    Back = nullptr;
    Size = 0;
}

//copy constructor
template <class T>
Queue<T>::Queue(const Queue<T>& otherQueue) {
    Front = Back = nullptr;
    Size = 0;
    copyQueue(otherQueue);
}

//initializer list constructor
template <class T>
Queue<T>::Queue(std::initializer_list<T> l) {
    Front = Back = nullptr;
    Size = 0;
    for(const T& x : l)
        enqueue(x);
}

//move constructor
template <class T>
Queue<T>::Queue(Queue<T>&& otherQueue) noexcept {
    Front = otherQueue.Front;
    Back = otherQueue.Back;
    Size = otherQueue.Size;
    otherQueue.Front = nullptr;
    otherQueue.Back = nullptr;
}

//overloaded assignment operator
template <class T>
Queue<T>& Queue<T>::operator=(const Queue<T>& otherQueue) {
    if(this != &otherQueue)
        copyQueue(otherQueue);
    return *this;
}

//move assignment operator
template <class T>
Queue<T>& Queue<T>::operator=(Queue<T>&& otherQueue) noexcept {
    if(this != &otherQueue) {
        Size = otherQueue.Size;
        std::swap(Front, otherQueue.Front);
        std::swap(Back, otherQueue.Back);
    }
    return *this;
}

template <class T>
inline bool Queue<T>::isEmpty() const {
    return (!Front);
}

template <class T>
inline bool Queue<T>::isFull() const {
    try {
        Node<T>* tempNode{new Node<T>};
        delete tempNode;
        return false;
    }
    catch(std::bad_alloc& exception) {
        return true;
    }
}

template <class T>
inline int Queue<T>::size() const {
    return Size;
}

template <class T>
void Queue<T>::destroy() {
    Node<T>* temp{nullptr};
    while(Front) {
        temp = Front;
        Front = Front->link;
        delete temp;
        temp = nullptr;
    }
    Back = nullptr;
    Size = 0;
}

template <class T>
void Queue<T>::enqueue(const T& insertItem) {
    Node<T>* newNode{new Node<T>(insertItem, nullptr)};
    if(!Front)
        Front = Back = newNode;
    else {
        Back->link = newNode;
        Back = newNode;
    }
    Size++;
}

template <class T>
void Queue<T>::dequeue() {
    if(Front) {
        Node<T>* temp{Front};
        Front = Front->link;
        delete temp;
        Size--;
    }
}

template <class T>
T& Queue<T>::front() const {
    if(!Front)
        throw EmptyQueueError();
    return Front->data;
}

template <class T>
T& Queue<T>::back() const {
    if(!Back)
        throw FullQueueError();
    return Back->data;
}

template <class T>
void Queue<T>::copyQueue(const Queue<T>& otherQueue) {
    if(Front)
        destroy();
    if(otherQueue.Front) {
        Node<T>* otherCurrent{otherQueue.Front};
        Front = new Node<T>(otherCurrent->data, nullptr);
        otherCurrent = otherCurrent->link;
        Node<T>* current{Front};
        while(otherCurrent) {
            current = (current->link = new Node<T>(otherCurrent->data, nullptr));
            otherCurrent = otherCurrent->link;
        }
        Back = current;
    }
    Size = otherQueue.Size;
}

template <class T>
Queue<T>::~Queue() {
    destroy();
}

#endif //CS_421_PROJECT3_QUEUE_HPP
