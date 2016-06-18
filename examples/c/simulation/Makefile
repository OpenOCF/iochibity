IOTIVITY_HOME=$(HOME)/iotivity/iotivation/libiotivity

OS=darwin
ARCH=x86_64
PHASE=release

CC= gcc -std=c11
CFLAGS= -O2 -Wall -Wextra $(SYSCFLAGS) $(MYCFLAGS)
LDFLAGS= $(SYSLDFLAGS) $(MYLDFLAGS)
LIBS= -lm $(SYSLIBS) $(MYLIBS)

AR= ar rcu
RANLIB= ranlib
RM= rm -f

SYSCFLAGS=-I$(IOTIVITY_HOME)/resource/csdk/stack/include -I$(IOTIVITY_HOME)/resource/c_common -I$(IOTIVITY_HOME)/resource/csdk/logger/include
SYSLDFLAGS="-L$(IOTIVITY_HOME)/out/$(OS)/$(ARCH)/$(PHASE)"
SYSLIBS="-loc"

MYCFLAGS=
MYLDFLAGS=
MYLIBS=
MYOBJS=

# == END OF USER SETTINGS -- NO NEED TO CHANGE ANYTHING BELOW THIS LINE =======

PLATS= aix bsd c89 freebsd generic linux macosx mingw posix solaris

CORE_O=	server.o
LIB_O=
BASE_O= $(CORE_O) $(LIB_O) $(MYOBJS)

LUA_T=	server
LUA_O=	server.o

ALL_O= $(BASE_O) $(LUA_O)
ALL_T= $(LUA_A) $(LUA_T)
ALL_A= $(LUA_A)

# Targets start here.
# default: $(PLAT)

all:	$(ALL_T)

o:	$(ALL_O)

a:	$(ALL_A)

$(LUA_A): $(BASE_O)
	$(AR) $@ $(BASE_O)
	$(RANLIB) $@

$(LUA_T): $(LUA_O) $(LUA_A)
	$(CC) -o $@ $(LDFLAGS) $(LUA_O) $(LUA_A) $(LIBS)

$(LUAC_T): $(LUAC_O) $(LUA_A)
	$(CC) -o $@ $(LDFLAGS) $(LUAC_O) $(LUA_A) $(LIBS)

clean:
	$(RM) $(ALL_T) $(ALL_O)

depend:
	@$(CC) $(CFLAGS) -MM l*.c

echo:
	@echo "OS= $(OS)"
	@echo "ARCH= $(ARCH)"
	@echo "CC= $(CC)"
	@echo "CFLAGS= $(CFLAGS)"
	@echo "LDFLAGS= $(SYSLDFLAGS)"
	@echo "LIBS= $(LIBS)"
	@echo "AR= $(AR)"
	@echo "RANLIB= $(RANLIB)"
	@echo "RM= $(RM)"

# Convenience targets for popular platforms
ALL= all

none:
	@echo "Please do 'make PLATFORM' where PLATFORM is one of these:"
	@echo "   $(PLATS)"

generic: $(ALL)

# list targets that do not create files (but not all makes understand .PHONY)
.PHONY: all $(PLATS) default o a clean depend echo none

# DO NOT DELETE

server.o: server.c

# (end of Makefile)
