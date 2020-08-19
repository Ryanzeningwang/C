INCDIR = ./include
SRCDIR = ./source

CC = clang++
CFLAGS = -Wall -g -I$(INCDIR)

PROG = object/obj_test
HDRS = $(INCDIR)/ObjectManager.h $(INCDIR)/Object.h
SRCS = $(SRCDIR)/main0.c $(SRCDIR)/ObjectManager.c

# object files have the same name as source files
OBJS = object/ObjectManager.o object/main.o 

# the program file depends on all object files
$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(PROG)
  
# build each object file
object/ObjectManager.o: $(SRCDIR)/ObjectManager.c $(HDRS)
	$(CC) $(CFLAGS) -c $(SRCDIR)/ObjectManager.c -o object/ObjectManager.o

object/main.o: $(SRCDIR)/main0.c  $(HDRS)
	$(CC) $(CFLAGS) -c $(SRCDIR)/main0.c -o object/main.o
  
clean:
	rm -f $(PROG) $(OBJS)
