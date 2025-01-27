#!/bin/sh
#
# Sample watchdog routines, to invoke a specified shell function
# (which in this sample sends a SIGTERM to the invoking process)
# if the invoking shell script process takes longer than the specified
# time in the section of shell code between the begin_watchdog() and
# end_watchdog() calls.
#
# Paul Jackson
# pj@usa.net
# 3 Nov 2012

# token = begin_watchdog(secs, func)
#
# Call function 'func' in 'secs' seconds unless
# end_watchdog() called first with 'token'

begin_watchdog()
{
	secs=$1
	func=$2
	dir=$(mktemp -d)
	(
		trap 'rmdir $dir' 1 2 3 15
		sleep $secs
		test -d $dir && $func
	) 1>/dev/null &

	# the returned token is just the temp directory path
	echo $dir
}

# end_watchdog(token)
#
# Call off the watchdog - rmdir the token directory

end_watchdog()
{
	dir=$1
	rmdir $dir
}

woof()
{
	echo Woof - Killing process group $$ 1>&2
	kill -TERM -$$ 1>&2
}

# Call woof() if find pipe takes longer than 7 seconds

token=$(begin_watchdog 7 woof)

# To test the above watchdog routines, uncomment one of following
# two find commands.
#
# The one with maxdepth 2 is presumed to finish in under 7 seconds.
# The one with maxdepth 5 is presumed to finish in over 7 seconds.
#
# This will test both possible outcomes for the watchdog routines.
# If these numbers don't work for your system ... change them :)

( find / -maxdepth 2 -xdev -type f -print0 | xargs -0 md5sum | wc ) 2>&-
# ( find / -maxdepth 5 -xdev -type f -print0 | xargs -0 md5sum | wc ) 2>&-

# Call off the dogs

end_watchdog $token

echo watchdog guarded find pipe completed before dog barked
echo hanging around another 7 secs to prove watchdog disabled
sleep 7
echo good - still here - watchdog didnt bark

