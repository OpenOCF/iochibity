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
        home = os.environ['HOME']
        gtestpath = home + '/.iotivity.d/gtest-1.7.0/lib/.libs'
        print "gt:", gtestpath
        env.AppendUnique(LIBPATH = gtestpath)
        env_conf = Configure(env)
        target_os = env.get('TARGET_OS')

        if not env_conf.CheckLib('gtest'):
                print "gtest not found"
                exit(1)

        if not env_conf.CheckLib('gtest_main'):
                print "gtest_main not found"
                exit(1)

        if env_conf.CheckLib('uuid'):
                env.AppendUnique(CPPDEFINES = ['HAVE_UUID'])

        if env_conf.CheckLib('pthread'):
                env.AppendUnique(PTHREAD_CFLAGS = ['-pthread'])
                env.AppendUnique(PTHREAD_LIBS = ['-lpthread'])
                env.AppendUnique(CPPDEFINES = ['HAVE_PTHREADS'])

        if env_conf.CheckProg('gdbus-codegen'):
                env.AppendUnique(CPPDEFINES = ['HAVE_GDBUS'])
                #WARNING: having gdbus-codegen does not guarantee bluez stack
                # it could be installed on e.g. os x by some lib other than bluez

        if _platform in ['linux', 'linux2']:
                #TODO: make sure we have a bluez stack
                sys.stdout.write('Checking bluetooth version... ')
                sys.stdout.flush()
                #try
                bltv = subprocess.Popen(["bluetoothd", "-v"], stdout=subprocess.PIPE).communicate()[0]
                sys.stdout.write(bltv)
                bltv = bltv.replace('.', '')
                bldef = 'BLUEZ=' + bltv.rstrip()
                env_conf.env.PrependUnique(CPPDEFINES = [bldef])

        if env_conf.CheckFunc('clock_gettime'):
	        env_conf.env.AppendUnique(CPPDEFINES = ['HAVE_CLOCK_GETTIME'])

        if env_conf.CheckFunc('gettimeofday'):
	        env_conf.env.AppendUnique(CPPDEFINES = ['HAVE_GETTIMEOFDAY'])

        if env_conf.CheckFunc('strptime'):
	        env_conf.env.AppendUnique(CPPDEFINES = ['HAVE_STRPTIME'])

        if env_conf.CheckCHeader('arpa/inet.h'):
                env_conf.env.Append(CPPDEFINES = ['-DHAVE_ARPA_INET_H'])
        if env_conf.CheckCHeader('fcntl.h'):
                env_conf.env.Append(CPPDEFINES = ['-DHAVE_FCNTL_H'])
        if env_conf.CheckCHeader('grp.h'):
                env_conf.env.Append(CPPDEFINES = ['-DHAVE_GRP_H'])
        if env_conf.CheckCHeader('i6addr.h'):
                env_conf.env.Append(CPPDEFINES = ['-DHAVE_I6ADDR_H'])
        if env_conf.CheckCHeader('linux/limits.h'):
                env_conf.env.Append(CPPDEFINES = ['-DHAVE_LINUX_LIMITS_H'])
        if env_conf.CheckCHeader('memory.h'):
                env_conf.env.Append(CPPDEFINES = ['-DHAVE_MEMORY_H'])
        if env_conf.CheckCHeader('netdb.h'):
                env_conf.env.Append(CPPDEFINES = ['-DHAVE_NETDB_H'])
        if env_conf.CheckCHeader('netinet/in.h'):
                env_conf.env.Append(CPPDEFINES = ['-DHAVE_NETINET_IN_H'])
        if env_conf.CheckCHeader('pthread.h'):
                env_conf.env.Append(CPPDEFINES = ['-DHAVE_PTHREAD_H'])
        if env_conf.CheckCHeader('stdlib.h'):
                env_conf.env.Append(CPPDEFINES = ['-DHAVE_STDLIB_H'])
        if env_conf.CheckCHeader('string.h'):
                env_conf.env.Append(CPPDEFINES = ['-DHAVE_STRING_H'])
        if env_conf.CheckCHeader('strings.h'):
                env_conf.env.Append(CPPDEFINES = ['-DHAVE_STRINGS_H'])
        if env_conf.CheckCHeader('sys/socket.h'):
                env_conf.env.Append(CPPDEFINES = ['-DHAVE_SYS_SOCKET_H'])
        if env_conf.CheckCHeader('sys/stat.h'):
                env_conf.env.Append(CPPDEFINES = ['-DHAVE_SYS_STAT_H'])
        if env_conf.CheckCHeader('sys/time.h'):
                env_conf.env.Append(CPPDEFINES = ['-DHAVE_SYS_TIME_H'])
        if env_conf.CheckCHeader('sys/timeb.h'):
                env_conf.env.Append(CPPDEFINES = ['-DHAVE_SYS_TIMEB_H'])
        if env_conf.CheckCHeader('sys/types.h'):
                env_conf.env.Append(CPPDEFINES = ['-DHAVE_SYS_TYPES_H'])
        if env_conf.CheckCHeader('sys/unistd.h'):
                env_conf.env.Append(CPPDEFINES = ['-DHAVE_SYS_UNISTD_H'])
        if env_conf.CheckCHeader('syslog.h'):
                env_conf.env.Append(CPPDEFINES = ['-DHAVE_SYSLOG_H'])
        if env_conf.CheckCHeader('time.h'):
                env_conf.env.Append(CPPDEFINES = ['-DHAVE_TIME_H'])
        if env_conf.CheckCHeader('unistd.h'):
                env_conf.env.Append(CPPDEFINES = ['-DHAVE_UNISTD_H'])
        if env_conf.CheckCHeader('uuid/uuid.h'):
                env_conf.env.Append(CPPDEFINES = ['-DHAVE_UUID_UUID_H'])
        if env_conf.CheckCHeader('windows.h'):
                env_conf.env.Append(CPPDEFINES = ['-DHAVE_WINDOWS_H'])
        if env_conf.CheckCHeader('winsock2.h'):
                env_conf.env.Append(CPPDEFINES = ['-DHAVE_WINSOCK2_H'])
        if env_conf.CheckCHeader('ws2tcpip.h'):
                env_conf.env.Append(CPPDEFINES = ['-DHAVE_WS2TCPIP_H'])

        if env_conf.CheckFunc('GetSystemTimeAsFileTime') or target_os == 'windows':
	# TODO: Remove target_os check.
	# We currently check for 'windows' as well, because the environment can
	# sometimes get so polluted that CheckFunc ceases to work!
	        env_conf.env.AppendUnique(CPPDEFINES = ['HAVE_GETSYSTEMTIMEASFILETIME'])

        env = env_conf.Finish()
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

