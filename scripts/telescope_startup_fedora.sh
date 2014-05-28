#!/bin/bash
#
# Startup Fedora and RedHat script for TeleScope CQ.
#
# Author: Kirill Belyaev
# kirillbelyaev@yahoo.com
# kirill@cs.colostate.edu
# description: TeleScope CQ
# processname: telescope
# pidfile: /var/run/telescope.pid
# Source function library.      This creates the operating environment for the process to be started
. /etc/rc.d/init.d/functions


TeleScope=telescope
TeleScope_EXEC=/usr/local/bin/telescope
PIDFILE=/var/run/telescope.pid

case "$1" in
  start)
	if [ -f $PIDFILE ]
	then
		echo "$PIDFILE exists, $TeleScope is already running or crashed!"
	else
	        echo -n "Starting $TeleScope: "
        	daemon $TeleScope_EXEC
		echo
	        touch /var/lock/subsys/$TeleScope
	fi	
        ;;
  stop)

	if [ ! -f $PIDFILE ]
	then
		echo "$PIDFILE does not exist, $TeleScope is not running!"
	else
        	echo -n "Shutting down $TeleScope, please be patient: "
	        killproc $TeleScope_EXEC
	        echo
	        rm -f /var/lock/subsys/$TeleScope
	        rm -f $PIDFILE
		sleep 2
	fi	
        ;;
 *)
        echo "Usage: $0 {start|stop}"
        exit 1
esac

exit 0

