本文档用于指导搭建整套实验环境。

## 1. 自行安装虚拟机

1. 安装Ubuntu 14.04/16.04虚拟机

2. 更换源，提高更新速度  
- https://www.cnblogs.com/littlemonsters/p/5783690.html

3. 更换内核版本至4.2.0-27-generic
- https://github.com/TT-openvswitch/TeamDoc/blob/master/Downgrade-kernel.md

4. 安装vm-tools

5. 安装mininet
- https://blog.csdn.net/asneverbefore/article/details/78916645

6. 安装Git

7. 安装wireshark

> 如果使用mininet官方的虚拟机镜像，则以上步骤可省略

## 2. 配置Git&安装环境

1. clone自己fork的ovs repo

2. 在ovs repo中添加两个remote源，一个是TT-openvswitch的，另一个是官方的

3. 基于TT-openvswitch/ovs的tt-ovs-2.5分支，创建本地的tt-ovs-2.5分支

4. 安装ovs，使用ovs_install.sh安装即可

5. clone自己fork的ryu repo

6. 在ryu repo中添加两个remote源，一个是TT-openvswitch的，另一个是官方的

7. 基于TT-openvswitch/ryu的tt-exp分支，创建本地的tt-exp分支

8. 安装ryu

- 先安装pip：sudo apt install python-pip
- 安装ryu：cd ryu; pip install .
- 安装完成回会提醒让升级pip
  - sudo pip install --upgrade pip
  - 这里会出现问题，安装失败，尝试sudo pip install --upgrade --ignore-installed pip
  - 如果还是失败，参考 https://stackoverflow.com/questions/49940813/pip-no-module-named-internal/50639340

9. 创建自己branch，写完代码，编译通过，实验没有问题，就向TT-openvswitch提交pull request
