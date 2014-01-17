#include <UnitTest++.h>
#include <ReportAssert.h>
#include <iostream>

#include <df/system/Array.h>

namespace {

TEST(check_constructor_empty)
{		
	df::Array<int> array1;	
	CHECK(array1.size() == 0);
	CHECK(array1.reserved_size() == 0);	
}

TEST(check_constructor_reserve)
{
	df::Array<int> array2(10);
	CHECK(array2.size() ==0);
	CHECK(array2.reserved_size() == 10);
}

TEST(check_copy_constructor)
{
	df::Array<int> array1;
	array1.push_back(1);
	array1.push_back(2);

	df::Array<int> array2(array1);
	
	CHECK(array2.size() == array1.size());
	for(uint32_t i=0; i<array1.size(); ++i)
	{
		CHECK(array2[i] == array1[i]);
	}
}

TEST(check_assign)
{
	df::Array<int> array1;
	array1.push_back(1);
	array1.push_back(2);

	df::Array<int> array2;
	array2 = array1;
	
	CHECK(array2.size() == array1.size());
	for(uint32_t i=0; i<array1.size(); ++i)
	{
		CHECK(array2[i] == array1[i]);
	}
}

TEST(check_reserve)
{	
	df::Array<int> array1;
	array1.reserve(25);
	CHECK(array1.reserved_size() >= 25);
}

TEST(check_resize)
{	
	df::Array<int> array1;
	array1.resize(25);
	CHECK(array1.size() == 25);
	CHECK(array1.reserved_size() >= 25);

	array1.resize(10);
	CHECK(array1.size() == 10);
	CHECK(array1.reserved_size() >= 25);

	array1.resize(0);
	array1.resize(10, 5);
	for(int i=0;i<10;++i)
	{
		CHECK(array1[i] == 5);
	}	
}

TEST(check_clear)
{	
	df::Array<int> array1;
	array1.resize(10);	
	array1.clear();	
}

TEST(check_push_back)
{	
	df::Array<int> array1;
	for(int i = 0; i<10; ++i)
	{
		array1.push_back(i);
	}
	CHECK(array1.size() == 10);
	for(int i = 0; i<10; ++i)
	{
		CHECK(array1[i] == i);
	}
	// now pop
	for(int i = 0; i<5; ++i)
	{
		array1.pop_back();
	}
	CHECK(array1.size() == 5);
	for(int i = 0; i<5; ++i)
	{
		CHECK(array1[i] == i);
	}
}

TEST(check_insert_remove)
{
	df::Array<int> array1;
	for(int i = 1; i<10; ++i)
	{
		array1.push_back(i);
	}
	array1.insert(0,0);
	for(int i = 0; i<10; ++i)
	{
		CHECK(array1[i] == i);
	}

	array1.remove(5);
	CHECK(array1[5] == 6);
	array1.remove(5, 4);
	CHECK(array1.size() == 5);
	for(size_t i = 0; i<5; ++i)
	{
		CHECK(array1[i] == i);
	}

	array1.unsorted_remove(1);
	for(size_t i = 0; i<array1.size(); ++i)
	{
		CHECK(array1[i] != 1);
	}

}


}