# Container Query Library

[![MSBuild](https://github.com/muazsh/ContainerQueryLibrary/actions/workflows/msbuild.yml/badge.svg)](https://github.com/muazsh/ContainerQueryLibrary/actions/workflows/msbuild.yml)

This library provides C++ containers with basic SQL statements functionalities (or basic C# LINQ methods) namely: `Select`, `Where`, `Update`, `Delete`, `Distinct`, `OrderBy`, `GroupBy` and `Join`. Also it includes `WhereLazy` and `DistinctLazy` which are **lazy** versions of `Where` and `Distinct` respectively. 

The supported containers are std::vector and std::list. However; for some functions all types of containers are supported.

`C++17` suppotrs all of this library functions, `C++14` supports all except of `Distinct` and `OrderBy`, and `C++11` supports only `Select`, `Where`, `Update` and `Delete`.

# Description:
* Select:
	*  For a given container it returns a list of whatever the selector returns
	*  from each element of the container.
	*  Returns: A list of selected things from each element.
	*  Usage:
	```
	   struct Employee { int id; std::string name; std::string address; };
	   std::list<Employee> ls{ {1, "Jack", "Kaiserslautern"}, {2, "Jill", "Berlin"} };
        
	   auto res = Select(ls, [](const Employee& emp) { return emp.name;});
	   
	   // Output: {"Jack","Jill"}
	```
	*  Minimum Standard: C++11.
	*  Complexity: O(n).
	
* Where:
	*  Filters any container with push_back modifier based on a predicate. 
	*  Returns: A container of elements that satisfies the predicate.
	*  Usage:
  ```
	  list<int> ls{1,2,3,4,5};
	  auto predicate = [](const int& v){ return v%2 == 0; };
	  auto res = Where(ls, predicate); 
	  
	  // Output: {2,4}
  ```
	*  Minimum Standard: C++11.
	*  Complexity: O(n).
	*  A lazy version `WhereLazy` of this query is included, and it requires microsoft compiler of at least C++17.

* Update:
	*  Updates a container based on an updating function and a predicate.
	*  Usage:
  ```
	  list<int> ls{1,2,3,4,5};
	  auto set = [](int& v){ v = 10; };
	  auto predicate = [](const int& v){ return v%2 == 0; };
	  Update(ls, predicate, set);
	  
	  // Output: {1,10,3,10,5}
  ```
	*  Minimum Standard: C++11.
	*  Complexity: O(n).
	*  In case the predicate function is missing it applies changes to all elements.

* Delete:
	*  Removes elements from a container based on a predicate.
	*  Usage:
  ```
	  list<int> ls{1,7,3,4,7};
	  auto predicate = [](const int& v){ return v == 7; };
	  Delete(ls, predicate);
	  
	  // Output: {1,3,4}
  ```
	*  Minimum Standard: C++11.
	*  Complexity: O(n).
	
* Distinct:
	*  Returns: A distinct container of the original one.
	*  Usage:
  ```
	  list<int> ls{ 11,11,2,2,3,5,6 };
	  auto res = Distinct(ls);
	  
	  // Output: {11,2,3,5,6}
  ```
  	*  Minimum Standard: C++17.
	*  Complexity: O(n.log(n)).
  	*  A lazy version `DistinctLazy` of this query is included, and it requires microsoft compiler with of least C++17.
  
* OrderBy:
	*  Sorts std::vector and std::list containers of any type based on an ordering function.
	*  Usage:
  ```
	  struct MyStruct{ int x,y; };
	  vector<MyStruct> ls{ {3,5} ,{1,4}, {2,7}, {1,1}};
	  auto func = [](const MyStruct& l, const MyStruct& r) { return l.y < r.y; };
	  OrderBy(ls, func);
	  
	  // Output: { {1,1},{1,4},{3,5},{2,7} }
  ```
	*  Minimum Standard: C++17.
	*  Complexity: O(n.log(n)).
  
* GroupBy:
	*  Groups containers of any type based on a condition.
	*  Returns: A map whose key is the grouping data member and whose value is a container of elements share the same value of the key.
	*  Usage:
  ```
	  struct MyStruct{ int x,y; };
	  vector<MyStruct> ls{ {1,4} ,{3,4} ,{1,4}, {2,7}, {1,1}};
	  auto func = [](const MyStruct& myStruct) { return myStruct.x; };
	  auto res = GroupBy(ls, func);
	  
	  // Output: [1,{ {1,4},{1,4},{1,1} }]
	  //         [3,{ {3,4} }]
	  //         [2,{ {2,7} }]
  ```
	*  Minimum Standard: C++14.
	*  Complexity: O(n).
  
* Join:
	*  Joins 2 containers of any type based on a shared data member.
	*  Returns: A map whose key is the joining data member and whose value is a pair of containers(subsets of original 2 containers) of elements share the same value of the key.
	*  Usage:
  ```
	  struct MyStruct1{ int x,y; };
	  struct MyStruct2{ int w,z; };
	  vector<MyStruct1> vec{ {1,4} ,{3,4} ,{1,4}, {2,7}, {1,1}};
	  list<MyStruct2> ls{ {2,1} ,{3,4} ,{1,4}, {2,8}, {3,1} };
	  auto func1 = [](const MyStruct1& myStruct) { return myStruct.y; };
	  auto func2 = [](const MyStruct2& myStruct) { return myStruct.z; };
	  auto res = Join(vec, ls, func1, func2);
	  
	  // Output: [4,<{ {1,4},{3,4},{1,4} },{ {3,4},{1,4} }>]
	  //         [1,<{ {1,1} },{ {2,1},{3,1} }>]
  ```
	*  Minimum Standard: C++14.
	*  Complexity: O(n.m) where n is the first container size and m is the second one's.
  
  
