2016-07-05  Gregg Reynolds  <gar@Sparky.lan>

	* move documentation from root dir to doc/

	* move shell scripts from root dir to bin/

	* migrate all sample code out of iotivity repo and into separate c/cxx sdks:
	   https://github.com/iotk/iotivity-c
	   https://github.com/iotk/iotivity-cxx

	* various other build cleanups

2016-06-04  Gregg Reynolds  <reynolds-gregg@norc.org>

	* SConstruct: set flags -Wall, -std=c11, -std=c++11 in root
      SConstruct, remove from all other SConscript files

