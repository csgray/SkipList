// skiplist.h 
// Corey S. Gray
// 09 Jan 2018
//
// For CS 311 Fall 2017
// Header for class SkipList
// Based on "Skip Lists: A Probabilistic Alternative to Balanced Trees" by William Pugh
// PDF available at ftp://ftp.cs.umd.edu/pub/skipLists/skiplists.pdf

#ifndef FILE_SKIPLIST_H_INCLUDED
#define FILE_SKIPLIST_H_INCLUDED

#include <memory>	// for std::shared_ptr, std::make_shared
#include <limits>	// for std::numeric_limits
#include <random>	// for std::minstd_rand
#include <iostream>		// for std::cout

// Global Variables
const double proportion = 0.25;	// Each level has this proportion of nodes relative to lower level
const int MaxLevel = 16;	// MaxLevel for 2^32 nodes with a _proportion of 1/4 is log base 1/(_proportion) 2^16 is 8

// struct SkipListNode
// Skip List int node. SkipListNode objects should be created with std::make_shared and owned by a std::shared_ptr.
// An empty list consists of the head and NIL nodes with nothing between them.
//
// Invariants:
//		_data == data
//		_forwardNodes is an array of nullptr or ptrs to other nodes
struct SkipListNode {
	int _value;
	std::shared_ptr<SkipListNode> _forwardNodes[MaxLevel];
	
	// Parameterized Constructor
	// Creates an unlinked SkipListNode (which isn't that useful),
	// unless neighboring nodes are passed as additional parameters.
	//
	// Preconditions:
	//		data is not empty
	// Postconditions:
	//		_data == data
	// Exceptions:
	//		
	// Strong Guarantee, Exception Neutral
	explicit SkipListNode(int value)
		: _value(value)
	{}

	// Destructor
	~SkipListNode() = default;
};

// double random()
// Generates a double between 0 and 1
double randomDouble()
{
	std::random_device seed;
	std::minstd_rand generator(seed());
	std::uniform_real_distribution<double> random(0.00, 1.00);
	return random(generator);
}

// int randomLevel()
// Returns an integer between 1 and MaxLevel to determine a new node's level
int randomLevel()
{
	int level = 1;
	while ((randomDouble() < proportion) && (level < MaxLevel))
		level++;
	return level;
}

// class SkipList
// Uses a SkipList to hold a sorted dataset.
// Invariants:
//		_head is a shared_ptr to a SkipListNode of "negative infinity"
//		_tail is a shared_ptr to a SkipListNode of "positive infinity"
class SkipList {
// ***** SkipList: Data Members *****
public:
	std::shared_ptr<SkipListNode> _head;
	std::shared_ptr<SkipListNode> _tail;

// ***** SkipList: Constructors and Destructors *****
public:
	// Default Constructor
	// Creates an empty list bracketed by the _head and _tail nodes
	// _head and _tail are as close to negative infinity and infinity as possible
	SkipList()
	{
		_tail = std::make_shared<SkipListNode>(std::numeric_limits<int>::max());
		_head = std::make_shared<SkipListNode>(std::numeric_limits<int>::min());
		for (int i = MaxLevel-1; i >= 0; i--)
			_head->_forwardNodes[i] = _tail;
	}

	// Destructor
	~SkipList() = default;
};

#endif // #ifndef FILE_SKIPLIST_H_INCLUDED