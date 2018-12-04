STRIP := strip
CMAKE := cmake
PYTHON := python3
MKDIR := mkdir
CD := cd

all: cffi

cffi: clib ./screenshot/api_build.py
	$(PYTHON) './screenshot/api_build.py'

clib:
	-$(MKDIR) './build'
	$(CD) './build' && $(CMAKE) '../ext' -DCMAKE_BUILD_TYPE=Release
	$(MAKE) -C './build'

clean:
	$(RM) -rf './build'
