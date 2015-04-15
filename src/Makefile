roperscientific-objs =  RoperScientificRoiCtrlObj.o \
			RoperScientificSyncCtrlObj.o \
			RoperScientificBinCtrlObj.o \
			RoperScientificDetInfoCtrlObj.o \
			RoperScientificInterface.o \
			RoperScientificCamera.o

SRCS = $(roperscientific-objs:.o=.cpp) 

CXXFLAGS += -I../include -I../../../hardware/include -I../../../common/include \
	-I../sdk/linux/include \
	-pthread -fPIC -g 

all:	RoperScientific.o

RoperScientific.o:	$(roperscientific-objs)
	$(LD) -o $@ -r $+

clean:
	rm -f *.o *.P

%.o : %.cpp
	$(COMPILE.cpp) -MD $(CXXFLAGS) -o $@ $<
	@cp $*.d $*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d

-include $(SRCS:.cpp=.P)

.PHONY: check-syntax
check-syntax:
	$(CXX) -Wall -Wextra -fsyntax-only $(CXXFLAGS) $(CHK_SOURCES)
