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
%.pic.o:	%.cpp
		$(CXX) -fPIC -c $(CPPFLAGS) $(CXXFLAGS) -o $@ $<

%.o:		%.cpp
		$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -o $@ $<

%.s:		%.cpp
		$(CXX) -S $(CPPFLAGS) $(CXXFLAGS) -o $@ $<

# Phony targets
.PHONY: clean default all

# Compile object files
UTILITY_OBJECTS := $(patsubst %.cpp,%.o,$(wildcard utility/*.cpp))
TEST_OBJECTS := $(patsubst %.cpp,%.o,$(wildcard test/*.cpp))

# Binary targets
TESTS = test/html-parser \
	test/word-indexer

test/html-parser: test/html-parser.o $(UTILITY_OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

test/word-indexer: test/word-indexer.o $(UTILITY_OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

# Make targets/rules
default: all

all: $(TESTS)

clean:
	-rm -f $(TESTS) */*.o */*.d
