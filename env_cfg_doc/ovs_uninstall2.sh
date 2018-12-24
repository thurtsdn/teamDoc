cd ovs
make clean

export PATH=$PATH:/usr/local/share/openvswitch/scripts
ovs-ctl stop
ovs-dpctl del-dp ovs-system
rmmod  openvswitch
