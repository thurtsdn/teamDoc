#!/bin/bash

#ovs2.5.x uninstall

kill `cd /usr/local/var/run/openvswitch && cat ovsdb-server.pid ovs-vswitchd.pid`
rmmod openvswitch
rm -rf /usr/local/etc/openvswitch
cd ovs
make clean
