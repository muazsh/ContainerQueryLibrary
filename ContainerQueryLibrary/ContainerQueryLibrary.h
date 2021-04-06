#ifndef CONTAINER_QUERY_LIBRARY
#define CONTAINER_QUERY_LIBRARY
#pragma once

#include<vector>
#include<list>
#include<map>
#include<algorithm>

#if (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
#include <experimental/generator>
using namespace std::experimental;
#endif

namespace cql
{
	template <typename Type>
	struct is_vector : public std::false_type {};
	template <typename... A>
	struct is_vector<std::vector<A...> > : public std::true_type {};

	template <typename Type>
	struct is_list : public std::false_type {};
	template <typename... A>
	struct is_list<std::list<A...> > : public std::true_type {};

	/* Where Statement:
	*  It allows to filter any container with push_back modifier based on a predicate. 
	* 
	*  Returns: A container of elements that satisfy the predicate.
	* 
	*  Usage:
	*  list<int> ls{1,2,3,4,5};
	*  auto predicate = [](const int& v){ return v%2 == 0; };
	*  auto res = Where(ls, predicate);
	* 
	*  Minimum c++ standard: c++11.
	*/
	template<template<typename...> typename TContainer, typename TElement, typename TFunc>
	TContainer<TElement> Where(const TContainer<TElement>& container, const TFunc& predicate)
	{
		TContainer<TElement> result;
		for (auto element : container)
		{
			if (predicate(element))
				result.push_back(element);
		}
		return result;
	}

	/* Update Statement:
	*  It allows to update a container based on an updating function.
	*
	*  Usage:
	*  list<int> ls{1,2,3,4,5};
	*  auto setFunc = [](int& v){ v = 10; };
	*  Update(ls, setFunc);
	*/
	template<typename TContainer, typename TSetFunc>
	void Update(TContainer& container, const TSetFunc& setFunc)
	{
		for (auto itr = container.begin(); itr != container.end(); ++itr)
			setFunc(*itr);
	}

	/* Update Statement:
	*  It allows to update a container based on an updating function and a predicate.
	*
	*  Usage:
	*  list<int> ls{1,2,3,4,5};
	*  auto set = [](int& v){ v = 10; };
	*  auto predicate = [](const int& v){ return v%2 == 0; };
	*  Update(ls, predicate, set);
	*/
	template<typename TContainer, typename TFunc, typename TSetFunc>
	void Update(TContainer& container, const TFunc& predicate, const TSetFunc& setFunc)
	{
		for (auto itr = container.begin(); itr != container.end(); ++itr)
			if (predicate(*itr))
				setFunc(*itr);
	}

