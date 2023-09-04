
#pragma once

template <typename T>
class SinglyLinkedList
{
private:
	struct Node
	{
		Node *next;
		T value;
	};

public:
	void push(T value)
	{
	}
	void remove(T value)
	{
	}
};

template <typename T>
class Queue
{
private:
	struct Node
	{
		Node *next;
		T value;
	};

	Node* head;
	Node* tail;

public:
	void enquque(T value);
	T dequeue();
};