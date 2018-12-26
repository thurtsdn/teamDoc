本文档用于指导从源码安装wireshark。

thurtsdn/wireshark中的wireshark-2.6.5-tt-ext分支是添加了对TT扩展支持的版本，在实验环境中需要安装该版本，以正确识别扩展帧字段。

### 1. 首先准备源码

下载源码：
git clone https://github.com/thurtsdn/wireshark.git

切换分支：
git checkout wireshark-2.6.5-tt-ext

### 2. 安装必要组件

视自己开发环境的情况，可能需要安装以下组件：

- GLib-2.0
- Qt (> 4.8)
- cmake

### 3. 创建目录&编译安装

进入wireshark源码目录：  
- cd wireshark

创建编译目录并进入：  
- mkdir build
- cd build

编译安装：
- sudo cmake ..
- sudo make
- sudo make install

