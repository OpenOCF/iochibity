import os, urllib2
from sys import platform as _platform
import subprocess
from string import maketrans
from SCons.Script import *

# Download from URL 'url', will save as 'target'
def download(target, url) :
	if os.path.exists(target) :
		return target
	try :
		print "Download %s from %s" % (target, url)
		print "Downloading ..."
		stream = urllib2.urlopen(url)
		file = open(target, 'wb')
		file.write(stream.read())
		file.close()
		print "... Download complete"
		return target
	except Exception, e :
		raise SCons.Errors.StopError( '%s [%s]' % (e, url) )

def feature_tests() :
        print "Running feature tests..."
        env = Environment()
        # if env.GetOption('clean'):
        #         print "We are cleaning, skip the config"
        #         return;

        conf = Configure(env)
        host_os = os.environ['IOTIVITY_HOST_OS']
        target_os = os.environ['IOTIVITY_TARGET_OS']
        print "IOTIVITY_HOST_OS:", host_os
        print "IOTIVITY_TARGET_OS:", target_os

        if not conf.CheckCXX():
                print('!! Your compiler and/or environment is not correctly configured.')
                Exit(0)

        if not conf.CheckFunc('printf'):
                print('!! Your compiler and/or environment is not correctly configured.')
                Exit(0)

        if not conf.CheckLib('gtest'):
                print "\tWARNING: googletest not found."
                print "\t\tTesting will be disabled."
                print "\t\tTo enable it, install googletest from https://github.com/google/googletest"
                print "\t\tand set GTEST_DIR (in source.me)."

        if conf.CheckLib('curl'):
                env.AppendUnique(HAVE_LIBCURL = 1)

        if conf.CheckLib('uuid'):
                env.AppendUnique(CPPDEFINES = ['HAVE_UUID'])

        if conf.CheckLib('pthread'):
                env.AppendUnique(PTHREAD_CFLAGS = ['-pthread'])
                env.AppendUnique(PTHREAD_LIBS = ['-lpthread'])
                env.AppendUnique(CPPDEFINES = ['HAVE_PTHREADS'])

        if conf.CheckProg('gdbus-codegen'):
                env.AppendUnique(CPPDEFINES = ['HAVE_GDBUS'])
                #WARNING: having gdbus-codegen does not guarantee bluez stack
                # it could be installed on e.g. os x by some lib other than bluez

        if conf.CheckProg('bluetoothd'):
                #TODO: make sure we have a bluez stack
                sys.stdout.write('Checking bluetooth version... ')
                sys.stdout.flush()
                #try
                bltv = subprocess.Popen(["bluetoothd", "-v"], stdout=subprocess.PIPE).communicate()[0]
                sys.stdout.write(bltv)
                bltv = bltv.replace('.', '')
                bldef = 'BLUEZ=' + bltv.rstrip()
                conf.env.PrependUnique(CPPDEFINES = [bldef])

        if conf.CheckFunc('clock_gettime'):
	        conf.env.AppendUnique(CPPDEFINES = ['HAVE_CLOCK_GETTIME'])

        if conf.CheckFunc('gettimeofday'):
	        conf.env.AppendUnique(CPPDEFINES = ['HAVE_GETTIMEOFDAY'])

        if conf.CheckFunc('strptime'):
	        conf.env.AppendUnique(CPPDEFINES = ['HAVE_STRPTIME'])

        if conf.CheckCHeader('arpa/inet.h'):
                conf.env.Append(CPPDEFINES = ['-DHAVE_ARPA_INET_H'])
        if conf.CheckCHeader('fcntl.h'):
                conf.env.Append(CPPDEFINES = ['-DHAVE_FCNTL_H'])
        if conf.CheckCHeader('grp.h'):
                conf.env.Append(CPPDEFINES = ['-DHAVE_GRP_H'])
        if conf.CheckCHeader('i6addr.h'):
                conf.env.Append(CPPDEFINES = ['-DHAVE_I6ADDR_H'])
        if conf.CheckCHeader('linux/limits.h'):
                conf.env.Append(CPPDEFINES = ['-DHAVE_LINUX_LIMITS_H'])
        if conf.CheckCHeader('memory.h'):
                conf.env.Append(CPPDEFINES = ['-DHAVE_MEMORY_H'])
        if conf.CheckCHeader('netdb.h'):
                conf.env.Append(CPPDEFINES = ['-DHAVE_NETDB_H'])
        if conf.CheckCHeader('netinet/in.h'):
                conf.env.Append(CPPDEFINES = ['-DHAVE_NETINET_IN_H'])
        if conf.CheckCHeader('pthread.h'):
                conf.env.Append(CPPDEFINES = ['-DHAVE_PTHREAD_H'])
        if conf.CheckCHeader('stdlib.h'):
                conf.env.Append(CPPDEFINES = ['-DHAVE_STDLIB_H'])
        if conf.CheckCHeader('string.h'):
                conf.env.Append(CPPDEFINES = ['-DHAVE_STRING_H'])
        if conf.CheckCHeader('strings.h'):
                conf.env.Append(CPPDEFINES = ['-DHAVE_STRINGS_H'])
        if conf.CheckCHeader('sys/socket.h'):
                conf.env.Append(CPPDEFINES = ['-DHAVE_SYS_SOCKET_H'])
        if conf.CheckCHeader('sys/stat.h'):
                conf.env.Append(CPPDEFINES = ['-DHAVE_SYS_STAT_H'])
        if conf.CheckCHeader('sys/time.h'):
                conf.env.Append(CPPDEFINES = ['-DHAVE_SYS_TIME_H'])
        if conf.CheckCHeader('sys/timeb.h'):
                conf.env.Append(CPPDEFINES = ['-DHAVE_SYS_TIMEB_H'])
        if conf.CheckCHeader('sys/types.h'):
                conf.env.Append(CPPDEFINES = ['-DHAVE_SYS_TYPES_H'])
        if conf.CheckCHeader('sys/unistd.h'):
                conf.env.Append(CPPDEFINES = ['-DHAVE_SYS_UNISTD_H'])
        if conf.CheckCHeader('syslog.h'):
                conf.env.Append(CPPDEFINES = ['-DHAVE_SYSLOG_H'])
        if conf.CheckCHeader('time.h'):
                conf.env.Append(CPPDEFINES = ['-DHAVE_TIME_H'])
        if conf.CheckCHeader('unistd.h'):
                conf.env.Append(CPPDEFINES = ['-DHAVE_UNISTD_H'])
        if conf.CheckCHeader('uuid/uuid.h'):
                conf.env.Append(CPPDEFINES = ['-DHAVE_UUID_UUID_H'])
        if conf.CheckCHeader('windows.h'):
                conf.env.Append(CPPDEFINES = ['-DHAVE_WINDOWS_H'])
        if conf.CheckCHeader('winsock2.h'):
                conf.env.Append(CPPDEFINES = ['-DHAVE_WINSOCK2_H'])
        if conf.CheckCHeader('ws2tcpip.h'):
                conf.env.Append(CPPDEFINES = ['-DHAVE_WS2TCPIP_H'])

        if conf.CheckFunc('GetSystemTimeAsFileTime') or target_os == 'windows':
	# TODO: Remove target_os check.
	# We currently check for 'windows' as well, because the environment can
	# sometimes get so polluted that CheckFunc ceases to work!
	        conf.env.AppendUnique(CPPDEFINES = ['HAVE_GETSYSTEMTIMEASFILETIME'])

        env = conf.Finish()
        print "... feature testing complete"
        return env



# if target_os in ['darwin','linux']:
# 	# Verify that 'google unit test' library is installed.  If not,
# 	# get it and install it
# 	SConscript(src_dir + '/extlibs/gtest/SConscript')

# 	# Verify that 'hippomocks' mocking code is installed.  If not,
# 	# get it and install it
# 	SConscript(src_dir + '/extlibs/hippomocks.scons')


# # elif target_os == 'darwin':
# # 	# Verify that 'google unit test' library is installed.  If not,
# # 	# get it and install it
# # 	SConscript(src_dir + '/extlibs/gtest/SConscript')

# # 	# Build C stack's unit tests.
# # 	SConscript('csdk/stack/test/SConscript')
# # 	SConscript('csdk/connectivity/test/SConscript')
