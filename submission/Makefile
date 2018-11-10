CC=gcc
CFLAGS=-D_GNU_SOURCE
DEPS = $(wildcard *.h)
KV_STOR_OBJS = kv_store.o kv_append_point.o kv_block_allocator.o kv_block_array.o kv_directory.o kv_block.o
KV_HARNESS_OBJS = kv_harness.o
KV_STRESS_OBJS = kv_stress.o test_director.o kv_checker.o

all: kv_harness kv_stress

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

kv_harness: $(KV_HARNESS_OBJS) $(KV_STOR_OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

kv_stress: $(KV_STRESS_OBJS) $(KV_STOR_OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm *.o kv_harness kv_stress

test: all
	./kv_harness delme.kv
	rm delme.kv
	./kv_stress delme.kv
	rm delme.kv
