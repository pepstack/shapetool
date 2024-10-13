# @file Makefile
#   Makefile both for mingw on windows
#
# @copyright Copyright(c) 2024, mapaware.top
# @since 2024-09-29 10:00:10
# @date 2024-10-14 01:36:38
########################################################################
# Linux, CYGWIN_NT, MSYS_NT, ...
shuname="$(shell uname)"
OSARCH=$(shell echo $(shuname)|awk -F '-' '{ print $$1 }')

# Note: cygwin is not supported! use mingw instead
ifeq ($(OSARCH), CYGWIN_NT)
	OSARCH=CYGWIN64
	$(error cygwin is not supported! use mingw instead)
endif

# mingw
ifeq ($(OSARCH), MINGW64_NT)
	OSARCH=MINGW64
else ifeq ($(OSARCH), MSYS_NT)
	OSARCH=MINGW64
else ifeq ($(OSARCH), MINGW32_NT)
	OSARCH=MINGW32
	$(error 32-bit mingw is not supported)
else
	OSARCH=LINUX64
endif


# compiler
CC=gcc

# compile directives
CFLAGS += -std=gnu11 -D_GNU_SOURCE -fPIC -Wall -Wno-unused-function -Wno-unused-variable

# load libs: -lpthread = libpthread.so
LDFLAGS += -lm -lpthread


# 主程序名
APPNAME := shapetool

# default build is release
# make BUILD=DEBUG
BUILD ?= RELEASE

# project source dirs
PREFIX := .
SRC_DIR := $(PREFIX)/source
DEPS_DIR := $(PREFIX)/deps

COMMON_DIR := $(SRC_DIR)/common

# depends libs
LIB_SHAPEFILE_DIR := $(DEPS_DIR)/shapefile


# include dirs for header files (*.h)
INCDIRS += -I$(SRC_DIR) \
	-I$(COMMON_DIR) \
	-I$(LIB_SHAPEFILE_DIR)/source
#

# Set all dirs for C source: './source/a ./source/b'
ALLCDIRS += $(SRC_DIR) \
	$(COMMON_DIR)
#

# Get pathfiles for C source files: './source/a/1.c ./source/b/2.c'
CSRCS := $(foreach cdir, $(ALLCDIRS), $(wildcard $(cdir)/*.c))

# Get names of object files: '1.o 2.o'
COBJS = $(patsubst %.c, %.o, $(notdir $(CSRCS)))


ifeq ($(OSARCH), MINGW64)
	CFLAGS += -D__MINGW64__ -m64
	INCDIRS += -I/mingw64/include
	LDFLAGS += -L$(LIB_SHAPEFILE_DIR) -L/mingw64/lib -lws2_32 -lcairo -lshapefile
else ifeq ($(OSARCH), LINUX64)
	CFLAGS += -D__LINUX__
    LDFLAGS +=  -L$(LIB_SHAPEFILE_DIR) -L/usr/local/lib -lrt -lshapefile
else
	$(error $(OSARCH) is not supported)
endif

ifeq ($(BUILD), DEBUG)
    # make BUILD=DEBUG
	CFLAGS += -D_DEBUG -g
else
    # default is release
	CFLAGS += -DNDEBUG -O3
endif

############################### make target ############################
.PHONY: all clean help revise


all: $(APPNAME)


# http://www.gnu.org/software/make/manual/make.html#Eval-Function
define COBJS_template =
$(basename $(notdir $(1))).o: $(1)
	$(CC) $(CFLAGS) -c $(1) $(INCDIRS) -o $(basename $(notdir $(1))).o
endef

$(foreach source,$(CSRCS),$(eval $(call COBJS_template,$(source))))
################################################################

$(APPNAME): $(COBJS)
	@echo "Build $(BUILD) app..."
	$(CC) $(CFLAGS) \
		-Wl,--rpath='/usr/local/lib:$(PREFIX):$(PREFIX)/libs' \
		-o $@ \
		$^ \
		$(LDFLAGS)
	@echo "Done."


clean:
	@$(PREFIX)/clean.sh $(APPNAME)


revise:
	@/usr/bin/find . \( -path ./deps -o -path ./.vscode \) -prune -o -type f -mtime -30 \( -name '*.h' -o -name '*.c' \) | xargs -I {} sh -c "sh revise-source.sh {}"
	@/usr/bin/find . \( -path ./deps -o -path ./.vscode \) -prune -o -type f -mtime -30 \( -name '*.hxx' -o -name '*.cxx' \) | xargs -I {} sh -c "sh revise-source.sh {}"
	@/usr/bin/find . \( -path ./deps -o -path ./.vscode \) -prune -o -type f -mtime -30 \( -name '*.hpp' -o -name '*.cpp' \) | xargs -I {} sh -c "sh revise-source.sh {}"
	@/usr/bin/find . \( -path ./deps -o -path ./.vscode \) -prune -o -type f -mtime -30 \( -name '*.java' -o -name '*.py' \) | xargs -I {} sh -c "sh revise-source.sh {}"
	@/usr/bin/find . \( -path ./deps -o -path ./.vscode \) -prune -o -type f -mtime -30 -name '*.sh' | xargs -I {} sh -c "sh revise-source.sh {}"
	@/usr/bin/find . \( -path ./deps -o -path ./.vscode \) -prune -o -type f -mtime -30 -name 'Makefile' | xargs -I {} sh -c "sh revise-source.sh {}"
	@echo "(Ok) revise source files done."


help:
	@echo
	@echo "Build project on $(OSARCH) with the following commands:"
	@echo "  make BUILD=DEBUG  # build app for debug $(APPNAME)"
	@echo "  make clean        # clean all temp files"
	@echo "  make              # build app for release (default)"
	@echo
