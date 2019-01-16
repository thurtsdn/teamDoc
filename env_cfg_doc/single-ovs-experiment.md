### 单OVS下实验过程:

1. 启动ryu和mininet，参考文档https://github.com/thurtsdn/TeamDoc/blob/master/env_cfg_doc/startup-of1.3-tt-exp-env%20.md

2. 打开TT报文网络端口63000

   TT报文的目的端口为63000，进行实验之前需打开主机的63000端口，否则将出现ICMP端口不可达的错误报文。在mininet中输入下面的命令：

   ```
   # mininet中打开主机h2的对应端口
   mininet >> h2 ufw enable
   mininet >> h2 iptables -A INPUT -p udp -i h2-eth0 --dport 63000 -j ACCEPT
   ```


3. ryu发送TT调度表，在/home/chenwh/Workspace/Data/tt_test/schedule/  目录下存放调度表文件。具体格式见https://github.com/thurtsdn/TeamDoc/tree/master/test_data/tt_test_sj/schedule 文件夹下调度表格式。

   > 具体实验的过程中，vport的id存在随机的变化，接下来将会解决该问题，在单ovs双端口的情况下，ovs会生成四个端口，其中id为0的端口为内部端口，在调度表中，一般使用id为1或者2的端口号，可以通过先使用send.sh发送报文，通过dmesg | grep PROCESS中的信息推测当前发送的端口的id。

4. 使用https://github.com/thurtsdn/TeamDoc/blob/master/shell/send.sh 脚本发送TT报文，其中UDP报文的数据域为flow id。

