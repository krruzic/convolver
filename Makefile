# project name (generate executable with this name)
TARGET   = convolve
CC       = gcc
# compiling flags here
CFLAGS   = -g -Wall -I.
LINKER   = gcc -g -O2
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
	$(LINKER) $@ $(OBJECTS) $(LFLAGS)
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
