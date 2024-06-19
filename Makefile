# Makefile for iPhone2GNode

# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Werror -std=c++11 -pedantic-errors -ggdb
PKG_CONFIG = pkg-config
LIBS = `$(PKG_CONFIG) --libs fuse sqlite3`
CXXFLAGS += `$(PKG_CONFIG) --cflags fuse sqlite3`

# Project structure
SRCDIR = source
OBJDIR = obj
BINDIR = bin

# Targets and files
APP = $(BINDIR)/notesfs
TESTS = $(OBJDIR)/regex-replace.tts $(OBJDIR)/filter.tts
OBJS_TDD = $(OBJDIR)/regex-replace.o $(OBJDIR)/filter.o
OBJS_NO_TDD = $(OBJDIR)/notesfs.o
ALL_OBJS = $(OBJS_TDD) $(OBJS_NO_TDD)

# Phony targets
.PHONY: all clean force

# Default target
all: $(APP) $(TESTS)
    @echo "Build completed successfully."

# Clean target
clean:
    @echo "Cleaning..."
    @rm -rf $(OBJDIR)/*.o $(OBJDIR)/*.d $(OBJDIR)/*.tts $(BINDIR)/*

# Force target
force: clean all

# Dependency generation
-include $(ALL_OBJS:.o=.d)

# Linking executable
$(APP): $(ALL_OBJS)
    @echo "Linking $@..."
    @mkdir -p $(BINDIR)
    $(CXX) -o $@ $(OBJS_TDD) $(OBJS_NO_TDD) $(LIBS)

# Compiling source files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
    @echo "Compiling $<..."
    @mkdir -p $(OBJDIR)
    $(CXX) $(CXXFLAGS) -c -o $@ $<
    @$(CXX) $(CXXFLAGS) -MM -MT $@ -MP -MF $(OBJDIR)/$*.d $<

# Running test targets
$(OBJDIR)/%.tts: $(OBJDIR)/%.tt $(APP)
    @echo "Running test $<..."
    @timeout $(DEFAULT_TIMEOUT) $(VALGRIND) ./$(APP) --test $< && touch $@

# Placeholder for non-existent test targets
$(OBJDIR)/%.tt:
    @echo "Error: $@ not found! Please create one."
    @false
