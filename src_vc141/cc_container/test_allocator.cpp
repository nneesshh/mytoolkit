#include <string>
#include <map>

#include "malloc_allocator.h"
#include "new_delete_allocator.h"
#include "FSBAllocator.h"
#include "FSBPoolAllocator.h"

struct test_t {
	test_t() {
		printf("construct, %s\n", c.c_str());
	}

	~test_t() {
		printf("destruct, %s\n", c.c_str());
	}

	int a;
	int b;
	std::string c;
};

void test_allocator1() {


	std::map<uint64_t, test_t, std::less<uint64_t>, mp::malloc_allocator<std::pair<const uint64_t, test_t>>> mapTest1;
	std::map<uint64_t, test_t, std::less<uint64_t>, mp::new_delete_allocator<std::pair<const uint64_t, test_t>>> mapTest2;
	std::map<uint64_t, test_t, std::less<uint64_t>, mp::FSBAllocator<std::pair<const uint64_t, test_t>>> mapTest3;
	std::map<uint64_t, test_t, std::less<uint64_t>, mp::FSBPoolAllocator<std::pair<const uint64_t, test_t>>> mapTest4;

	auto& mapTest = mapTest4;
	test_t& t1 = mapTest[100001];
	test_t& t2 = mapTest[100002];

	t1.a = 11111;
	t1.b = 22222;
	t1.c = "asdadasdadasdad33333";

	t2.a = 911111;
	t2.b = 922222;
	t2.c = "xxxxxxxxxxxxxxxxxxxxxxxxxxx1234567890";

}