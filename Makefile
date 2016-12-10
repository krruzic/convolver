# project name (generate executable with this name)
TARGET   = convolve.out
CC       = gcc
# compiling flags here
CFLAGS   = -Wall -I.
LINKER   = gcc -o
# linking flags here
LFLAGS   = -Wall -I. -lm

# change these to proper directories where each file should be
SRCDIR   = src
BINDIR   = bin

SOURCES  := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(BINDIR)/%.o)

# link to binary
$(BINDIR)/$(TARGET): $(OBJECTS)
	@$(LINKER) $@ $(LFLAGS) $(OBJECTS)
	@echo "Make successful!"

# compile src files to objects
$(OBJECTS): $(BINDIR)/%.o : $(SRCDIR)/%.c | dir
	@$(CC) $(CFLAGS) -c $< -o $@
	
dir: 
	@mkdir -p "bin" 

.PHONY: clean
clean:
	rm -f $(OBJECTS)
	rm -f $(BINDIR)/$(TARGET)
	@echo "Project cleaned!"
