# This file is part of ExtractEDGAR_XBRL.

# ExtractEDGAR_XBRL is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# ExtractEDGAR_XBRL is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with ExtractEDGAR_XBRL.  If not, see <http://www.gnu.org/licenses/>.

# see link below for make file dependency magic
#
# http://bruno.defraine.net/techtips/makefile-auto-dependencies-with-gcc/
#
MAKE=gmake

BOOSTDIR := /extra/boost/boost-1.68_gcc-8
GCCDIR := /extra/gcc/gcc-8
GTESTDIR := /usr/local/include
CPP := $(GCCDIR)/bin/g++

# If no configuration is specified, "Debug" will be used
ifndef "CFG"
	CFG := Debug
endif

#	common definitions

OUTFILE := Unit_Test

CFG_INC := -I/home/dpriedel/projects/cpp_like_py \
		-I$(GTESTDIR) -I$(BOOSTDIR) 

RPATH_LIB := -Wl,-rpath,$(GCCDIR)/lib64 -Wl,-rpath,$(BOOSTDIR)/lib -Wl,-rpath,/usr/local/lib

SDIR1 := .
SRCS1 := $(SDIR1)/test2.cpp


SRCS := $(SRCS1)

VPATH := $(SDIR1)

#
# Configuration: Debug
#
ifeq "$(CFG)" "Debug"

OUTDIR := Debug_unit

CFG_LIB := -lpthread -L$(BOOSTDIR)/lib \
		-L/usr/local/lib \
		-lgmock -lgmock_main -lgtest 

OBJS1=$(addprefix $(OUTDIR)/, $(addsuffix .o, $(basename $(notdir $(SRCS1)))))

OBJS=$(OBJS1)
DEPS=$(OBJS:.o=.d)

COMPILE=$(CPP) -c  -x c++  -O0  -g3 -std=c++17 -D_DEBUG -fPIC -o $@ $(CFG_INC) $< -march=native -MMD -MP
LINK := $(CPP)  -g -o $(OUTFILE) $(OBJS) $(CFG_LIB) -Wl,-E $(RPATH_LIB)

endif #	DEBUG configuration

# Build rules
all: $(OUTFILE)

$(OUTDIR)/%.o : %.cpp
	$(COMPILE)

$(OUTFILE): $(OUTDIR) $(OBJS1)
	$(LINK)

-include $(DEPS)

$(OUTDIR):
	mkdir -p "$(OUTDIR)"

# Rebuild this project
rebuild: cleanall all

# Clean this project
clean:
	rm -f $(OUTFILE)
	rm -f $(OBJS)
	rm -f $(OUTDIR)/*.P
	rm -f $(OUTDIR)/*.d
	rm -f $(OUTDIR)/*.o

# Clean this project and all dependencies
cleanall: clean
