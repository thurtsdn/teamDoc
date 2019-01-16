#!/bin/bash

#ovs2.5.x install & uninstall

if [ "$1" == "-h" ]; then
    echo -e "Openvswitch install&uninstall shell helps."
    echo -e "\t -r: Only uninstall openvswtich. Otherwise, reinstall the openvswitch."
    exit
fi

export PATH=$PATH:/usr/local/share/openvswitch/scripts                                        
ovs-ctl stop
ovs-dpctl del-dp ovs-system
rmmod openvswitch
kil `cd /usr/local/var/run/openvswitch && cat ovsdb-server.pid ovs-vswitchd.pid`
rm -rf /usr/local/etc/openvswitch
cd ovs 
make clean

if [ "$1" == "-r" ]; then
    exit
fi

./boot.sh
./configure --with-linux=/lib/modules/`uname -r`/build   --enable-Werror
make
make install
make modules_install
/sbin/modprobe openvswitch
mkdir -p /usr/local/etc/openvswitch
ovsdb-tool create /usr/local/etc/openvswitch/conf.db vswitchd/vswitch.ovsschema
ovsdb-server --remote=punix:/usr/local/var/run/openvswitch/db.sock \
                 --remote=db:Open_vSwitch,Open_vSwitch,manager_options \
                 --private-key=db:Open_vSwitch,SSL,private_key \
                 --certificate=db:Open_vSwitch,SSL,certificate \
                 --bootstrap-ca-cert=db:Open_vSwitch,SSL,ca_cert \
                 --pidfile --detach
ovs-vsctl --no-wait init
ovs-vswitchd --pidfile --detach

