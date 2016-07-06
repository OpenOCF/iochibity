2016-07-05  Gregg Reynolds  <reynolds-gregg@norc.org>

	* remove extlibs/android. obtaining and installing sdk/ndk is user
	responsibility.  anyway android support has been migrated to
	external sdk project at https://github.com/iotk/iotivity-android

	* move documentation from root dir to doc/

	* move shell scripts from root dir to bin/

	* migrate all sample code out of iotivity repo and into separate c/cxx sdks:
	   https://github.com/iotk/iotivity-c
	   https://github.com/iotk/iotivity-cxx

	* various other build cleanups

2016-07-03  Gregg Reynolds  <reynolds-gregg@norc.org>

	* remove gtest - treat it as ordinary dependency that user is
	expected to obtain and install. build scripts now only include
	-lgtest -lgtest-main, which are expected to be in libpath, we
	feature test for them like any other lib.

	* remove hippomocks - as for gtest, user can easily obtain and
	install, we feature test for the header

2016-06-04  Gregg Reynolds  <reynolds-gregg@norc.org>

	* SConstruct: set flags -Wall, -std=c11, -std=c++11 in root
      SConstruct, remove from all other SConscript files

