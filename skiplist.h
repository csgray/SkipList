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
#include <random>	// for std::minstd_rand, std::uniform_real_distribution

// Global Variables
// Edit these to change the performance of the skip list. Pugh recommends a proportion of 0.25 unless 
// "the variability of running times is a primary concern" in which case use 0.5.
const double proportion = 0.25;	// Each level has this proportion of nodes relative to the level below it
const int MaxLevel = 16;	// MaxLevel for 2^32 nodes with a _proportion of 1/4 is log base 1/(_proportion) 2^16 is 16

// struct SkipListNode
// Skip List int node. SkipListNode objects should be created with std::make_shared and owned by a std::shared_ptr.
// An empty list consists of the head and tail (NIL in Pugh's paper) nodes with nothing between them.
//
// Invariants:
//		_key == key
//		_forwardNodes is an array of nullptr or ptrs to other nodes
struct SkipListNode {
	int _key;
	std::shared_ptr<SkipListNode> _forwardNodes[MaxLevel];
	
	// Parameterized Constructor
	// Creates an unlinked SkipListNode
	//
	// Preconditions:
	//		key is not empty
	// Postconditions:
	//		_key == key
	// Exceptions: 
	//		
	// Strong Guarantee, Exception Neutral
	explicit SkipListNode(int key)
		: _key(key)
	{
		for (int i = 0; i < MaxLevel; i++)
			_forwardNodes[i] = nullptr;
	}

	// Destructor
	// As SkipListNodes are created using make_shared,
	// they are deleted when their reference count drops to zero.
	~SkipListNode() = default;	
};

// randomDouble()
// Returns a double between 0 and 1
// 
// Preconditions: None
// No-Throw Guarantee
double randomDouble()
{
	std::random_device seed;
	std::minstd_rand generator(seed());
	std::uniform_real_distribution<double> random(0.0, 1.0);
	double result = random(generator);
	return result;
}

// randomLevel()
// Returns an integer between 1 and MaxLevel to determine a new node's level
//
// Preconditions: randomDouble() function
// No-Throw Guarantee
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
//		Nodes are linked
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
	//
	// Preconditions: None
	// Postconditions: SkipList with invariants
	// Exceptions: Throws if make_shared throws
	//
	// Strong Guarantee, Exception Neutral
	SkipList()
	{
		_tail = std::make_shared<SkipListNode>(std::numeric_limits<int>::max());
		_head = std::make_shared<SkipListNode>(std::numeric_limits<int>::min());
		for (int i = MaxLevel-1; i >= 0; i--)
			_head->_forwardNodes[i] = _tail;
	}

	// Destructor
	// Moves from _head to _tail setting each element of the _forewardNodes arrays to nullptr.
	// This decrements the shared_ptr reference counts, eventually deleting the items,
	// while avoiding the stack overflow errors created with recursive destructor calls.
	//
	// No-Throw Guarantee
	~SkipList()
	{
		std::shared_ptr<SkipListNode> currentNode = _head;
		std::shared_ptr<SkipListNode> tempNode;
		while (currentNode->_forwardNodes[0])
		{
			tempNode = currentNode->_forwardNodes[0];
			for (int i = MaxLevel - 1; i >= 0; i--)
				currentNode->_forwardNodes[i] = nullptr;
			currentNode = tempNode;
		}
	}

// ***** SkipList: Public Member Functions *****
public:
	// search
	// Looks for an item and returns a const shared_ptr if it is found, nullptr otherwise
	//
	// Preconditions: A SkipList
	// Exceptions: None
	// Strong Guarantee, Exception Neutral
	const std::shared_ptr<SkipListNode> search(int searchKey)
	{
		std::shared_ptr<SkipListNode> currentNode = _head;
		for (int i = MaxLevel - 1; i >= 0; i--) {
			while (currentNode->_forwardNodes[i]->_key < searchKey)
				currentNode = currentNode->_forwardNodes[i];
		}
		currentNode = currentNode->_forwardNodes[0];
		if (currentNode->_key == searchKey)
			return currentNode;
		return nullptr;
	}

	// insert
	// Inserts a new node into the list at the proper, sorted position.
	// Links the previous nodes to it and links it to following nodes up to randomLevel.
	//
	// NOTE: Duplicate entries will be inserted _before_ the previous entries.
	// This is because skip lists are designed to allow update operations which would preclude duplicate entries,
	// and this code will be updated for that as soon as I figure out how to test it properly.
	// 
	// Preconditions:
	//		A SkipList
	//		An integer key
	// Postconditions:
	//		A new SkipListNode with _key == key inserted immediately after the node with the next-lowest key
	// Exceptions:
	//		Throws if make_shared throws
	// Strong Guarantee, Exception Neutral
	void insert(int insertKey)
	{
		// Determine which nodes at each level need to be updated
		std::shared_ptr<SkipListNode> updateNodes[MaxLevel];
		std::shared_ptr<SkipListNode> currentNode = _head;
		for (int i = MaxLevel - 1; i >= 0; i--) {
			while (currentNode->_forwardNodes[i]->_key < insertKey)
				currentNode = currentNode->_forwardNodes[i];
			updateNodes[i] = currentNode;
		}

		// Create a new node and link it
		int level = randomLevel();
		auto newNode = std::make_shared<SkipListNode>(insertKey);
		for (int i = 0; i < level; i++)
		{
			newNode->_forwardNodes[i] = updateNodes[i]->_forwardNodes[i];
			updateNodes[i]->_forwardNodes[i] = newNode;
		}
	}

	// remove
	// Removes a node from the list by updating the pointers that referenced it
	// to point at the nodes that it used to point to. This decrements the shared_ptr's
	// reference count until the node is removed.
	//
	// Preconditions: A SkipList
	// Postconditions: If a node with _key == key existed, that node is removed.
	// Strong Guarantee, Exception-Neutral
	void remove(int removeKey)
	{
		
		// Determine which nodes at each level need to be updated
		std::shared_ptr<SkipListNode> updateNodes[MaxLevel];
		std::shared_ptr<SkipListNode> currentNode = _head;
		for (int i = MaxLevel - 1; i >= 0; i--) {
			while (currentNode->_forwardNodes[i]->_key < removeKey)
				currentNode = currentNode->_forwardNodes[i];
			updateNodes[i] = currentNode;
		}
		currentNode = currentNode->_forwardNodes[0];

		if (currentNode->_key == removeKey)
		{
			for (int i = 0; i < MaxLevel; i++)
			{
				if (updateNodes[i]->_forwardNodes[i] != currentNode)
					break;
				updateNodes[i]->_forwardNodes[i] = currentNode->_forwardNodes[i];
			}
				
		}
	}
};

#endif // #ifndef FILE_SKIPLIST_H_INCLUDED