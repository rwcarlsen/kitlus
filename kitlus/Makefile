
##### set default var values #####
ifeq ($(strip $(CYCLUS_ROOT)),)
	CYCLUS_ROOT = $(HOME)/.local $(HOME)/cyc/install
endif
ifeq ($(strip $(PREFIX)),)
	PREFIX = $(HOME)/.local
endif

##### dependency config #####
includes += -I..
includes += $(CYCLUS_ROOT:%=-I%/include/cyclus)
includes += $(shell PKG_CONFIG_PATH=$HOME/.local pkg-config --cflags sqlite3)
includes += $(shell PKG_CONFIG_PATH=$HOME/.local pkg-config --cflags cbc)
includes += $(shell PKG_CONFIG_PATH=$HOME/.local pkg-config --cflags libxml-2.0)
includes += $(shell PKG_CONFIG_PATH=$HOME/.local pkg-config --cflags libxml++-2.6)
libs += $(CYCLUS_ROOT:%=-L%/lib)
libs += -lcycluscore

##### variable setup #####
headers = $(wildcard *.h)
source = $(wildcard *.cc)
objs = $(source:%.cc=%.o)

libname = libkitlus.so
postfix = kitlus
prefix = $(DESTDIR)/$(PREFIX)
ifeq ($(strip $(DESTDIR)),)
	prefix = $(PREFIX)
endif

##### primary targets #####
build: $(libname)

debug: CXXFLAGS += -g
debug: $(libname)

release: CXXFLAGS += -O3
release: $(libname)

install: release
	@mkdir -p $(prefix)/lib
	@mkdir -p $(prefix)/include/$(postfix)
	cp $(libname) $(prefix)/lib/
	cp $(headers) $(prefix)/include/$(postfix)/

$(libname) : $(objs)
	@echo "CXX -shared -fpic -rdynamic $(CXXFLAGS) $(CPPFLAGS) $(libs) $(objs) -o $@"
	@$(CXX) -shared -fpic -rdynamic $(CXXFLAGS) $(CPPFLAGS) $(libs) $^ -o $@

clean:
	rm -f *.o *.d *.so

##### pattern rules to build all object files from source #####

# pull in dependency info for *existing* .o files
-include $(objs:%.o=%.d)

%.o : %.cc %.h
	@echo "CXX -c -fpic $(CXXFLAGS) $(CPPFLAGS) $< -o $@"
	@$(CXX) -c -fpic $(CXXFLAGS) $(CPPFLAGS) $(includes) $< -o $@
	@$(CXX) -MM $(CXXFLAGS) $(includes) $< > $*.d

# disable makes built-in implicit rules
.SUFFIXES:
