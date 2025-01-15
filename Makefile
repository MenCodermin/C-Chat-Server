# Compiler and Flags
CC = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
LDFLAGS_WIN = -lws2_32 -liphlpapi
LDFLAGS_LINUX = -lpthread

# Directories and Files
APPNAME = myapp
SRCDIR = src
OBJDIR = obj
SRC = $(wildcard $(SRCDIR)/*.cpp)
OBJ = $(SRC:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# Detect OS
ifeq ($(OS),Windows_NT)
    LDFLAGS = $(LDFLAGS_WIN)
    MKDIR = mkdir $(OBJDIR)
    RM = del /q /f
else
    LDFLAGS = $(LDFLAGS_LINUX)
    MKDIR = mkdir -p $(OBJDIR)
    RM = rm -rf
endif

# Build Rules
all: $(OBJDIR) $(APPNAME)

$(APPNAME): $(OBJ)
	@echo "Linking: $(CC) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)"
	$(CC) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJDIR):
	@echo "Creating directory: $(OBJDIR)"
	@if [ ! -d "$(OBJDIR)" ]; then $(MKDIR); fi

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@echo "Compiling: $(CC) $(CXXFLAGS) -o $@ -c $<"
	$(CC) $(CXXFLAGS) -o $@ -c $<

clean:
	rm -rf obj *.d *.exe
