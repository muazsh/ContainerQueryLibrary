# Container Query Library
This library provides c++ containers with basic C# LINQ functions namely: `Where`, `Distinct`, `OrderBy`, `GroupBy` and `Join`.

The supported containers are std::vector and std::list. However, for some functions all types of containers are supported.

Currently `c++17` suppotrs all of this library functions, `c++14` supports all except of `OrderBy`, and `c++11` supports only `Where` and `Distinct`.

# Summary:
* Where:
	*  It allows to filter any container with push_back modifier based on a predicate. 
	*  Returns: A container of elements that satisfy the predicate.
	*  Usage:
  ```
	  list<int> ls{1,2,3,4,5}
	  auto predicate = [](const int& v){ return v%2 == 0; }
	  auto res = Where(ls, predicate); 
  ```
	*  Minimum c++ standard: c++11.

* Distinct:
	*  Returns: A distinct container of the original one.
	*  Usage:
  ```
	  std::list<int> ls{ 11,11,2,2,3,5,6 };
	  auto res = Distinct(ls);
  ```
  
* OrderBy:
	*  It Allows to sort std::vector and std::list containers of any type based on an ordering function.
	*  Usage:
  ```
	  struct MyStruct{ int x,y; };
	  std::vector<MyStruct> ls{ {1,4} ,{3,4} ,{1,4}, {2,7}, {1,1}};
	  auto func = [](const MyStruct& l, const MyStruct& r) { return l.y < r.y; }
	  OrderBy(ls, func);
  ```
	*  Minimum c++ standard: c++17.
  
* GroupBy:
	*  It Allows to group containers of any type based on any data member of that type.
  *  Returns: a map whose key is the grouping data member and whose value is a container of elements share the same value of the key.
	*  Usage:
  ```
	  struct MyStruct{ int x,y; };
	  std::vector<MyStruct> ls{ {1,4} ,{3,4} ,{1,4}, {2,7}, {1,1}};
	  auto func = [](const MyStruct& myStruct) { return myStruct.x; }
	  auto res = GroupBy(ls, func);
  ```
	*  Minimum c++ standard: c++14.
  
* Join:
	*  It Allows to join 2 containers of any type based on shared data member.
  *  Returns: a map whose key is the joining data member and whose value is a pair of containers(subsets of original 2 containers) of elements share the same value of the key.
	*  Usage:
  ```
	  struct MyStruct1{ int x,y; };
	  struct MyStruct2{ int w,z; };
	  std::vector<MyStruct1> vec{ {1,4} ,{3,4} ,{1,4}, {2,7}, {1,1}};
	  std::list<MyStruct2> ls{ {1,1} ,{3,4} ,{1,4}, {2,8}, {1,1} };
	  auto func1 = [](const MyStruct1& myStruct) { return myStruct.y; }
	  auto func2 = [](const MyStruct2& myStruct) { return myStruct.z; }
	  auto res = Join(vec, ls, func1, func2);
  ```
	*  Minimum c++ standard: c++14.
  
  
