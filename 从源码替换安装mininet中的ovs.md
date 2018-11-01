mininet VM 安装

>  http://mininet.org/download/

卸载 mininet 中的自带 ovs 模块

> ~$ mininet/util/install.sh -r

下载 ovs 源码

> ~$ git clone https://github.com/openvswitch/ovs.git
> ~$ cd ovs
> ~/ovs$ git checkout v2.5.0

配置
> ~$ cd ovs
> ~/ovs$ ./boot.sh
> ~/ovs$ ./configure --with-linux=/lib/modules/`uname -r`/build    --enable-Werror

编译安装
> ~/ovs$ make

安装 ovs 内核模块
> ~/ovs$ make modules_install

检测 ovs 内核模块是否安装成功
> ~/ovs$ /sbin/modprobe openvswitch
>若没有任何报错则表示 ovs 内核模块安装成功

数据库配置
> ~$ mkdir -p /usr/local/etc/openvswitch
> ~$ ovsdb-tool create /usr/local/etc/openvswitch/conf.db vswitchd/vswitch.ovsschema
ovs2.5.x 源码安装教程

启动
> ~$ ovsdb-server --remote=punix:/usr/local/var/run/openvswitch/db.sock \
  ​               --remote=db:Open_vSwitch,Open_vSwitch,manager_options \
  ​               --private-key=db:Open_vSwitch,SSL,private_key \
  ​               --certificate=db:Open_vSwitch,SSL,certificate \
  ​               --bootstrap-ca-cert=db:Open_vSwitch,SSL,ca_cert \
  ​               --pidfile --detach

> ~$ ovs-vsctl --no-wait init
> ~$ ovs-vswitchd --pidfile --detach

ovs更新

kill ovs守护进程

> $ kill 'cd /usr/local/var/run/openvswitch && cat ovsdb-server.pid ovs-vswitchd.pid'

测试：

> ~$ ovs-vsctl --version 

附：
ovs 2.5.x 官方安装手册

> http://www.openvswitch.org/support/dist-docs-2.5/INSTALL.md.html


