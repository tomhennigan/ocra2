
shared = dylib
#debug = -DDEBUG

CC = gcc
CFLAGS = -I. $(debug) -O -g
#SHARED = -shared
SHARED = -dynamiclib

OBJS-libocra2 = libocra2.o
TARGET-libocra2 = libocra2.$(shared)

OBJS-main = main.o
TARGET-main = main

OBJS-cat = cat_bin.o
TARGET-cat = cat_bin

all:: $(TARGET-libocra2) $(TARGET-main) $(TARGET-cat)

$(TARGET-libocra2): $(OBJS-libocra2)
	$(CC) $(LDFLAGS) $(SHARED) $(OBJS-libocra2) -o $@
	
$(TARGET-cat): $(OBJS-cat)

$(TARGET-main): $(OBJS-main)
	$(CC) $(CFLAGS) -L. -I. -locra2 $< -o $@

clean::
	rm -f $(OBJS-libocra2) $(TARGET-libocra2)
	rm -f $(OBJS-cat) $(TARGET-cat)
	rm -f $(OBJS-main) $(TARGET-main)
