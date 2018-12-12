本文档用于指导启动在mininet中使用Ryu和ovs进行tt扩展实验的环境。

## 启动环境步骤

### 1. 启动并配置wireshark  
1. 首先打开一个终端，用sudo权限启动wireshark  
```
$ sudo wireshark
``` 
> 注意：所使用的wireshark版本需要足够新支持识别openflow1.3协议数据

2. 启动后在上面Filter栏中输入openflow_v4，回车

### 2. 启动ryu控制器  
新打开一个终端，启动我们自己实现的tt_13 App  
```
$ sudo ryu-manager --verbose ryu.app.simple_switch_tt_13 
```
> 注意：在运行ryu控制器之前，需要将调度表文件放在某个目录下，并将ryu/app/simple_switch_tt_13.py中调度表文件地址更改为该目录

### 3. 启动基于ovs的拓扑 
新打开一个终端，启动mininet
```
$ sudo mn --switch ovs --controller remote  
```

通过分析wireshark采集的数据判断行为是否符合预期。