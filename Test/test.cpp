#include "pch.h"


TEST(ContainerQueryLibrary, Select) {
	struct Employee { int id; std::string name; std::string address; };
	std::list<Employee> ls{ {1, "Jack", "Kaiserslautern"}, {2, "Jill", "Berlin"} };

	auto res1 = cql::Select(ls, [](const Employee& emp) { return emp.name; });
	EXPECT_EQ(res1.size(), 2);
	EXPECT_EQ(res1.front(), "Jack");
	EXPECT_EQ(res1.back(), "Jill");

	auto res2 = cql::Select(ls, [](const Employee& emp) { return std::make_pair(emp.id, emp.address); });
	EXPECT_EQ(res2.size(), 2);
	EXPECT_EQ(res2.front().first, 1);
	EXPECT_EQ(res2.front().second, "Kaiserslautern");
	EXPECT_EQ(res2.back().first, 2);
	EXPECT_EQ(res2.back().second, "Berlin");

	std::list<Employee> ls2{};
	auto res3 = cql::Select(ls2, [](const Employee& emp) { return emp.name; });
	EXPECT_EQ(res3.size(), 0);
}

TEST(ContainerQueryLibrary, Where) {
	std::list<int> ls{ 1,2,3,4,5 };
	auto predicate = [](const int& v) { return v % 2 == 0; };
	auto res = cql::Where(ls, predicate);
	EXPECT_EQ(res.size() , 2);
	EXPECT_EQ(res.front(), 2);
	EXPECT_EQ(res.back(), 4);
}

TEST(ContainerQueryLibrary, WhereLazy) {
	std::vector<int> ls{ 1,2,3,4,5 };
	std::vector<int> expected{ 2,4 };
	auto predicate = [](const int& v) { return v % 2 == 0; };
	auto res = cql::WhereLazy(ls, predicate);
	int size = 0;
	auto itr = expected.begin();
	for (auto elem : res)
	{
		size++;
		EXPECT_EQ(elem, *itr);
		itr++;
	}
	EXPECT_EQ(size, 2);
}

TEST(ContainerQueryLibrary, UpdateWithoutPredicate) {
	std::list<int> ls{ 1,2,3,4,5 };
	auto setFunc = [](int& v) { v = 10; };
	cql::Update(ls, setFunc);
	for (int elem : ls)
	{
		EXPECT_EQ(elem, 10);
	}
}

TEST(ContainerQueryLibrary, UpdateWithPredicate) {
	std::vector<int> ls{ 1,2,3,4,5 };
	auto predicate = [](const int& v) { return v % 2 == 0; };
	auto setFunc = [](int& v) { v = 10; };
	cql::Update(ls, predicate, setFunc);
	EXPECT_EQ(ls.size(), 5);
	EXPECT_EQ(ls[0], 1);
	EXPECT_EQ(ls[1], 10);
	EXPECT_EQ(ls[2], 3);
	EXPECT_EQ(ls[3], 10);
	EXPECT_EQ(ls[4], 5);
}

TEST(ContainerQueryLibrary, Delete) {
	std::vector<int> ls{ 1,7,3,4,7 };
	auto predicate = [](const int& v) { return v == 7; };
	cql::Delete(ls, predicate);
	EXPECT_EQ(ls.size(), 3);
	EXPECT_EQ(ls[0], 1);
	EXPECT_EQ(ls[1], 3);
	EXPECT_EQ(ls[2], 4);
}

TEST(ContainerQueryLibrary, OrderBy) {
	struct MyStruct { int x, y; };
	std::vector<MyStruct> ls{ {1,4} ,{3,4} ,{1,4}, {2,7}, {1,1} };
	auto func = [](const MyStruct& l, const MyStruct& r) { return l.y < r.y; };
	cql::OrderBy(ls, func);
	EXPECT_EQ(ls[0].y, 1);
	EXPECT_EQ(ls[1].y, 4);
	EXPECT_EQ(ls[2].y, 4);
	EXPECT_EQ(ls[3].y, 4);
	EXPECT_EQ(ls[4].y, 7);
}

TEST(ContainerQueryLibrary, Distinct) {
	std::vector<int> ls{ 11,11,2,2,3,5,6 };
	auto res = cql::Distinct(ls);
	EXPECT_EQ(res.size(), 5);
	EXPECT_EQ(res[0], 2);
	EXPECT_EQ(res[1], 3);
	EXPECT_EQ(res[2], 5);
	EXPECT_EQ(res[3], 6);
	EXPECT_EQ(res[4], 11);
}

TEST(ContainerQueryLibrary, DistinctLazy) {
	std::vector<int> ls{ 11,11,2,2,3,5,6 };
	std::vector<int> expected{ 11,2,3,5,6 };
	auto res = cql::DistinctLazy(ls);
	int size = 0;
	auto itr = expected.begin();
	for (auto elem : res)
	{
		size++;
		EXPECT_EQ(elem, *itr);
		itr++;
	}
	EXPECT_EQ(size, 5);
}

TEST(ContainerQueryLibrary, GroupBy) {
	struct MyStruct { int x, y; };
	std::vector<MyStruct> ls{ {5,7} ,{3,9} ,{5,4}, {2,6} };
	auto func = [](const MyStruct& myStruct) { return myStruct.x; };
	auto res = cql::GroupBy(ls, func);
	EXPECT_EQ(res.size(), 3);

	EXPECT_EQ(res[5].size(), 2);
	EXPECT_EQ(res[5][0].y, 7);
	EXPECT_EQ(res[5][1].y, 4);

	EXPECT_EQ(res[2].size(), 1);
	EXPECT_EQ(res[2][0].y, 6);

	EXPECT_EQ(res[3].size(), 1);
	EXPECT_EQ(res[3][0].y, 9);
}

TEST(ContainerQueryLibrary, Join) {
	struct MyStruct1 { int x, y; };
	struct MyStruct2 { int w, z; };
	std::vector<MyStruct1> vec{ {1,4} ,{3,4} ,{2,9}, {2,7}, {12,1} };
	std::list<MyStruct2> ls{ {2,1} ,{4,4} ,{5,1}, {2,8}, {3,6} };
	auto func1 = [](const MyStruct1& myStruct) { return myStruct.y; };
	auto func2 = [](const MyStruct2& myStruct) { return myStruct.z; };
	auto res = cql::Join(vec, ls, func1, func2);

	EXPECT_EQ(res.size(), 2);

	EXPECT_EQ(res[4].first.size(), 2);
	EXPECT_EQ(res[4].second.size(), 1);
	EXPECT_EQ(res[4].first[0].x, 1);
	EXPECT_EQ(res[4].first[1].x, 3);
	EXPECT_EQ(res[4].second.front().w, 4);
	
	EXPECT_EQ(res[1].first.size(), 1);
	EXPECT_EQ(res[1].second.size(), 2);
	EXPECT_EQ(res[1].first[0].x, 12);
	EXPECT_EQ(res[1].second.front().w, 2);
	EXPECT_EQ(res[1].second.back().w, 5);

}

