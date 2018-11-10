CC=gcc
CFLAGS=-D_GNU_SOURCE -I./kv_store -I./test_director
DEPS = $(wildcard *.h)

KV_STOR_OBJS = kv_store/kv_store.o kv_store/kv_append_point.o kv_store/kv_block_allocator.o kv_store/kv_block_array.o kv_store/kv_directory.o kv_store/kv_block.o
TEST_DIRECTOR_OBJS = test_director/test_director.o test_director/kv_checker.o

KV_HARNESS_OBJS = kv_harness.o
KV_STRESS_OBJS = kv_stress.o

all: kv_harness kv_stress

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

kv_harness: $(KV_HARNESS_OBJS) $(KV_STOR_OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

kv_stress: $(KV_STRESS_OBJS) $(KV_STOR_OBJS) $(TEST_DIRECTOR_OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm kv_store/*.o test_director/*.o *.o kv_harness kv_stress

test: all
	./kv_harness delme.kv
	rm delme.kv
	./kv_stress delme.kv
	rm delme.kv
