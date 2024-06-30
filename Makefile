
CXXSTANDARD := c++23

CXX ?= c++

CXXFLAGS ?= -O2 -g

CPPFLAGS ?= 

RUSTC ?= rustc

RUST_EDITION := 2021

RUSTFLAGS ?= -C opt-level=2 -C debug-info=2

INCLUDE_PATH := include/

TESTS := option array

ALL_CXXFLAGS := $(CXXFLAGS) -std=$(CXXSTANDARD)

ALL_CPPFLAGS := $(CPPFLAGS) $(INCLUDE_PATH:%=-I %)


all: $(TESTS:%=tests/bin/%$(EXEEXT))

.PHONY: all test $(TESTS:%=run-%)

test: $(TESTS:%=run-%)

tests/bin:
	mkdir -p tests/bin

$(TESTS:%=tests/bin/%$(EXEEXT).d): tests/bin/%$(EXEEXT).d: tests/%.cxx tests/bin
	$(CXX) $(ALL_CPPFLAGS) $(ALL_CXXFLAGS) -MM -MF $@ $<

include $(TESTS:%=tests/bin/%$(EXEEXT).d)

$(TESTS:%=tests/bin/%$(EXEEXT)): tests/bin/%$(EXEEXT): tests/%.cxx
	$(CXX) $(ALL_CPPFLAGS) $(ALL_CXXFLAGS) -MMD -MF $@.d -o $@ $<


$(TESTS:%=run-%): run-%: tests/bin/%$(EXEEXT)
	@echo "Running test $<"
	@$^ && echo "Passed..." || echo "Failed..." 
