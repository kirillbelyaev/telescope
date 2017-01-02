#!/bin/bash
#
# Shutdown script for TeleScope CQ.
#
# Author: Kirill Belyaev
# kirillbelyaev@yahoo.com
# kirill@cs.colostate.edu
# description: TeleScope CQ
# processname: telescope
# pidfile: /var/run/telescope.pid
# Source function library.      This creates the operating environment for the process to be started
#. /etc/rc.d/init.d/functions

echo -n "Shutting down TeleScope CQ, please be patient: "
exec `ps -C telescope | grep -F "telescope" | awk '{ print "kill -HUP "  $1 }' `
exit 0
