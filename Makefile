BIN_FILES  = request linked_list libclaves cliente servidor

CC = gcc

CPPFLAGS = -I$(INSTALL_PATH)/include -Wall

LDFLAGS = -L$(INSTALL_PATH)/lib/
LDLIBS = -lpthread -lrt -lz


all: $(BIN_FILES)
.PHONY : all

request: request.h
	$(CC) $(CFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

linked_list: linked_list.h
	$(CC) $(CFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

libclaves.so: claves.c request.h
	$(CC) $(CFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@


cliente: cliente.o libclaves.so
	$(CC) $(CFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@


servidor: servidor.o linked_list.h request.h
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $<

clean:
	rm -f $(BIN_FILES) *.o

.SUFFIXES:
.PHONY : clean
