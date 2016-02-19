###############
## Makefile
###############

BINARY=../bin/appRapidChannel
SHARED_LIB=../lib/libRapidChannel.so
BOOST_BASE=${BOOST_INSTALL_ROOT}
APP_SETTING_BASE=${APP_SETTING_ROOT}
TINYXML_BASE=${TINYXML_ROOT}
APP_LOGGER_BASE=${APP_LOGGER_ROOT}

ARCH = $(shell uname -m)	# ARCH becomes the output from the uname -m command.
CXX=/usr/bin/g++
CXXFLAGS=-g -O2 -std=c++0x -Wall $(INCS)

SRCS = \
	TcpClientTransport.cpp \
	SeqNoMemoryMap.cpp \

MAIN_SRC = Main.cpp

OBJS = $(patsubst %.cpp, %.o, $(SRCS))
MAIN_OBJ = $(patsubst %.cpp, %.o, $(MAIN_SRC))

INCS = -I../include \
       -I$(BOOST_BASE)/include \
       -I$(APP_SETTING_BASE)/include \
       -I$(TINYXML_BASE)/include \
       -I$(APP_LOGGER_BASE)/include \

LIBS = \
        -L$(BOOST_BASE)/lib -lboost_thread \
        -L$(BOOST_BASE)/lib -lboost_system \
        -L$(BOOST_BASE)/lib -lboost_iostreams \
        -L$(APP_SETTING_BASE)/lib -lappSetting \
        -L$(TINYXML_BASE)/lib -ltinyXml \
        -L$(APP_LOGGER_BASE)/lib -lappLogger \
		-lpthread

all: $(BINARY)
	
$(BINARY): $(OBJS) $(MAIN_OBJ)
	$(CXX) -o $(BINARY) $(OBJS) $(MAIN_OBJ) $(LIBS)

$(SHARED_LIB): $(OBJS)
	$(CXX) -shared $^ -o $@ $(LIBS)

createlib: $(SHARED_LIB)
	
run:
	@echo "----------------------------"
	@echo "Running the programme. "
	@echo "----------------------------"
	@echo "----------------------------"
	$(BINARY)
	@echo "----------------------------"

clean:
	rm -rf $(BINARY) $(SHARED_LIB) *.o *.log
