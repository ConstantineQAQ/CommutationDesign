#include "Receive.h"

const char* white[3] = {"s", "b", "t"};
// 0:主节点 1:中继节点 2:中继节点 3:终端节点
bool Topology[4] = {false,false,false,false};
String ReceiveData;
String SendData = "";
Send* sender = new Send();

String Receive::processPacket(const char currentAddress)
{
    // 先根据当前节点在拓扑中的位置
    // switch(currentAddress)
    // {
    //     case 'm':
    //         // 主节点
    //         if(!Topology[0])
    //         {
    //             return "";
    //         }
    //         break;
    //     case 's':
    //         // 中继节点一号
    //         if(!Topology[1])
    //         {
    //             return "";
    //         }
    //         break;
    //     case 'b':
    //         // 中继节点二号
    //         if(!Topology[2])
    //         {
    //             return "";
    //         }
    //         break;
    //     case 't':
    //         // 终端节点
    //         if(!Topology[3])
    //         {
    //             return "";
    //         }
    //         break;
    //     default:
    //         Serial.println("Error");
    //         break;
    // }
    SendData = ""; // 清空发送数据
    int packetSize = LoRa.parsePacket();
    if(packetSize){
        while(LoRa.available())
        {
            String ReceiveData = LoRa.readString();
            char senderAddress = ReceiveData.charAt(0); // 获取发送地址
            char receiverAddress = ReceiveData.charAt(1); // 获取接收地址
            char destinationAddress = ReceiveData.charAt(2); // 获取目的地址
            char FrameType = ReceiveData.charAt(3); // 获取帧类型
            int FrameTypeInt = FrameType - '0';
            // 判断当前节点是否为接受节点
            if (currentAddress == receiverAddress || receiverAddress == 'a')
            {
            switch(FrameTypeInt) 
            {
                case REQUEST_FRAME:
                    // 判断是否为请求帧
                    Serial.println("ReceiveRequestFrame");
                    processRequestFrame(ReceiveData);
                    Serial.println("***************************");
                    break;
                case HEARTBEAT_FRAME:
                    // 判断是否为心跳帧
                    Serial.println("ReceiveHeartbeatFrame");
                    processHeartbeatFrame(ReceiveData);
                    Serial.println("***************************");
                    break;
                case RESPONSE_FRAME:
                    // 判断是否为响应帧
                    Serial.println("ReceiveResponseFrame");
                    processResponseFrame(ReceiveData);
                    Serial.println("***************************");
                    break;
                case MASTER_NODE_FRAME:
                    // 判断是否为主节点可服务帧
                    Serial.println("ReceiveMasterNodeFrame");
                    processMasterNodeJoinRequest(ReceiveData, currentAddress);
                    Serial.println("***************************");
                    break;
                case DATA_FRAME:
                    // 判断是否为数据帧
                    Serial.println("ReceiveDataFrame");
                    SendData = processDataFrame(ReceiveData, destinationAddress, receiverAddress);
                    Serial.println("***************************");
                    break;
                case RETRANSMISSION_FRAME:
                    // 判断是否为告知重传帧
                    Serial.println("ReceiveRetransmissionFrame");
                    processRetransmissionFrame(ReceiveData);
                    Serial.println("***************************");
                    break;
                case TOPOLOGY_CHANGE_FRAME:
                    // 判断是否为拓扑结构帧
                    Serial.println("ReceiveTopologyChangeFrame");
                    processTopologyChangeFrame(ReceiveData);
                    Serial.println("***************************");
                    break;
                default:
                    // 你可以在这里处理未知的帧类型
                    Serial.println("UnknownFrame");
                    Serial.println("***************************");
                    break;
            }
            }
            // Serial.print("ReceiveRawData: ");
            // Serial.println(ReceiveData);
            // Frame Receiveframe;
            // Receiveframe.toStruct(ReceiveData);
        }
    }
    return SendData;
}

