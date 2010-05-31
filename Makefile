# This file doesn't contain any changes specific to the local
# installation (eg. any specific library paths etc.). See the
# .config file for these.
#

-include .config
-include */*.d

# Compilers and mandatory libraries
CXX ?= g++
CC ?= $(CXX)
LDLIBS += -lstdc++

# Boost specific flags
LDLIBS_BOOST ?=
LDFLAGS_BOOST ?=
CPPFLAGS_BOOST ?= -I/opt/local/include

# Compiler flags and optional additional Boost flags too!
CXXFLAGS += -Wall -pedantic -ansi -Wno-long-long -MMD -MP
CPPFLAGS += -I. $(CPPFLAGS_BOOST)
LDFLAGS += $(LDFLAGS_BOOST)
LDLIBS += $(LDLIBS_BOOST)

# Suffix rules
%.o:	%.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -o $@ $<

# Compile object files
SCHEDULER_OBJECTS := $(patsubst %.cpp,%.o,$(wildcard sched/*.cpp))
UTILITY_OBJECTS := $(patsubst %.cpp,%.o,$(wildcard utility/*.cpp))
COMMON_OBJECTS := $(patsubst %.cpp,%.o,$(wildcard common/*.cpp))
TEST_OBJECTS := $(patsubst %.cpp,%.o,$(wildcard test/*.cpp))
URL := $(patsubst %.cpp,%.o,$(wildcard url/*.cpp))

# Binary targets
TESTS = test/html-parser \
	test/word-indexer \
	test/url-parser \
	test/url-tree \
	test/url-scheduler

test/html-parser: test/html-parser.o \
	$(COMMON_OBJECTS) \
	$(URL) \
	$(UTILITY_OBJECTS) \
	$(SCHEDULER_OBJECTS) ;\
	$(CXX) $(LDFLAGS) -o bin/$@ $^ $(LDLIBS)

test/word-indexer: test/word-indexer.o \
	$(COMMON_OBJECTS) \
	$(URL) \
	$(UTILITY_OBJECTS) \
	$(SCHEDULER_OBJECTS) ;\
	$(CXX) $(LDFLAGS) -o bin/$@ $^ $(LDLIBS)

test/url-parser: test/url-parser.o \
	$(COMMON_OBJECTS) \
	$(URL) \
	$(UTILITY_OBJECTS) \
	$(SCHEDULER_OBJECTS) ;\
	$(CXX) $(LDFLAGS) -o bin/$@ $^ $(LDLIBS)

test/url-tree: test/url-tree.o \
	$(COMMON_OBJECTS) \
	$(URL) \
	$(UTILITY_OBJECTS) \
	$(SCHEDULER_OBJECTS) ;\
	$(CXX) $(LDFLAGS) -o bin/$@ $^ $(LDLIBS)
 
test/url-scheduler: test/url-scheduler.o \
	$(COMMON_OBJECTS) \
	$(URL) \
	$(UTILITY_OBJECTS) \
	$(SCHEDULER_OBJECTS) ;\
	$(CXX) $(LDFLAGS) -o bin/$@ $^ $(LDLIBS)

# Phony targets
.PHONY: default all clean

# Make targets
default: all

all: $(TESTS)

clean:
	-rm -f */*.o */*.d
	-rm -f bin/*/*
