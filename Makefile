CC      = gcc
CFLAGS  = -Wall -Wextra -O3

TARGET  = fshare
SRCS    = $(wildcard *.c)
OBJS    = $(SRCS:%.c=bin/%.o)

PREFIX  = /usr/local
BINDIR  = bin
INSTALL_BINDIR = $(PREFIX)/bin

all: $(BINDIR)/$(TARGET)

# Final binary (link step)
$(BINDIR)/$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile .c -> .o into bin/
bin/%.o: %.c | $(BINDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Ensure bin/ exists
$(BINDIR):
	mkdir -p $(BINDIR)

install: $(BINDIR)/$(TARGET)
	install -d $(INSTALL_BINDIR)
	install -m 755 $(BINDIR)/$(TARGET) $(INSTALL_BINDIR)/$(TARGET)

uninstall:
	rm -f $(INSTALL_BINDIR)/$(TARGET)

clean:
	rm -rf $(BINDIR)

.PHONY: all install uninstall clean
