#pragma once
#ifndef CONTAINER_QUERY_LIBRARY
#define CONTAINER_QUERY_LIBRARY

#include<vector>
#include<list>
#include<map>
#include<algorithm>

#if (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
#include <experimental/generator>
#endif

namespace cql
{
	template <typename Type>
	struct IsVector : public std::false_type {};
	template <typename... A>
	struct IsVector<std::vector<A...> > : public std::true_type {};

	template <typename Type>
	struct IsList : public std::false_type {};
	template <typename... A>
	struct IsList<std::list<A...> > : public std::true_type {};

	/* Select Query:
	*  For a given container it returns a list of whatever the selector returns
	*  from each element of the container.
	*
	*  Returns: A list of selected things from each element.
	*
	*  Usage:
	*  struct Employee { int id; std::string name; std::string address; };
    *  std::list<Employee> ls{ {1, "Jack", "Kaiserslautern"}, {2, "Jill", "Berlin"} };

	*  auto res = Select(ls, [](const Employee& emp) { return emp.name;});  // Retruns a list of employees names.
	*
	*  Minimum C++ standard: C++11.
	*/
	template<typename TContainer, typename TFunc>
	auto Select(const TContainer& container, const TFunc& selector)
	{
		using type = decltype(selector(*container.begin()));
		std::list<type> result;
		for (auto& element : container)
		{
			result.push_back(selector(element));
		}
		return result;
	}

	/* Where Statement:
	*  It filters any container with push_back modifier based on a predicate. 
	* 
	*  Returns: A container of elements that satisfy the predicate.
	* 
	*  Usage:
	*  list<int> ls{1,2,3,4,5};
	*  auto predicate = [](const int& v){ return v%2 == 0; };
	*  auto res = Where(ls, predicate);
	* 
	*  Minimum C++ standard: C++11.
	*/
	template<template<typename...> typename TContainer, typename TElement, typename TFunc>
	TContainer<TElement> Where(const TContainer<TElement>& container, const TFunc& predicate)
	{
		TContainer<TElement> result;
		for (auto& element : container)
		{
			if (predicate(element))
				result.push_back(element);
		}
		return result;
	}

	/* Update Statement:
	*  It updates a container based on an updating function.
	*
	*  Usage:
	*  list<int> ls{1,2,3,4,5};
	*  auto setFunc = [](int& v){ v = 10; };
	*  Update(ls, setFunc);
	*/
	template<typename TContainer, typename TSetFunc>
	void Update(TContainer& container, const TSetFunc& setFunc)
	{
		for (auto& element : container)
		{
			setFunc(element);
		}
	}

	/* Update Statement:
	*  It updates a container based on an updating function and a predicate.
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
		for (auto &element : container)
		{
			if (predicate(element))
				setFunc(element);
		}
	}

	/* Delete Statement:
	*  It removes elements from a container based on a predicate.
	*
	*  Usage:
	*  list<int> ls{1,7,3,4,7};
	*  auto predicate = [](const int& v){ return v == 7; };
	*  Delete(ls, predicate);
	*/
	template<typename TContainer, typename TFunc>
	void Delete(TContainer& container, const TFunc& predicate)
	{
		container.erase(std::remove_if(container.begin(), container.end(), predicate), container.end());		
	}

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
	/* OrderBy:
	*  It sorts std::vector and std::list containers of any type based on an ordering function.
	* 
	*  Usage:
	*  struct MyStruct{ int x,y; };
	*  vector<MyStruct> ls{ {1,4} ,{3,4} ,{1,4}, {2,7}, {1,1}};
	*  auto func = [](const MyStruct& l, const MyStruct& r) { return l.y < r.y; };
	*  OrderBy(ls, func);
	* 
	*  Minimum C++ standard: C++17.
	*/
	template<typename TContainer, typename TFunc>
	void OrderBy(TContainer& container, const TFunc& func)
	{
		if constexpr (IsVector<TContainer>::value)
		{
			std::sort(container.begin(), container.end(), func);
		}
		if constexpr (IsList<TContainer>::value)
		{
			container.sort(func);
		}
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
		TContainer result = container;
		if constexpr (IsVector<TContainer>::value)
		{
			std::sort(result.begin(), result.end());
			result.erase(std::unique(result.begin(), result.end()), result.end());
		}
		if constexpr (IsList<TContainer>::value)
		{
			result.sort();
			result.unique();
		}
		return result;
	}
#endif

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201402L) || __cplusplus >= 201402L)
	/* GroupBy:
	*  It groups containers of any type based on any data member of that type.
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
	*  Minimum C++ standard: C++14.
	*/
	template<template<typename...> typename TContainer, 
		typename TElement, 
		typename TGroupingMemberFunc>
	auto GroupBy(const TContainer<TElement>& container, const TGroupingMemberFunc& groupingMemberFunc)
	{
		TElement containerElement;
		using GroupingMemberType = decltype(groupingMemberFunc(containerElement));
		std::map<GroupingMemberType, TContainer<TElement>> result;
		for (auto& elem : container)
		{
			result[groupingMemberFunc(elem)].push_back(elem);
		}
		return result;
	}

	/* Join:
	*  It joins 2 containers of any type based on shared data member.
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
	*  Minimum C++ standard: C++14.
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

		for (auto& elem : group1)
		{
			if (group2.find(elem.first) != group2.end())
				result[elem.first] = std::make_pair(elem.second, group2[elem.first]);
		}

		return result;
	}
#endif

#if (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
	template<template<typename...> typename TContainer, typename TElement, typename TFunc>
	std::experimental::generator<TElement> WhereLazy(const TContainer<TElement>& container, const TFunc& predicate)
	{
		for (auto& element : container)
		{
			if (predicate(element))
				co_yield(element);
		}
	}

	template<template<typename...> typename TContainer, typename TElement>
	std::experimental::generator<TElement> DistinctLazy(const TContainer<TElement>& container)
	{
		TContainer<TElement> result;
		for (auto& element : container)
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