###
#
#Determines the name of the executables. Change for each new version
VERSION = 42
EXTRACT_VERSION = _v1

EXE = aavsim$(VERSION)

# header files, used to make the build of objects dependent on these as well
HEADERS = src/*.h
# src and .o directories. Add to here when a new file has been added to CEPAC
OBJS = src/AavUtil.o	\
	src/DecisionTree.o	\
	src/Patient.o	\
	src/SimContext.o	\
	src/StateUpdater.o	\
	src/InitializePatient.o	\
	src/mtrand.o
GUI_OBJS = $(OBJS) src/GUI/main.o src/GUI/DisplayBox.o src/GUI/statusWidget.o src/GUI/cepac-icon.o
CONSOLE_OBJS = $(OBJS) src/ConsoleMain.o

#variables for use with gui build
LIBDIRNAME = /c/wxWidgets-3.1.1/lib/gcc_lib

# script executable build targets, uses perl2exe to package
SCRIPTS_EXES = upgrade_inputs$(VERSION).exe data_extraction$(EXTRACT_VERSION).exe
	
#compiler and related flags
CXX = g++
CXXFLAGS = -I. -w -std=c++17
LDFLAGS = -lm -static-libgcc -static-libstdc++ -static
# Alternate LDFLAGS for Mac OS. May also want to add Mac to version name
#LDFLAGS = -lm -static-libstdc++
# Alternate LDFlags for Linux. May also want to add Linux to version name
#LDFLAGS = -lm -static-libgcc 

WX_CXXFLAGS = -D__WXMSW__ -I/c/wxWidgets-3.1.1/lib/gcc_lib/mswu -I/c/wxWidgets-3.1.1/include -DDWX_PRECOMP -Wundef -Wno-ctor-dtor-privacy -fno-strict-aliasing
WX_LDFLAGS = -L$(LIBDIRNAME) -mwindows -lwxmsw31ud_core -lrpcrt4 -loleaut32 -lole32 -luuid -lwinspool -lwinmm -lshell32 -lcomctl32 -lcomdlg32 -lctl3d32 -ladvapi32 -lwsock32 -lgdi32
WX_CXXFLAGS_DEBUG = -I/usr/local/lib/wx/include/msw-unicode-static-3.1 -I/usr/local/include/wx-3.1 -D__WXDEBUG__ -D__WXMSW__
WX_LDFLAGS_DEBUG = -L/usr/local/lib -Wl,--subsystem,windows -mwindows /usr/local/lib/libwx_mswu_richtext-3.1.a /usr/local/lib/libwx_mswu_aui-3.1.a /usr/local/lib/libwx_mswu_xrc-3.1.a /usr/local/lib/libwx_mswu_qa-3.1.a /usr/local/lib/libwx_mswu_html-3.1.a /usr/local/lib/libwx_mswu_adv-3.1.a /usr/local/lib/libwx_mswu_core-3.1.a /usr/local/lib/libwx_baseu_xml-3.1.a /usr/local/lib/libwx_baseu_net-3.1.a /usr/local/lib/libwx_baseu-3.1.a -lwxregexu-3.1 -lwxexpat-3.1 -lwxtiff-3.1 -lwxjpeg-3.1 -lwxpng-3.1 -lwxzlib-3.1 -lrpcrt4 -loleaut32 -lole32 -luuid -luxtheme -lwinspool -lwinmm -lshell32 -lshlwapi -lcomctl32 -lcomdlg32 -lctl3d32 -ladvapi32 -lversion -lwsock32 -lgdi32 -loleacc

#rules for compiling C and CPP files
.SUFFIXES: .c .cpp

%.o : %.c $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@
	
%.o : %.rc
	windres $^ -o $@

%$(VERSION).exe %$(EXTRACT_VERSION).exe : scripts/%.pl
	perl2exe -o $@ $^  

##################
# These are the only args that should be called from the command line
##################

all : console

console : CXXFLAGS += -O3
console : $(CONSOLE_OBJS)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $(EXE)
	
console_debug : CXXFLAGS += -g -O0
console_debug : $(CONSOLE_OBJS)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $(EXE)

gui: CXXFLAGS += -g -O0 $(WX_CXXFLAGS_DEBUG)
gui: LDFLAGS += $(WX_LDFLAGS_DEBUG)
gui: $(GUI_OBJS)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $(EXE)
	
gui_debug : CXXFLAGS += -g -O0 $(WX_CXXFLAGS_DEBUG)
gui_debug : LDFLAGS += $(WX_LDFLAGS_DEBUG)
gui_debug : $(GUI_OBJS)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $(EXE)

scripts : $(SCRIPTS_EXES) 

clean : 
	rm -f src/*.o src/GUI/*.o
	rm -f $(EXE) $(EXE).exe $(SCRIPTS_EXES)