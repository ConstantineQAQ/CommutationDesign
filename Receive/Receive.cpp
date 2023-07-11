#include "Receive.h"

const char* white[3] = {"s", "b", "t"};
// 0:主节点 1:中继节点 2:中继节点 3:终端节点
bool Topology[4] = {false,false,false,false};
String ReceiveData;
String SendData = "";
Send* sender = new Send();

String Receive::processPacket(const char currentAddress)
{
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
            if (currentAddress == receiverAddress)
            {
                // 判断帧类型
                switch(FrameTypeInt) 
                {
                    case REQUEST_FRAME:
                        // 判断是否为请求帧
                        Serial.println("ReceiveRequestFrame");
                        processRequestFrame(ReceiveData, currentAddress);
                        Serial.println("***************************");
                        break;
                    case HEARTBEAT_FRAME:
                        // 判断是否为心跳帧
                        Serial.println("ReceiveHeartbeatFrame");
                        processHeartbeatFrame(ReceiveData, currentAddress);
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
                        SendData = processDataFrame(ReceiveData, destinationAddress, receiverAddress, currentAddress);
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

bool Receive::receiveACK(const char currentAddress,const char destinationAddress)
{
    int packetSize = LoRa.parsePacket();
    if(packetSize){
        while(LoRa.available())
        {
            String ReceiveData = LoRa.readString();
            char R_senderAddress = ReceiveData.charAt(0); // 获取发送地址
            char R_receiverAddress = ReceiveData.charAt(1); // 获取接收地址
            char R_destinationAddress = ReceiveData.charAt(2); // 获取目的地址
            char FrameType = ReceiveData.charAt(3); // 获取帧类型
            int FrameTypeInt = FrameType - '0';
            char send_address[2]; // 创建一个只包含一个字符的字符串
            send_address[0] = ReceiveData.charAt(1); // 将ReceiveData的第一个字符复制到firstChar
            send_address[1] = '\0'; // 添加字符串结束标记
            char* Send_Address = send_address; // 将firstChar的地址赋给Sendaddress
            char receive_address[2]; 
            receive_address[0] = ReceiveData.charAt(1); 
            receive_address[1] = '\0'; 
            char* Receive_Address = receive_address; 
            char destnition_address[2]; 
            destnition_address[0] = ReceiveData.charAt(2); 
            destnition_address[1] = '\0';
            char* Destination_Address = destnition_address;
            if (currentAddress == R_destinationAddress && FrameTypeInt == RESPONSE_FRAME && R_senderAddress == destinationAddress)
            {
                return true;
            }
        }
        return false;
    }
}

    void Receive::processRequestFrame(const String& ReceiveData, const char currentAddress) 
    {
        char fifthChar = ReceiveData.charAt(4); // 获取数据位
        int fifthValue = fifthChar - '0'; // 将字符转换为整数
        // 判断是否是目的节点
        char destinationAddress = ReceiveData.charAt(2); // 获取目的地址
        if (currentAddress == destinationAddress)
        {
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
        else if(currentAddress != destinationAddress)
        {
            // 获取原发送信息中的发送地址和目的地址
            // 获取发送地址
            char send_address[2];
            send_address[0] = ReceiveData.charAt(0);
            send_address[1] = '\0';
            char* Send_Address = send_address;
            // 获取目的地址
            char destination_address[2];
            destination_address[0] = ReceiveData.charAt(2); 
            destination_address[1] = '\0'; 
            char* Destination_Address = destination_address; 
            // 转发请求
            Frame Requestframe;
            // 判断是否为中继节点
            if (currentAddress == 's')
            {
                Requestframe.initRequestFrame(Send_Address, Destination_Address, Destination_Address, REQUEST_FRAME, fifthValue == 1 ? NETWORK_JOIN_REQUEST : NETWORK_LEAVE_REQUEST);
            }
            // 判断是否为中继节点B
            else if (currentAddress == 'b')
            {
                Requestframe.initRequestFrame(Send_Address, Topology[1] == 1 ? "s" : Destination_Address, Destination_Address, REQUEST_FRAME, fifthValue == 1 ? NETWORK_JOIN_REQUEST : NETWORK_LEAVE_REQUEST);
            }
            sender->sendNeedACK(Requestframe, 3, 1000, currentAddress);
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

    template <typename T>
    void Receive::processFrameWithForward(const String& ReceiveData, const char currentAddress, 
                           void (Frame::*initFrameFunc)(const char*, const char*, const char*, FrameType, T), 
                           T responseType)
    {
        // 获取发送地址
        char send_address[2]; // 创建一个只包含一个字符的字符串
        send_address[0] = ReceiveData.charAt(0); // 将ReceiveData的第一个字符复制到firstChar
        send_address[1] = '\0'; // 添加字符串结束标记
        char* Send_Address = send_address; // 将firstChar的地址赋给Sendaddress
        // 获取接受地址
        char receive_address[2];
        receive_address[0] = ReceiveData.charAt(1);
        receive_address[1] = '\0';
        char* Receive_Address = receive_address;
        // 获取目的地址
        char destination_address[2];
        destination_address[0] = ReceiveData.charAt(2); 
        destination_address[1] = '\0'; 
        char* Destination_Address = destination_address;
        FrameType frametype;
        // 判断帧类型
        switch (ReceiveData.charAt(3))
        {
            case '1':
                frametype = REQUEST_FRAME;
                break;
            case '2':
                frametype = HEARTBEAT_FRAME;
                break;
            case '3':
                frametype = RESPONSE_FRAME;
                break;
            case '4':
                frametype = MASTER_NODE_FRAME;
                break;
            case '5':
                frametype = DATA_FRAME;
                break;
            case '6':
                frametype = RETRANSMISSION_FRAME;
                break;
            case '7':
                frametype = TOPOLOGY_CHANGE_FRAME;
                break;
            default:
                break;
        }
        // 判断是否是目的节点
        char destinationAddress = ReceiveData.charAt(2); // 获取目的地址
        if (destinationAddress == currentAddress) 
        {
            // 目的节点响应该帧
            Frame ResponseFrame;
            // 如果是中继S节点，就直接响应主节点
            if (currentAddress == 's')
            {
                (ResponseFrame.*initFrameFunc)(Destination_Address, "m", "m", RESPONSE_FRAME, responseType);
            }
            // 如果是中继B节点，就看中继S节点是不是在线
            else if (currentAddress == 'b')
            {
                (ResponseFrame.*initFrameFunc)(Destination_Address, Topology[1] == 1 ? "s" : "m", "m", RESPONSE_FRAME, responseType);

            }
            // 如果是终端的情况
            else if (currentAddress == 't')
            {
                // 先看中继B节点是不是在线
                if (Topology[2] == 1)
                {
                    (ResponseFrame.*initFrameFunc)(Destination_Address, "b", "m", RESPONSE_FRAME, responseType);
                }
                // 再看中继S节点是不是在线  
                else if (Topology[1] == 1)
                {
                    (ResponseFrame.*initFrameFunc)(Destination_Address, "s", "m", RESPONSE_FRAME, responseType);
                }
                // 如果都不在线，就直接响应主节点
                else
                {
                    (ResponseFrame.*initFrameFunc)(Destination_Address, "m", "m", RESPONSE_FRAME, responseType);
                }
            }
            sender->sendFrame(ResponseFrame);
        }
        else
        {
            // 如果不是目的节点，就转发
            Frame transmitFrame;
            // 如果是中继B节点，就直接转发给终端
            if (currentAddress == 'b')
            {
                (transmitFrame.*initFrameFunc)(Send_Address, Destination_Address, Destination_Address, frametype, responseType);
            }
            // 如果是中继S节点，就看中继B节点是不是在线
            else if (currentAddress == 's')
            {
                // 如果中继B节点在线，就转发给中继B节点
                (transmitFrame.*initFrameFunc)(Send_Address, Topology[2] == 1 ? "b" : Destination_Address, Destination_Address, frametype, responseType);
            }
            sender->sendFrame(transmitFrame);
        }
    }

    void Receive::processHeartbeatFrame(const String& ReceiveData, const char currentAddress) 
    {
        processFrameWithForward<ResponseFrameData>(ReceiveData, currentAddress, &Frame::initResponseFrame, HEARTBEAT_RESPONSE);
    }

    // 对于中继节点来说，处理来自主节点的广播上网告知
    void Receive::processMasterNodeJoinRequest(const String& ReceiveData, const char currentAddress)
    {
        processFrameWithForward<ResponseFrameData>(ReceiveData, currentAddress, &Frame::initResponseFrame, MASTERJOIN_RESPONSE);
    }

    // 处理响应帧
    void Receive::processResponseFrame(const String& ReceiveData) 
    {
        char R_senderAddress = ReceiveData.charAt(0); // 获取发送地址
        char R_receiverAddress = ReceiveData.charAt(1); // 获取接收地址
        char R_destinationAddress = ReceiveData.charAt(2); // 获取目的地址
        char FrameType = ReceiveData.charAt(3); // 获取帧类型
        int FrameTypeInt = FrameType - '0';
        char send_address[2]; // 创建一个只包含一个字符的字符串
        send_address[0] = ReceiveData.charAt(0); // 将ReceiveData的第一个字符复制到firstChar
        send_address[1] = '\0'; // 添加字符串结束标记
        char* Send_Address = send_address; // 将firstChar的地址赋给Sendaddress
        char receive_address[2]; 
        receive_address[0] = ReceiveData.charAt(1); 
        receive_address[1] = '\0'; 
        char* Receive_Address = receive_address; 
        char destnition_address[2]; 
        destnition_address[0] = ReceiveData.charAt(2); 
        destnition_address[1] = '\0';
        char* Destination_Address = destnition_address;
        char fifthChar = ReceiveData.charAt(4); // 获取第五个字符
        int fifthValue = fifthChar - '0'; // 将字符转换为整数
        ResponseFrameData responseFrameData;
        switch (fifthValue)
        {
            case 1:
                responseFrameData = REQUEST_RESPONSE;
                break;
            case 2:
                responseFrameData = HEARTBEAT_RESPONSE;
                break;
            case 3:
                responseFrameData = DATA_RESPONSE;
                break;
            case 4:
                responseFrameData = MASTERJOIN_RESPONSE;
                break;
        }
        // 转发ACK
        Serial.println(String(Receive_Address) + " ReceiveACK from " + String(Send_Address) + " to " + String(Destination_Address));
        Frame transmitACKFrame;
        // 如果是s接受到了这个ACK，那么就看B中继在不在，如果在就转发给B，如果不在就转发给目的地址
        if (R_receiverAddress == 's')
        {
            if (R_destinationAddress == 'm')
            {
                transmitACKFrame.initResponseFrame(Send_Address, Destination_Address, Destination_Address, RESPONSE_FRAME, responseFrameData);
            }
            else if (R_destinationAddress == 't')
            {
                transmitACKFrame.initResponseFrame(Send_Address, Topology[2] == 1 ? "b" : Destination_Address, Destination_Address, RESPONSE_FRAME, responseFrameData);
            }
        }
        // 如果是b与上相反
        else if (R_receiverAddress == 'b')
        {
            if (R_destinationAddress == 't')
            {
                transmitACKFrame.initResponseFrame(Send_Address, Destination_Address, Destination_Address, RESPONSE_FRAME, responseFrameData);
            }
            else if (R_destinationAddress == 'm')
            {
                transmitACKFrame.initResponseFrame(Send_Address, Topology[1] == 1 ? "s" : Destination_Address, Destination_Address, RESPONSE_FRAME, responseFrameData);
            }
        }
        sender->sendFrame(transmitACKFrame);
    }
    
    // 四个节点都有可能收到数据帧
    String Receive::processDataFrame(const String &ReceiveData, const char destinationAddress, const char receiveAddress, const char currentAddress)
    {
        // 解包数据帧中的信息
        String unzipData = ReceiveData.substring(4, ReceiveData.length());
        Serial.println("unzipData: " + unzipData);
        // 获取发送地址
        char send_address[2]; // 创建一个只包含一个字符的字符串
        send_address[0] = ReceiveData.charAt(0); // 将ReceiveData的第一个字符复制到firstChar
        send_address[1] = '\0'; // 添加字符串结束标记
        char* Send_Address = send_address; // 将firstChar的地址赋给Sendaddress
        // 获取接受地址
        char receive_address[2];
        receive_address[0] = ReceiveData.charAt(1);
        receive_address[1] = '\0';
        char* Receive_Address = receive_address;
        // 获取目的地址
        char destination_address[2];
        destination_address[0] = ReceiveData.charAt(2); 
        destination_address[1] = '\0'; 
        char* Destination_Address = destination_address;
        FrameType frametype;
        // 判断帧类型
        switch (ReceiveData.charAt(3))
        {
            case '1':
                frametype = REQUEST_FRAME;
                break;
            case '2':
                frametype = HEARTBEAT_FRAME;
                break;
            case '3':
                frametype = RESPONSE_FRAME;
                break;
            case '4':
                frametype = MASTER_NODE_FRAME;
                break;
            case '5':
                frametype = DATA_FRAME;
                break;
            case '6':
                frametype = RETRANSMISSION_FRAME;
                break;
            case '7':
                frametype = TOPOLOGY_CHANGE_FRAME;
                break;
            default:
                break;
        }
        // 判断是否是目的节点
        if (destinationAddress == currentAddress) 
        {
            if (send_address[0] == 'm')
            {
                // 目的节点响应该帧
                Frame ResponseFrame;
                // 如果是中继S节点，就直接响应主节点
                if (currentAddress == 's')
                {
                    ResponseFrame.initResponseFrame(Destination_Address, "m", "m", RESPONSE_FRAME, DATA_RESPONSE);
                }
                // 如果是中继B节点，就看中继S节点是不是在线
                else if (currentAddress == 'b')
                {
                    ResponseFrame.initResponseFrame(Destination_Address, Topology[1] == 1 ? "s" : "m", "m", RESPONSE_FRAME, DATA_RESPONSE);
                }
                // 如果是终端的情况
                else if (currentAddress == 't')
                {
                    // 先看中继B节点是不是在线
                    if (Topology[2] == 1)
                    {
                        ResponseFrame.initResponseFrame(Destination_Address, "b", "m", RESPONSE_FRAME, DATA_RESPONSE);
                    }
                    // 再看中继S节点是不是在线  
                    else if (Topology[1] == 1)
                    {
                        ResponseFrame.initResponseFrame(Destination_Address, "s", "m", RESPONSE_FRAME, DATA_RESPONSE);
                    }
                    // 如果都不在线，就直接响应主节点
                    else
                    {
                        ResponseFrame.initResponseFrame(Destination_Address, "m", "m", RESPONSE_FRAME, DATA_RESPONSE);
                    }
                }
                sender->sendFrame(ResponseFrame);
                return unzipData;
            }
            else if (send_address[0] == 't')
            {
                // 目的节点响应该帧
                Frame ResponseFrame;
                // 如果是中继B节点，就直接响应终端
                if (currentAddress == 'b')
                {
                    ResponseFrame.initResponseFrame(Destination_Address, "t", "t", RESPONSE_FRAME, DATA_RESPONSE);
                }
                // 如果是中继S节点，就看中继B节点是不是在线
                else if (currentAddress == 's')
                {
                    ResponseFrame.initResponseFrame(Destination_Address, Topology[2] == 1 ? "b" : "t", "t", RESPONSE_FRAME, DATA_RESPONSE);
                }
                // 如果是主节点的情况
                else if (currentAddress == 'm')
                {
                    // 先看中继S节点是不是在线
                    if (Topology[1] == 1)
                    {
                        ResponseFrame.initResponseFrame(Destination_Address, "s", "t", RESPONSE_FRAME, DATA_RESPONSE);
                    }
                    // 再看中继B节点是不是在线  
                    else if (Topology[1] == 1)
                    {
                        ResponseFrame.initResponseFrame(Destination_Address, "b", "t", RESPONSE_FRAME, DATA_RESPONSE);
                    }
                    // 如果都不在线，就直接响应终端
                    else
                    {
                        ResponseFrame.initResponseFrame(Destination_Address, "t", "t", RESPONSE_FRAME, DATA_RESPONSE);
                    }
                }
                sender->sendFrame(ResponseFrame);
                return unzipData;
            }
        }
        else
        {
            // 如果不是目的节点，就转发
            Frame transmitFrame;
            if (send_address[0] == 'm')
            {
                // 如果是中继B节点，就直接转发给终端
                if (currentAddress == 'b')
                {
                    transmitFrame.initDataFrame(Send_Address, Destination_Address, Destination_Address, frametype, unzipData);
                }
                // 如果是中继S节点，就看中继B节点是不是在线
                else if (currentAddress == 's')
                {
                    // 如果中继B节点在线，就转发给中继B节点
                    transmitFrame.initDataFrame(Send_Address, Topology[2] == 1 ? "b" : Destination_Address, Destination_Address, frametype, unzipData);
                }
            }
            else if(send_address[0] == 't')
            {
                // 如果是中继S节点,就直接转发给终端
                if (currentAddress == 's')
                {
                    transmitFrame.initDataFrame(Send_Address, Destination_Address, Destination_Address, frametype, unzipData);
                }
                // 如果是中继B节点，就看中继S节点是不是在线
                else if (currentAddress == 'b')
                {
                    // 如果中继S节点在线，就转发给中继S节点
                    transmitFrame.initDataFrame(Send_Address, Topology[1] == 1 ? "s" : Destination_Address, Destination_Address, frametype, unzipData);
                }
            }
            sender->sendFrame(transmitFrame);
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
        Serial.print("Topology: ");
        for(int i = 0; i < 4; i++)
        {
            Serial.print(Topology[i]);
        }
        Serial.println();
    }

// 实例化模版
template void Receive::processFrameWithForward<ResponseFrameData>(const String& ReceiveData, const char currentAddress, 
                                void (Frame::*initFrameFunc)(const char*, const char*, const char*, FrameType, ResponseFrameData), 
                                ResponseFrameData frameType);

template void Receive::processFrameWithForward<String&>(const String& ReceiveData, const char currentAddress, 
                                        void (Frame::*initFrameFunc)(const char*, const char*, const char*, FrameType, String&), 
                                        String& frameType);