// skiplist_test_suites.cpp
// Corey S. Gray
// 09 Jan 2018
//
// For CS 311 Fall 2017
// Tests for class SkipList
// Uses the "Catch" unit-testing framework
// Requires skiplist_test_main.cpp, catch.hpp, skiplist.h

// Includes for code to be tested
#include "skiplist.h"				// For class SkipList
#include "skiplist.h"				// Double inclusion test

// Includes and settings for Catch framework
#include "catch.hpp"				// For the "Catch" unit-testing framework
#define CATCH_CONFIG_FAST_COMPILE	// Disable some features for faster compile

// Additional includes for this test program
#include <limits>					// for std::numeric_limits
#include <vector>					// for std::vector
#include <algorithm>				// for std::sort, std::generate
#include <random>					// for std::random_device, std::minstd_rand, std::uniform_int_distribution
#include <iostream>					// for std::cout, std::endl

// *********************************************************************
// Utility Functions
// *********************************************************************

// Random Number Generator
// Returns a random integer between –2,147,483,648 and 2,147,483,647
// 
// Preconditions: None
// No-Throw Guarantee
int randomNumber()
{
	std::random_device seed;
	std::minstd_rand generator(seed());
	std::uniform_int_distribution<int> values(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
	return values(generator);
}

// *********************************************************************
// Test Cases
// *********************************************************************

TEST_CASE("SkipList Invariants", "[invariants]")
// Tests to make sure that new SkipLists have both _head and _tail of the appropriate values
// and that those two nodes are linked
{
	SECTION("New empty list.")
	{
		SkipList testList = SkipList();
		{
			INFO("_head is as close to negative infinity as possible.");
			REQUIRE(testList._head->_key == std::numeric_limits<int>::min());
		}
		{
			INFO("_tail is as close to positive infinity as possible.");
			REQUIRE(testList._tail->_key == std::numeric_limits<int>::max());
		}
		{
			INFO("_head is linked to tail at every level.");
			for (int i = 0; i < MaxLevel; i++)
				REQUIRE(testList._head->_forwardNodes[i] == testList._tail);
		}

	}
}

TEST_CASE("SkipList Insertions", "[member functions]")
// Tests to make sure that insert functions insert only one node with the correct _key,
// that nodes are in the correct order, and that old and new nodes are linked to each correctly.
{
	SECTION("Insert one item.")
	{
		SkipList testList = SkipList();
		{
			INFO("List does NOT have item pre-inserted. CHEATER!");
			REQUIRE(!(testList.search(0)));
		}
		testList.insert(0);
		std::shared_ptr<SkipListNode> result = testList.search(0);
		{
			INFO("List has item in it.");
			REQUIRE(result);
		}
		{
			INFO("_head is linked to item.");
			REQUIRE(testList._head->_forwardNodes[0] == result);
		}
		{
			INFO("Item is linked to _tail.");
			REQUIRE(result->_forwardNodes[0] == testList._tail);
		}
		{
			INFO("List does NOT have item not inserted.");
			REQUIRE(!(testList.search(1)));
		}
	}

	SECTION("Insert 10 items.")
	// Creates a vector of 10 ints, inserts them, sorts the original, and then
	// compares the list to the vector to make sure items were inserted in the right order.
	{
		SkipList testList = SkipList();
		std::vector<int> testInts = {0, -37, 42, 178, 91, -9999, 777, 9999, 3, 400};
		for (auto i : testInts)
			testList.insert(i);
		std::sort(testInts.begin(), testInts.end());
		std::vector<int> resultInts;
		std::shared_ptr<SkipListNode> node = testList._head;
		node = node->_forwardNodes[0];	// Don't include the _head node in the results
		while (node->_key != testList._tail->_key)
		{
			resultInts.push_back(node->_key);
			node = node->_forwardNodes[0];
		}
		{
			INFO("All items were inserted.");
			REQUIRE(resultInts.size() == testInts.size());
		}
		{
			INFO("Items are sorted.");
			REQUIRE(resultInts == testInts);
		}
	}

	SECTION("Insert 1,000,000 items.")
	// Stress-tests the insertion algorithm. Improperly written lists will stack overflow.
	// 100,000 ints is probably fine for testing, but may not be large enough for good level tests.
	// 5,000,000 also works but is so slow that it might mask other problems.
	// 10,000,000 ints throws bad alloc in Visual Studio 2017 regardless,
	// which may be cause to reduce MaxLevel and further improve performance and memory usage for smaller datasets.
	{
		SkipList testList = SkipList();
		int testNumber = 1000000;
		std::vector<int> testInts(testNumber);
		std::vector<int> resultInts;
		std::generate(testInts.begin(), testInts.end(), randomNumber);
		for (auto i : testInts)
			testList.insert(i);
		std::sort(testInts.begin(), testInts.end());
		std::shared_ptr<SkipListNode> node = testList._head;
		node = node->_forwardNodes[0];
		while (node->_key != testList._tail->_key)
		{
			resultInts.push_back(node->_key);
			node = node->_forwardNodes[0];
		}
		{
			INFO("All items were inserted.");
			REQUIRE(resultInts.size() == testInts.size());
		}
		{
			INFO("Items are sorted.");
			REQUIRE(resultInts == testInts);
		}

		// A node's level is determined at random, so tests for each level must use ranges.
		// Each test checks that the number of level[i] nodes is between 96% and 104% of the expected number.
		// Experimenting with different tolerances and levels revealed that statistical tests break down between
		// levels 5 and 7 due to lucky rolls resulting in only a few more or fewer nodes than expected.
		int testLevels = 4;
		auto countNode = testList._head;
		std::vector<int> levels(testLevels);
		for (int i = 1; i < testLevels; i++)
		{
			while ((countNode->_forwardNodes[i]) &&
				   (countNode->_forwardNodes[i] != testList._tail))
			{
				levels[i]++;
				countNode = countNode->_forwardNodes[i];
			}
			countNode = testList._head;
			
			{
				int low = (testNumber * std::pow(proportion, i)) * 0.96;	// Set low tolerance here
				int high = (testNumber * std::pow(proportion, i)) * 1.04;	// Set high tolerance here
				INFO("Insert creates an appropriate number of level " << i << " nodes. \n" << \
				     "NOTE: Skip list levels are randomly generated and may fall slightly outside \n" << \
				     "bounds by chance. Rerun the tests if you are off only by a few nodes.");
				REQUIRE(low < levels[i]);
				REQUIRE(levels[i] < high);
			}
		}
	}
}

TEST_CASE("SkipList Removals", "[member functions]")
// Tests to make sure that the remove function actually removes items, only removes what it is supposed to remove,
// and does not remove _head or _tail as that would break the list.
{
	SECTION("Insert ten items and remove one item.")
	{
		SkipList testList = SkipList();
		std::vector<int> testInts = { 0, -37, 42, 178, 91, -9999, 777, 9999, 3, 400 };
		for (auto i : testInts)
			testList.insert(i);
		std::sort(testInts.begin(), testInts.end());

		testList.remove(91);
		auto it = std::find(testInts.begin(), testInts.end(), 91);
		if (it != testInts.end())
			testInts.erase(it);

		std::vector<int> resultInts;
		std::shared_ptr<SkipListNode> node = testList._head;
		node = node->_forwardNodes[0];
		while (node->_key != testList._tail->_key)
		{
			resultInts.push_back(node->_key);
			node = node->_forwardNodes[0];
		}
		{
			INFO("Item 91 was successfully removed.");
			auto result = std::find(resultInts.begin(), resultInts.end(), 91);
			REQUIRE(result == resultInts.end());
		}
		{
			INFO("The correct number of items was removed.");
			REQUIRE(resultInts.size() == testInts.size());
		}
		{
			INFO("Items are still sorted after removal.");
			REQUIRE(resultInts == testInts);
		}
	}

	SECTION("Insert ten items and remove three items.")
	{
		SkipList testList = SkipList();
		std::vector<int> testInts = { 0, -37, 42, 178, 91, -9999, 777, 9999, 3, 400 };
		for (auto i : testInts)
			testList.insert(i);
		std::sort(testInts.begin(), testInts.end());

		testList.remove(-37);
		auto it = std::find(testInts.begin(), testInts.end(), -37);
		if (it != testInts.end())
			testInts.erase(it);

		testList.remove(400);
		auto it2 = std::find(testInts.begin(), testInts.end(), 400);
		if (it2 != testInts.end())
			testInts.erase(it2);

		testList.remove(777);
		auto it3 = std::find(testInts.begin(), testInts.end(), 777);
		if (it3 != testInts.end())
			testInts.erase(it3);

		// This should have no effect as 300 was never inserted.
		testList.remove(300);

		std::vector<int> resultInts;
		std::shared_ptr<SkipListNode> node = testList._head;
		node = node->_forwardNodes[0];
		while (node->_key != testList._tail->_key)
		{
			resultInts.push_back(node->_key);
			node = node->_forwardNodes[0];
		}
		{
			INFO("Item -37 was successfully removed.");
			auto result = std::find(resultInts.begin(), resultInts.end(), -37);
			REQUIRE(result == resultInts.end());
		}
		{
			INFO("Item 400 was successfully removed.");
			auto result = std::find(resultInts.begin(), resultInts.end(), 400);
			REQUIRE(result == resultInts.end());
		}
		{
			INFO("Item 777 was successfully removed.");
			auto result = std::find(resultInts.begin(), resultInts.end(), 777);
			REQUIRE(result == resultInts.end());
		}
		{
			INFO("The correct number of items was removed.");
			REQUIRE(resultInts.size() == testInts.size());
		}
		{
			INFO("Items are still sorted after removal.");
			REQUIRE(resultInts == testInts);
		}
		{
			INFO("_head cannot be removed.");
			testList.remove(testList._head->_key);
			REQUIRE(testList._head->_key == std::numeric_limits<int>::min());
		}
		{
			INFO("_tail cannot be removed.");
			testList.remove(testList._tail->_key);
			REQUIRE(testList._tail->_key == std::numeric_limits<int>::max());
		}
	}
}
