CC = gcc
CFLAGS = -Wall -fPIC
LDFLAGS = -ldl

STATIC_LIB = libfredjohn.a
SHARED_LIB = libbillsam.so
INTERPOSE_LINK = libmymalloc.so
INTERPOSE_LOAD = libwrapper.so
TARGET = program
TARGET_INTERPOSED = program_interposed

STATIC_SRCS = fred.c john.c
SHARED_SRCS = bill.c sam.c
INTERPOSE_LINK_SRC = my_malloc.c
INTERPOSE_LOAD_SRC = wrapper.c
MAIN_SRC = program.c

STATIC_OBJS = $(STATIC_SRCS:.c=.o)
SHARED_OBJS = $(SHARED_SRCS:.c=.o)
MAIN_OBJ = $(MAIN_SRC:.c=.o)

all: clean static_lib shared_lib program

static_lib: $(STATIC_OBJS)
	ar crv $(STATIC_LIB) $(STATIC_OBJS)
	ranlib $(STATIC_LIB)

shared_lib: $(SHARED_OBJS)
	$(CC) -shared -o $(SHARED_LIB) $(SHARED_OBJS) -lm

program: $(MAIN_OBJ) static_lib shared_lib
	$(CC) -o $(TARGET) $(MAIN_OBJ) -L. -lfredjohn -lbillsam -lm

link_interpose: $(INTERPOSE_LINK_SRC)
	$(CC) -shared -fPIC -o $(INTERPOSE_LINK) $(INTERPOSE_LINK_SRC)

load_interpose: $(INTERPOSE_LOAD_SRC)
	$(CC) -shared -fPIC -o $(INTERPOSE_LOAD) $(INTERPOSE_LOAD_SRC) $(LDFLAGS)

program_link_interposed: clean static_lib shared_lib link_interpose
	$(CC) -o $(TARGET_INTERPOSED) $(MAIN_SRC) \
		-Wl,--wrap=fred \
		-Wl,--wrap=john \
		-L. -lfredjohn -lbillsam -lm

all_interposed: clean static_lib shared_lib link_interpose load_interpose
	$(CC) -o $(TARGET_INTERPOSED) $(MAIN_SRC) \
		-Wl,--wrap=fred \
		-Wl,--wrap=john \
		-L. -lfredjohn -lbillsam -lm

run: program
	@echo "\n=== Running without interposition ==="
	export LD_LIBRARY_PATH=.; ./$(TARGET)

run_link: program_link_interposed
	@echo "\n=== Running with LINK-TIME interposition (fred, john) ==="
	export LD_LIBRARY_PATH=.; ./$(TARGET_INTERPOSED)

run_load: program load_interpose
	@echo "\n=== Running with LOAD-TIME interposition (bill, sam) ==="
	export LD_LIBRARY_PATH=.; export LD_PRELOAD=./$(INTERPOSE_LOAD); ./$(TARGET)

run_both: program_link_interposed load_interpose
	@echo "\n=== Running with BOTH interposition methods ==="
	@echo "Link-time: fred, john | Load-time: bill, sam"
	export LD_LIBRARY_PATH=.; export LD_PRELOAD=./$(INTERPOSE_LOAD); ./$(TARGET_INTERPOSED)

check_libs: program
	@echo "\n=== Loaded libraries ==="
	export LD_LIBRARY_PATH=.; ldd ./$(TARGET)

clean:
	rm -f *.o *.a *.so $(TARGET) $(TARGET_INTERPOSED)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

help:
	@echo "Available targets:"
	@echo "  make run          - Run normal program"
	@echo "  make run_link     - Run with link-time interposition (fred, john)"
	@echo "  make run_load     - Run with load-time interposition (bill, sam)"
	@echo "  make run_both     - Run with both interposition methods"
	@echo "  make check_libs   - Show loaded libraries"
	@echo "  make clean        - Remove all build artifacts"

.PHONY: all clean run run_link run_load run_both check_libs help