bool Receive::receiveACK(const char currentAddress)
{
    int packetSize = LoRa.parsePacket();
    if(packetSize){
        while(LoRa.available())
        {
            String ReceiveData = LoRa.readString();
            char senderAddress = ReceiveData.charAt(0); // 获取发送地址
            char receiverAddress = ReceiveData.charAt(1); // 获取接收地址
            char destinationAddress = ReceiveData.charAt(2); // 获取目的地址
            char FrameType = ReceiveData.charAt(3); // 获取帧类型
            int FrameTypeInt = FrameType - '0';
            char SecondChar[2]; // 创建一个只包含一个字符的字符串
            SecondChar[0] = ReceiveData.charAt(1); // 将ReceiveData的第一个字符复制到firstChar
            SecondChar[1] = '\0'; // 添加字符串结束标记
            char* ReceiveAddress = SecondChar; // 将firstChar的地址赋给Sendaddress
            char ThirdChar[2]; // 创建一个只包含一个字符的字符串
            ThirdChar[0] = ReceiveData.charAt(2); // 将ReceiveData的第一个字符复制到firstChar
            ThirdChar[1] = '\0'; // 添加字符串结束标记
            char* DestinationAddress = ThirdChar; // 将ThirdChar的地址赋给DestinationAddress
            if (currentAddress == receiverAddress)
            {
                // 再判断是否为目的节点
                if (receiverAddress == destinationAddress)
                {
                    return true;
                }
                else if(FrameTypeInt == RESPONSE_FRAME && receiverAddress != destinationAddress)
                {
                    // 转发ACK
                    // 先判断有几个中继节点在线
                    int count = 0;
                    for (int i = 1; i < 3; i++)
                    {
                        if (Topology[i])
                        {
                            count++;
                        }
                    }
                    // 如果有兄弟中继节点在线
                    if (count == 2 && receiverAddress == 's')
                    {
                        // 转发ACK
                        Frame ACKframe;
                        ACKframe.initResponseFrame(ReceiveAddress, "b", DestinationAddress, RESPONSE_FRAME, DATA_RESPONSE);
                        sender->sendFrame(ACKframe);
                        Serial.println("ReceiveACK");
                        Serial.println("***************************");
                    }
                    else if (receiverAddress == 'b')
                    {
                        // 转发ACK
                        Frame ACKframe;
                        ACKframe.initResponseFrame(ReceiveAddress, DestinationAddress, DestinationAddress, RESPONSE_FRAME, DATA_RESPONSE);
                        sender->sendFrame(ACKframe);
                        Serial.println("ReceiveACK");
                        Serial.println("***************************");
                    }
                    else if(count == 1)
                    {
                        // 转发ACK
                        Frame ACKframe;
                        ACKframe.initResponseFrame(ReceiveAddress, DestinationAddress, DestinationAddress, RESPONSE_FRAME, DATA_RESPONSE);
                        sender->sendFrame(ACKframe);
                        Serial.println("ReceiveACK");
                        Serial.println("***************************");
                    }
                }
            }
        }
        return false;
    }
}

    void Receive::processRequestFrame(const String& ReceiveData) {
        char fifthChar = ReceiveData.charAt(4); // 获取数据位
        int fifthValue = fifthChar - '0'; // 将字符转换为整数
        // 判断是否为入网请求
        if(fifthValue == NETWORK_JOIN_REQUEST)
        {
            Serial.print("ReceiveJoinRequest");
            processNetworkJoinRequest(ReceiveData);
        }
        // 判断是否为退网请求
        if(fifthValue == NETWORK_LEAVE_REQUEST){
            Serial.print("ReceiveLeaveRequest");
            processNetworkLeaveRequest(ReceiveData);
        }
    }

    void Receive::processNetworkJoinRequest(const String& ReceiveData) {
        Serial.print(ReceiveData);
        // 判断是否在白名单内
        char firstChar[2]; // 创建一个只包含一个字符的字符串
        firstChar[0] = ReceiveData.charAt(0); // 将ReceiveData的第一个字符复制到firstChar
        firstChar[1] = '\0'; // 添加字符串结束标记
        char* Sendaddress = firstChar; // 将firstChar的地址赋给Sendaddress
        for(int i = 0; i < 3; i++)
        {
            if (strcmp(Sendaddress, white[i]) == 0) {
                // 主节点响应该入网节点
                Frame JoinResponseframe;
                JoinResponseframe.initResponseFrame("m",Sendaddress,Sendaddress,RESPONSE_FRAME,REQUEST_RESPONSE);
                sender->sendFrame(JoinResponseframe);
                // 更新拓扑
                if(strcmp(Sendaddress, "s") == 0){
                    Topology[1] = true;
                }
                else if(strcmp(Sendaddress, "b") == 0){
                    Topology[2] = true;
                }
                else if(strcmp(Sendaddress, "t") == 0){
                    Topology[3] = true;
                }
                //写一个for循环读取拓扑数组，然后发送对应的四位二进制数
                String TopologyString = "";
                for(int i = 0; i < 4; i++){
                    if(Topology[i] == true){
                        TopologyString += "1";
                    }
                    else{
                        TopologyString += "0";
                    }
                }
                // 组帧
                Frame Topologyframe;
                for (int i = 1; i < 4; i++)
                {
                    if (Topology[i] == true)
                    {
                        // 将拓扑变化发给在网的节点
                        Topologyframe.initTopologyChangeFrame("m", white[i-1], white[i-1], TOPOLOGY_CHANGE_FRAME, TopologyString);
                        sender->sendNeedACK(Topologyframe, 3, 2000, 'm');
                    }
                }
            }
        }
    }

    void Receive::processNetworkLeaveRequest(const String& ReceiveData) {
        // 组帧
        char firstChar[2]; // 创建一个只包含一个字符的字符串
        firstChar[0] = ReceiveData.charAt(0); // 将ReceiveData的第一个字符复制到firstChar
        firstChar[1] = '\0'; // 添加字符串结束标记
        char* Sendaddress = firstChar; // 将firstChar的地址赋给Sendaddress
        // 主节点响应该退网节点
        Frame LeaveResponseframe;
        LeaveResponseframe.initResponseFrame("m",Sendaddress,Sendaddress,RESPONSE_FRAME,REQUEST_RESPONSE);
        sender->sendFrame(LeaveResponseframe);
        // 更新拓扑
        if(strcmp(Sendaddress, "s") == 0){
            Topology[1] = false;
        }
        else if(strcmp(Sendaddress, "b") == 0){
            Topology[2] = false;
        }
        else if(strcmp(Sendaddress, "t") == 0){
            Topology[3] = false;
        }
        //写一个for循环读取拓扑里为true的个数
        String TopologyString = "";
        for(int i = 0; i < 4; i++)
        {
            if(Topology[i] == true)
            {
                TopologyString += "1";
            }
            else{
                TopologyString += "0";
            }
        }
        // 组帧
        Frame Topologyframe;
        for (int i = 1; i < 4; i++)
        {
            if (Topology[i] == true)
            {
                // 将拓扑变化发给在网的节点
                Topologyframe.initTopologyChangeFrame("m", white[i-1], white[i-1], TOPOLOGY_CHANGE_FRAME, TopologyString);
                sender->sendNeedACK(Topologyframe, 3, 1000, 'm');
            }
        }
    }

    void Receive::processHeartbeatFrame(const String& ReceiveData) 
    {
        // 中继节点组帧
        char firstChar[2]; // 创建一个只包含一个字符的字符串
        firstChar[0] = ReceiveData.charAt(2); // 将ReceiveData的第一个字符复制到firstChar
        firstChar[1] = '\0'; // 添加字符串结束标记
        char* Sendaddress = firstChar; // 将firstChar的地址赋给Sendaddress
        Frame HeartbeatResponseframe;
        HeartbeatResponseframe.initResponseFrame(Sendaddress,"m","m",RESPONSE_FRAME,HEARTBEAT_RESPONSE);
        sender->sendFrame(HeartbeatResponseframe);
    }

    void Receive::processResponseFrame(const String& ReceiveData) 
    {
        char fifthChar = ReceiveData.charAt(4); // 获取第五个字符
        int fifthValue = fifthChar - '0'; // 将字符转换为整数
        if(fifthValue == MASTERJOIN_RESPONSE){
            // 你的处理主节点可服务响应的代码
            processMasterNodeJoinResponse(ReceiveData);
        }
    }

    // 对于主节点来说，处理自己发送的广播上网告知的响应
    void Receive::processMasterNodeJoinResponse(const String &ReceiveData)
    {
        Serial.println("a node try to join the network");
        char firstChar[2]; // 创建一个只包含一个字符的字符串
        firstChar[0] = ReceiveData.charAt(0); // 将ReceiveData的第一个字符复制到firstChar
        firstChar[1] = '\0'; // 添加字符串结束标记
        char* Sendaddress = firstChar; // 将firstChar的地址赋给Sendaddress
        // 主节点收到该节点对于主节点可服务的响应
        // 判断该节点是否在白名单中
        for(int i = 0; i < 3; i++)
        {
            if(strcmp(Sendaddress, white[i]) == 0)
            {
                Serial.println(String(Sendaddress) + " node is online");
                // 主节点响应该入网节点
                Frame JoinResponseframe;
                JoinResponseframe.initResponseFrame("m",Sendaddress,Sendaddress,RESPONSE_FRAME,REQUEST_RESPONSE);
                sender->sendFrame(JoinResponseframe);
                // 更新拓扑
                if(strcmp(Sendaddress, "s") == 0)
                {
                    Topology[1] = true;
                }
                else if(strcmp(Sendaddress, "b") == 0)
                {
                    Topology[2] = true;
                }
                else if(strcmp(Sendaddress, "t") == 0)
                {
                    Topology[3] = true;
                }
                // 拓扑变化帧的数据
                String TopologyString = "";
                for(int i = 0; i < 4; i++)
                {
                    if(Topology[i] == true)
                    {
                        TopologyString += "1";
                    }
                    else{
                        TopologyString += "0";
                    }
                }
                // 组帧
                Frame Topologyframe;
                for (int i = 1; i < 4; i++)
                {
                    if (Topology[i] == true)
                    {
                        // 将拓扑变化发给在网的节点
                        Topologyframe.initTopologyChangeFrame("m", white[i-1], white[i-1], TOPOLOGY_CHANGE_FRAME, TopologyString);
                        sender->sendNeedACK(Topologyframe, 3, 2000, 'm');
                    }
                }
            }
        }
    }


    // 对于中继节点来说，处理来自主节点的广播上网告知
    void Receive::processMasterNodeJoinRequest(const String& ReceiveData, const char currentAddress){
        // 将当前节点字符转换为字符串
        String currentaddress = String(currentAddress);
        Frame MasterNodeJoinResponseframe;
        MasterNodeJoinResponseframe.initResponseFrame(currentaddress.c_str(),"m","m",RESPONSE_FRAME,MASTERJOIN_RESPONSE);
        sender->sendFrame(MasterNodeJoinResponseframe);
        sender->sendFrame(MasterNodeJoinResponseframe);
    }

    // 四个节点都有可能收到数据帧
    String Receive::processDataFrame(const String &ReceiveData, const char destinationAddress, const char receiveAddress)
    {
        char SecondChar[2]; // 创建一个只包含一个字符的字符串
        SecondChar[0] = ReceiveData.charAt(1); // 将ReceiveData的第一个字符复制到firstChar
        SecondChar[1] = '\0'; // 添加字符串结束标记
        char* ReceiveAddress = SecondChar; // 将firstChar的地址赋给Sendaddress
        char ThirdChar[2]; // 创建一个只包含一个字符的字符串
        ThirdChar[0] = ReceiveData.charAt(2); // 将ReceiveData的第一个字符复制到firstChar
        ThirdChar[1] = '\0'; // 添加字符串结束标记
        char* DestinationAddress = ThirdChar; // 将firstChar的地址赋给Receiveaddress
        // 解包数据帧中的信息
        String unzipData = ReceiveData.substring(4, ReceiveData.length());
        Serial.println("unzipData: " + unzipData);
        // 先判断有几个中继节点在线
        int count = 0;
        for (int i = 1; i < 3; i++)
        {
            if (Topology[i])
            {
                count++;
            }
        }
        Serial.print("active Node is");
        Serial.println(count);
        // 判断目的地址是否为自己
        if (destinationAddress == receiveAddress)
        {
            return unzipData;
        }
        else
        {   // 目的地址不是自己就转发
            // 如果有兄弟中继节点在线
            if (count == 2 && receiveAddress == 's')
            {
                Serial.println("condition 1");
                // 组帧
                Frame Dataframe;
                Dataframe.initDataFrame(ReceiveAddress, "b", DestinationAddress, DATA_FRAME, unzipData);
                sender->sendNeedACK(Dataframe, 3, 1000, ReceiveAddress);
            }
            else if (receiveAddress == 'b')
            {
                Serial.println("condition 2");
                // 组帧
                Frame Dataframe;
                Dataframe.initDataFrame(ReceiveAddress, "t", DestinationAddress, DATA_FRAME, unzipData);
                sender->sendNeedACK(Dataframe, 3, 1000, ReceiveAddress);
            }
            else if (count == 1)
            {
                Serial.println("condition 3");
                // 组帧
                Frame Dataframe;
                Dataframe.initDataFrame(ReceiveAddress, "t", DestinationAddress, DATA_FRAME, unzipData);
                sender->sendNeedACK(Dataframe, 3, 1000, ReceiveAddress);
            }
        return "";
    }
    }

    void Receive::processRetransmissionFrame(const String &ReceiveData)
    {
        // TODO
    }

    void Receive::processTopologyChangeFrame(const String &ReceiveData)
    {
        char firstChar[2]; // 创建一个只包含一个字符的字符串
        firstChar[0] = ReceiveData.charAt(1); // 将ReceiveData的第一个字符复制到firstChar
        firstChar[1] = '\0'; // 添加字符串结束标记
        char* Sendaddress = firstChar; // 将firstChar的地址赋给Sendaddress
        String TopologyString = ReceiveData.substring(4, ReceiveData.length());
        // 更新拓扑
        for(int i = 0; i < 4; i++)
        {
            if(TopologyString.charAt(i) == '1')
            {
                Topology[i] = true;
            }
            else{
                Topology[i] = false;
            }
        }
        // TODO 可靠通信
        // 组帧
        Frame TopologyResponseframe;
        TopologyResponseframe.initResponseFrame(Sendaddress,"m","m",RESPONSE_FRAME, DATA_RESPONSE);
        sender->sendFrame(TopologyResponseframe);
        // 打印拓扑
        Serial.println("Topology: ");
        for(int i = 0; i < 4; i++)
        {
            Serial.print(Topology[i]);
        }
    }
