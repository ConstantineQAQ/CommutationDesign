# 重庆大学通信工程课程设计
## 课程设计题目
- 中继无线传输系统设计
## 课程设计内容
### 基本要求
- 1.完成三个无线设备的中继传输组网设计，完成白名单制度的设备入网过程；
- 2.中继无线传输系统网络结构为主节点-中继节点-终端节点；
- 3.完成传输结构（拓扑）信息维护，反映各节点（主、中继、终端）变化；
- 4.以帧结构形式传递三种以上信息
### 提高要求
- 1.利用LabVIEW平台实现PC端可视化控制（PC-主节点-中继节点-终端节点）的信道可读、可设置、可显示
- 2.动态新加入节点，以4个节点（2级中继节点）完成可靠通信模式下的业务交互（信息以帧结构方式传输），同时满足提高要求1
- 3.实现灵活的可靠组网过程，实现拓扑结构维护，同时满足提高要求1
- 4.完成组网设计方案编写，实现入网、网络维护、网络状态信息显示告警、网络参数设置在线调整（网络运行时可调整）等方案设计，完成其状态转换图、软件模块设计、结构化软件设计、伪代码设计。
- 5.扩展发挥，设计实用环境完成无限信息传输。
### 程序目录
- FrameStructure：帧结构
- MasterNode：主节点
- SlaveNode：中继节点
- TerminalNode：终端节点
- Send:发送Utils
- Receive:接收Utils
### 运行方法
- 1.压缩包解压后，将FrameStructure、Send、Receive作为模块导入Arduino IDE
- 2.将MasterNode、SlaveNode、SlaveNodeB、TerminalNode分别烧录至三块不同的Arduino板
- TODO
## 思路介绍
### 帧结构设计
- 发送地址：1字节
- 接收地址：1字节
- 目的地址：1字节
- 帧标识：1字节
- 数据帧：可选字节长度
### 节点入网方式：
- 1.中继节点上电后，发送入网请求，等待主节点检查其是否在白名单中
- 2.主节点不在网，主节点入网后广播主节点可服务帧，其他节点收到后回复该帧，主节点将其加入
### 无线传输网络结构
- 1.整体采用虚电路传输方式
- 2.主节点发送前就要知道中继谁在网，并按照当前方式发送给终端
### 拓扑信息维护
- 1.当某一个节点掉网后，他将无法回复来自主节点的心跳帧，主节点如果没有收到ACK就将其踢出网络并给所有在网的节点拓扑变化帧
- 2.当某一个节点上网后