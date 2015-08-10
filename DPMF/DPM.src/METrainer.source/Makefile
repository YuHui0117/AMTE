CXX		 = g++
CXXFLAGS = -Wall -I../include -Wno-deprecated -Wno-unused -O2
LINKFLAGS= -Wall -lpthread -O2
LIBFLAGS = -lz
LIBDIR   = ../lib


DIRS	 = src

MAKEFLAG = CXX="$(CXX)" CXXFLAGS="$(CXXFLAGS)" LINKFLAGS="$(LINKFLAGS)" LIBFLAGS="$(LIBFLAGS)" LIBDIR="$(LIBDIR)"

all:
	@echo ""
	@echo ""
	@echo "======================================== 开始编译 ========================================"
	@for x in $(DIRS); \
	do \
		echo "      进入目录: $$x"; \
		(cd $$x; make $(MAKEFLAG) all ; cd .. ); \
		echo "      返回目录: $$x"; \
		echo " "; \
	done
	@echo "======================================== 编译结束 ========================================"

clean:
	@echo "======================================== 清理文件 ========================================"
	-rm -f *.o
	@for x in $(DIRS); \
	do \
		echo "      进入目录: $$x"; \
		(cd $$x; make clean ; cd .. ); \
		echo "      返回目录: $$x"; \
		echo " "; \
	done
	@echo "======================================== 清理结束 ========================================"