5. 内核态可用来调试的信息

   dmesg | grep OVS_TT_FLOW_ATTR 显示内核收到的TT调度表信息

   ```
   [60017.128294] openvswitch: I get the OVS_TT_FLOW_ATTR_PORT: 1
   [60017.128300] openvswitch: I get the OVS_TT_FLOW_ATTR_ETYPE: 0
   [60017.128302] openvswitch: I get the OVS_TT_FLOW_ATTR_FLOW_ID: 0
   [60017.128305] openvswitch: I get the OVS_TT_FLOW_ATTR_BASE_OFFSET: 0
   [60017.128307] openvswitch: I get the OVS_TT_FLOW_ATTR_PERIOD: 4000000000
   [60017.128309] openvswitch: I get the OVS_TT_FLOW_ATTR_BUFFER_ID: 0
   [60017.128312] openvswitch: I get the OVS_TT_FLOW_ATTR_PACKET_SIZE: 64
   [60017.128314] openvswitch: I get the OVS_TT_FLOW_ATTR_EXECUTE_TIME: 0
   ```

   dmesg | grep DISPATCH 显示一个宏周期内TT报文的发送顺序

   ```
   [60017.128797] DISPATCH: macro_period: 16000000000, size: 14
   [60017.128799] DISPATCH: index 0, flow_id: 0, send_time: 0
   [60017.128802] DISPATCH: index 1, flow_id: 2, send_time: 1000000000
   [60017.128804] DISPATCH: index 2, flow_id: 4, send_time: 2000000000
   [60017.128806] DISPATCH: index 3, flow_id: 1, send_time: 3000000000
   [60017.128808] DISPATCH: index 4, flow_id: 0, send_time: 4000000000
   [60017.128810] DISPATCH: index 5, flow_id: 5, send_time: 5000000000
   [60017.128812] DISPATCH: index 6, flow_id: 3, send_time: 6000000000
   [60017.128813] DISPATCH: index 7, flow_id: 1, send_time: 7000000000
   [60017.128815] DISPATCH: index 8, flow_id: 0, send_time: 8000000000
   [60017.128817] DISPATCH: index 9, flow_id: 2, send_time: 9000000000
   [60017.128819] DISPATCH: index 10, flow_id: 1, send_time: 11000000000
   [60017.128821] DISPATCH: index 11, flow_id: 0, send_time: 12000000000
   [60017.128823] DISPATCH: index 12, flow_id: 3, send_time: 14000000000
   [60017.128824] DISPATCH: index 13, flow_id: 1, send_time: 15000000000
   ```

   dmesg | grep SEND_INFO 显示正在发送和发送完成的TT报文信息

   ```
   [60033.233108] SEND_INFO: mod_time 15000196335, cur_idx 0, current flow id 0, current send time 0
   [60034.231642] SEND_INFO: mod_time 265686, cur_idx 1, current flow id 2, current send time 1000000000
   [60035.230245] SEND_INFO: mod_time 1000404606, cur_idx 2, current flow id 4, current send time 2000000000
   [60036.228749] SEND_INFO: mod_time 2000449502, cur_idx 3, current flow id 1, current send time 3000000000
   [60037.227388] SEND_INFO: mod_time 3000623903, cur_idx 4, current flow id 0, current send time 4000000000
   [60038.225928] SEND_INFO: mod_time 4000698582, cur_idx 5, current flow id 5, current send time 5000000000
   [60039.224586] SEND_INFO: mod_time 5000896900, cur_idx 6, current flow id 3, current send time 6000000000
   [60040.222761] SEND_INFO: mod_time 6000608075, cur_idx 7, current flow id 1, current send time 7000000000
   [60041.220791] SEND_INFO: mod_time 7000173706, cur_idx 8, current flow id 0, current send time 8000000000
   [60042.219718] SEND_INFO: mod_time 8000639513, cur_idx 9, current flow id 2, current send time 9000000000
   [60044.216490] SEND_INFO: mod_time 10000488224, cur_idx 10, current flow id 1, current send time 11000000000
   [60045.214722] SEND_INFO: mod_time 11000253249, cur_idx 11, current flow id 0, current send time 12000000000
   [60047.211669] SEND_INFO: mod_time 13000273939, cur_idx 12, current flow id 3, current send time 14000000000
   [60048.210696] SEND_INFO: mod_time 14000840326, cur_idx 13, current flow id 1, current send time 15000000000
   ```

   > 1. 其中mod_time为报文在当前宏周期内的时间点，current send time为接下来要发送的tt报文在宏周期内的时间点。例如第一条中，宏周期为16000000000，mod_time为15000196335，根据上一步DISPATCH的信息可知，在当前的宏周期内没有要发送的报文，所以选择下一个宏周期的第一个报文进行发送。
   > 2. SEND_INFO显示的发送的flow id信息应该和DISPATCH显示的flow id顺序相同，但是存在可能两个报文在宏周期上发送的时间相同，这种情况下经过测试，SEND_INFO的信息和DISPATCH的信息存在差别。

   dmesg | grep MISS 显示无法发送的TT报文或丢包信息

   ```
   [60018.256442] MISS: vport id 1 can't send flow id 2
   [60019.254901] MISS: vport id 1 can't send flow id 4
   [60020.253515] MISS: vport id 1 can't send flow id 1
   [60021.251579] MISS: vport id 1 can't send flow id 0
   [60022.250782] MISS: vport id 1 can't send flow id 5
   [60023.248750] MISS: vport id 1 can't send flow id 3
   [60024.247405] MISS: vport id 1 can't send flow id 1
   [60025.246054] MISS: vport id 1 can't send flow id 0
   [60026.243763] MISS: vport id 1 can't send flow id 2
   [60028.241508] MISS: vport id 1 can't send flow id 1
   [60029.239379] MISS: vport id 1 can't send flow id 0
   [60031.237047] MISS: vport id 1 can't send flow id 3
   [60032.234980] MISS: vport id 1 can't send flow id 1
   ```

   > 无法发送的原因在于，TT buffer中找不到对应的报文，说明并没有对应的报文已经发送，或者发送了，超过了TT到达表规定的时间

   dmesg | grep FINISH显示TT调度表发送成功的报文信息

   ```
   [60627.292535] FINISH: vport id 1 send flow id 1 
   [60628.290867] FINISH: vport id 1 send flow id 0 
   [60631.285504] FINISH: vport id 1 send flow id 1 
   [60632.284000] FINISH: vport id 1 send flow id 0 
   [60635.279462] FINISH: vport id 1 send flow id 1 
   ```

   当使用send.sh发送报文时，可以使用如下命令查看发送的报文信息

   dmesg | grep PROCESS 显示端口到达的TT报文信息

   ```
   [60623.859531] openvswitch: PROCESS: vport_no 2 arrive flow id 0.
   [60623.859534] openvswitch: PROCESS: vport_no 2 arrive flow id 1.
   [60624.862332] openvswitch: PROCESS: vport_no 2 arrive flow id 0.
   [60624.865659] openvswitch: PROCESS: vport_no 2 arrive flow id 1.
   [60625.870500] openvswitch: PROCESS: vport_no 2 arrive flow id 0.
   [60625.872606] openvswitch: PROCESS: vport_no 2 arrive flow id 1.
   [60626.876345] openvswitch: PROCESS: vport_no 2 arrive flow id 0.
   [60626.879636] openvswitch: PROCESS: vport_no 2 arrive flow id 1.
   ```

   dmesg | grep LATE_PACKET 显示迟到的TT报文信息

   ```
   [60623.859536] openvswitch: LATE_PACKET: flow_id 1 arrive late on vport 2!, throw it!
   [60624.862341] openvswitch: LATE_PACKET: flow_id 0 arrive late on vport 2!, throw it!
   [60624.865667] openvswitch: LATE_PACKET: flow_id 1 arrive late on vport 2!, throw it!
   [60626.876354] openvswitch: LATE_PACKET: flow_id 0 arrive late on vport 2!, throw it!
   [60627.883147] openvswitch: LATE_PACKET: flow_id 1 arrive late on vport 2!, throw it!
   [60628.887069] openvswitch: LATE_PACKET: flow_id 0 arrive late on vport 2!, throw it!
   [60628.890483] openvswitch: LATE_PACKET: flow_id 1 arrive late on vport 2!, throw it!
   [60630.901076] openvswitch: LATE_PACKET: flow_id 0 arrive late on vport 2!, throw it!
   ```

 6. 使用wireshark进行抓包，参见https://github.com/thurtsdn/TeamDoc/blob/master/env_cfg_doc/wireshark-install-from-src.md

