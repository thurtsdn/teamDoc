udo apt-get update
>
> $ sudo apt-get install linux-image-extra-4.2.0-27-generic
>
> $ sudo apt-get install linux-image-4.2.0-27-generic

检查是否安装成功

> $ dpkg -l | grep 4.2.0-27-generic

配置 grub 文件

> $ sudo vim /etc/default/grub

将 `GRUB_DEFAULT=0` 改为

> GRUB_DEFAULT="Advanced options for Ubuntu>Ubuntu, with Linux 4.2.0-27-generic"

保存退出，更新 Grub 引导

> $ sudo update-grub

重启系统

> $ reboot

检查新内核是否安装成功

> $ uname -r

安装对应的 headers

> $ apt-get install   linux-headers-$(uanme -r)
