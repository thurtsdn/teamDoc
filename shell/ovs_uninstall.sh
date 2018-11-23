#!/bin/bash

#ovs2.5.x uninstall

kill `cd /usr/local/var/run/openvswitch && cat ovsdb-server.pid ovs-vswitchd.pid`
cd ovs
make clean
