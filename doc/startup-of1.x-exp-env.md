现在只能分步骤启动
（直接在mininet里指定ryu控制器，启动会出错）

1. 启动wireshark  
sudo wireshark 

2. 启动ryu控制器  
启动官方代码里面的：ryu-manager --verbose   ryu.app.simple_switch_13  
启动自己写的：ryu-manager --verbose ./***.py  

3. 启动OpenFlow1.3的拓扑：  
sudo mn --switch ovs --controller remote  
sudo mn --topo single,3 --switch ovs --controller remote  

4. OVS协议配置  
设置协议版本  
sudo ovs-vsctl set Bridge s1 protocols=OpenFlow13  
查看流表  
sudo ovs-ofctl -O OpenFlow13 dump-flows s1

5. 运行send脚本  
需要先安装sendip  
sudo apt-get install sendip  
在mininet的环境里，执行: h1 bash ./TeamDoc/send.sh &

其他指令：


查看 OVS 的版本信息：
ovs-appctl –version

查看 OVS 支持的 OpenFlow 协议的版本：
ovs-ofctl –version
