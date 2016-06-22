import os, urllib2

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
        env_conf = Configure(env)
        env_conf.CheckLib('uuid')
        if env_conf.CheckProg('gdbus-codegen'):
	        env_conf.env.AppendUnique(WITH_GDBUS = 1)
        if env_conf.CheckFunc('clock_gettime'):
	        env_conf.env.AppendUnique(CPPDEFINES = ['HAVE_CLOCK_GETTIME'])
        if env_conf.CheckFunc('gettimeofday'):
	        env_conf.env.AppendUnique(CPPDEFINES = ['HAVE_GETTIMEOFDAY'])
        if env_conf.CheckFunc('GetSystemTimeAsFileTime') or target_os == 'windows':
	# TODO: Remove target_os check.
	# We currently check for 'windows' as well, because the environment can
	# sometimes get so polluted that CheckFunc ceases to work!
	        env_conf.env.AppendUnique(CPPDEFINES = ['HAVE_GETSYSTEMTIMEASFILETIME'])
        env = env_conf.Finish()
        print "... feature testing complete"