	/* Distinct:
	* 
	*  Returns: a distinct container of the original one.
	*  
	*  Usage: 
	*  list<int> ls{ 11,11,2,2,3,5,6 };
	*  auto res = Distinct(ls);
	*/
	template<typename TContainer>
	TContainer Distinct(const TContainer& container)
	{
		TContainer result;
		for (auto element : container)
		{
			if (std::find(result.begin(), result.end(), element) == result.end())
				result.push_back(element);
		}
		return result;
	}

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
	/* OrderBy:
	*  It Allows to sort std::vector and std::list containers of any type based on an ordering function.
	* 
	*  Usage:
	*  struct MyStruct{ int x,y; };
	*  vector<MyStruct> ls{ {1,4} ,{3,4} ,{1,4}, {2,7}, {1,1}};
	*  auto func = [](const MyStruct& l, const MyStruct& r) { return l.y < r.y; };
	*  OrderBy(ls, func);
	* 
	*  Minimum c++ standard: c++17.
	*/
	template<typename TContainer, typename TFunc>
	void OrderBy(TContainer& container, const TFunc& func)
	{
		if constexpr (is_vector<TContainer>::value)
		{
			std::sort(container.begin(), container.end(), func);
		}
		if constexpr (is_list<TContainer>::value)
		{
			container.sort(func);
		}
	}
#endif

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201402L) || __cplusplus >= 201402L)
	/* GroupBy:
	*  It Allows to group containers of any type based on any data member of that type.
	* 
	*  Returns: a map whose key is the grouping data member and whose value is a container
	*  of elements share the same value of the key.
	* 
	*  Usage:
	*  struct MyStruct{ int x,y; };
	*  vector<MyStruct> ls{ {1,4} ,{3,4} ,{1,4}, {2,7}, {1,1}};
	*  auto func = [](const MyStruct& myStruct) { return myStruct.x; };
	*  auto res = GroupBy(ls, func);
	* 
	*  Minimum c++ standard: c++14.
	*/
	template<template<typename...> typename TContainer, 
		typename TElement, 
		typename TGroupingMemberFunc>
	auto GroupBy(const TContainer<TElement>& container, const TGroupingMemberFunc& groupingMemberFunc)
	{
		TElement containerElement;
		using GroupingMemberType = decltype(groupingMemberFunc(containerElement));
		std::map<GroupingMemberType, TContainer<TElement>> result;
		for (auto elem : container)
		{
			result[groupingMemberFunc(elem)].push_back(elem);
		}
		return result;
	}

	/* Join:
	*  It Allows to join 2 containers of any type based on shared data member.
	* 
	*  Returns: a map whose key is the joining data member and whose value is a pair
	*  of containers(subsets of original 2 containers) of elements share the same value of the key.
	* 
	*  Usage:
	*  struct MyStruct1{ int x,y; };
	*  struct MyStruct2{ int w,z; };
	*  vector<MyStruct1> vec{ {1,4} ,{3,4} ,{1,4}, {2,7}, {1,1}};
	*  list<MyStruct2> ls{ {1,1} ,{3,4} ,{1,4}, {2,8}, {1,1} };
	*  auto func1 = [](const MyStruct1& myStruct) { return myStruct.y; };
	*  auto func2 = [](const MyStruct2& myStruct) { return myStruct.z; };
	*  auto res = Join(vec, ls, func1, func2);
	* 
	*  Minimum c++ standard: c++14.
	*/
	template<template<typename...> typename TContainer1,
		template<typename...> typename TContainer2,
		typename TElement1, 
		typename TElement2, 
		typename TJoiningMemberFunc1, 
		typename TJoiningMemberFunc2>
	auto Join(const TContainer1<TElement1>& container1, 
		const TContainer2<TElement2>& container2, 
		const TJoiningMemberFunc1& joiningMemberFunc1, 
		const TJoiningMemberFunc2& joiningMemberFunc2)
	{
		TElement1 containerElement1;
		using GroupingMemberType = decltype(joiningMemberFunc1(containerElement1));
		std::map<GroupingMemberType, std::pair<TContainer1<TElement1>, TContainer2<TElement2>>> result;

		auto group1 = GroupBy(container1, joiningMemberFunc1);
		auto group2 = GroupBy(container2, joiningMemberFunc2);

		for (auto elem : group1)
		{
			if (group2.find(elem.first) != group2.end())
				result[elem.first] = std::make_pair(elem.second, group2[elem.first]);
		}

		return result;
	}
#endif

#if (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
	template<template<typename...> typename TContainer, typename TElement, typename TFunc>
	generator<TElement> WhereLazy(const TContainer<TElement>& container, const TFunc& predicate)
	{
		for (auto element : container)
		{
			if (predicate(element))
				co_yield(element);
		}
	}

	template<template<typename...> typename TContainer, typename TElement>
	generator<TElement> DistinctLazy(const TContainer<TElement>& container)
	{
		TContainer<TElement> result;
		for (auto element : container)
		{
			if (std::find(result.begin(), result.end(), element) == result.end())
			{
				result.push_back(element);
				co_yield(element);
			}
		}
	}
#endif
}

#endif // !CONTAINER_QUERY_LIBRARY