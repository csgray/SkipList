// skiplist_test_suites.cpp
// Corey S. Gray
// 09 Jan 2018
//
// For CS 311 Fall 2017
// Tests for class SkipList
// Uses the "Catch" unit-testing framework
// Requires skiplist_test_main.cpp, catch.hpp, skiplist.h

// Includes for code to be tested
#include "skiplist.h"         // For class SkipList
#include "skiplist.h"         // Double inclusion test

#define CATCH_CONFIG_FAST_COMPILE
// Disable some features for faster compile
#include "catch.hpp"       // For the "Catch" unit-testing framework

// Additional includes for this test program
#include <limits>		// for std::numeric_limits
#include <vector>		// for std::vector
#include <algorithm>	// for std::sort
#include <random>

// Random Number Generator
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


TEST_CASE("SkipList Invariants",
	"[invariants]")
{
	SECTION("New empty list.")
	{
		SkipList testList = SkipList();
		{
			INFO("Head is as close to negative infinity as possible.");
			REQUIRE(testList._head->_value == std::numeric_limits<int>::min());
		}
		{
			INFO("Head is as close to positive infinity as possible.");
			REQUIRE(testList._tail->_value == std::numeric_limits<int>::max());
		}
		{
			INFO("Head is linked to tail at every level.");
			for (int i = 0; i < MaxLevel; i++)
				REQUIRE(testList._head->_forwardNodes[i] == testList._tail);
		}

	}
}

TEST_CASE("SkipList Insertions",
	"[member functions]")
{
	SECTION("Insert one item.")
	{
		SkipList testList = SkipList();
		testList.insert(0);
		std::shared_ptr<SkipListNode> result = testList.search(0);
		{
			INFO("List has item in it.");
			REQUIRE(result);
		}
		{
			INFO("Head is linked to item.");
			REQUIRE(testList._head->_forwardNodes[0] == result);
		}
		{
			INFO("Item is linked to tail.");
			REQUIRE(result->_forwardNodes[0] == testList._tail);
		}
		{
			INFO("List does NOT have item not inserted.");
			REQUIRE(!(testList.search(1)));
		}
	}

	SECTION("Insert 10 items.")
	{
		SkipList testList = SkipList();
		// Populate data
		std::vector<int> testInts = {0, -37, 42, 178, 91, -9999, 777, 9999, 3, 400};
		for (auto i : testInts)
			testList.insert(i);
		std::sort(testInts.begin(), testInts.end());

		std::vector<int> resultInts;
		std::shared_ptr<SkipListNode> node = testList._head;
		node = node->_forwardNodes[0];
		while (node->_value != testList._tail->_value)
		{
			resultInts.push_back(node->_value);
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

	SECTION("Insert 100,000 items.")
	{
		SkipList testList = SkipList();
		std::vector<int> testInts(100000);	// 1,000,000 works but is slow
		std::vector<int> resultInts;
		
		// Generate 1000 random vectors across the entire range of possible ints
		std::generate(testInts.begin(), testInts.end(), randomNumber);

		// Populate skip list
		for (auto i : testInts)
			testList.insert(i);
		
		// Check for sorting
		std::sort(testInts.begin(), testInts.end());
		
		// Retrieve results
		std::shared_ptr<SkipListNode> node = testList._head;
		node = node->_forwardNodes[0];
		while (node->_value != testList._tail->_value)
		{
			resultInts.push_back(node->_value);
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
}

TEST_CASE("SkipList Removals",
	"[member functions]")
{
	SECTION("Insert 10 items and remove 1 item.")
	{
		SkipList testList = SkipList();
		// Populate data
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
		while (node->_value != testList._tail->_value)
		{
			resultInts.push_back(node->_value);
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

	SECTION("Insert 10 items and remove 1 item.")
	{
		SkipList testList = SkipList();
		// Populate data
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

		std::vector<int> resultInts;
		std::shared_ptr<SkipListNode> node = testList._head;
		node = node->_forwardNodes[0];
		while (node->_value != testList._tail->_value)
		{
			resultInts.push_back(node->_value);
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
	}
}
