CC=gcc
CXX=g++
CFLAGS=-D_GNU_SOURCE -I./kv_store -I./test_director

KV_STOR_OBJS = kv_store/kv_store.o kv_store/kv_append_point.o kv_store/kv_block_allocator.o kv_store/kv_block_array.o kv_store/kv_directory.o kv_store/kv_block.o
TEST_DIRECTOR_OBJS = test_director/test_director.o test_director/kv_checker.o

KV_HARNESS_OBJS = kv_harness.o
KV_STRESS_OBJS = kv_stress.o

CPPU_TEST_OBJS = $(patsubst %.cpp,%.o,$(wildcard cpputest/src/CppUTest/*.cpp) $(wildcard cpputest/src/CppUTestExt/*.cpp) $(wildcard cpputest/src/Platforms/Gcc/*.cpp))

all: kv_harness kv_stress

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

%.o: %.cpp
	$(CXX) -c -o $@ $< $(CFLAGS) -I./cpputest/include

libcpputest.a: $(CPPU_TEST_OBJS)
	ar r $@ $^
	ranlib $@

unittests: kv_block_allocator_unittest kv_directory_unittest kv_append_point_unittest kv_store_unittest

kv_append_point_unittest: libcpputest.a test/kv_append_point_unittest.o kv_store/kv_append_point.o test/kv_block_allocator_mock.o 
	$(CXX) -o $@ $^ $(CFLAGS) -L. -lcpputest
	./kv_append_point_unittest

kv_block_allocator_unittest: libcpputest.a test/kv_block_allocator_unittest.o kv_store/kv_block_allocator.o
	$(CXX) -o $@ $^ $(CFLAGS) -L. -lcpputest
	./kv_block_allocator_unittest

kv_directory_unittest: libcpputest.a test/kv_directory_unittest.o kv_store/kv_directory.o
	$(CXX) -o $@ $^ $(CFLAGS) -L. -lcpputest
	./kv_directory_unittest

kv_store_unittest: libcpputest.a test/kv_store_unittest.o kv_store/kv_store.o test/kv_block_allocator_mock.o test/kv_append_point_mock.o test/kv_directory_mock.o test/kv_block_mock.o test/kv_block_array_mock.o
	$(CXX) -o $@ $^ $(CFLAGS) -L. -lcpputest
	./kv_store_unittest

kv_harness: $(KV_HARNESS_OBJS) $(KV_STOR_OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

kv_stress: $(KV_STRESS_OBJS) $(KV_STOR_OBJS) $(TEST_DIRECTOR_OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm kv_store/*.o test_director/*.o test/*.o *.o kv_harness kv_stress *_unittest

test: all unittests
	./kv_harness delme.kv
	rm delme.kv
	./kv_stress delme.kv
	rm delme.kv
