buildDir=build
#buildDir=build_debug

#change to Debug for debug mode
buildType=Release
#buildType=Debug

all: build

mkbuilddir:
	[ -d $(buildDir) ] || mkdir $(buildDir)

cmake: mkbuilddir CMakeLists.txt
	cd $(buildDir) && cmake -DCMAKE_BUILD_TYPE=$(buildType) ..

build: cmake
	$(MAKE) -C $(buildDir)

clean:
	$(MAKE) -C $(buildDir) clean
	
cleanup_cache:
	cd $(buildDir) && rm -rf *
	
run: all
	cd bin && ./vision
	
runClient: all
	cd bin && ./client
	
runGraphicalClient: all
	cd bin && ./graphicalClient
