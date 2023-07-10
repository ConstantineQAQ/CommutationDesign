#include "Send.h"


void Send::sendDataFrame(Frame &frame)
{
    LoRa.beginPacket();
    LoRa.print(frame.senderAddress);
    LoRa.print(frame.receiverAddress);
    LoRa.print(frame.destinationAddress);
    LoRa.print(frame.frameType);
    LoRa.print(frame.frameData.dataFrameData);
    LoRa.endPacket();
    Serial.print("SendData: ");
    Serial.print(frame.senderAddress);
    Serial.print(frame.receiverAddress);
    Serial.print(frame.destinationAddress);
    Serial.print(frame.frameType);
    Serial.print(frame.frameData.dataFrameData);
    Serial.println();
    Serial.print("frameData.dataFrameData: ");
    Serial.println(frame.frameData.dataFrameData);
    Serial.println("***************************");
}

void Send::sendRequestFrame(Frame &frame)
{
    LoRa.beginPacket();
    LoRa.print(frame.senderAddress);
    LoRa.print(frame.receiverAddress);
    LoRa.print(frame.destinationAddress);
    LoRa.print(frame.frameType);
    LoRa.print(frame.frameData.requestData);
    LoRa.endPacket();
    Serial.print("SendRequest:");
    Serial.print(frame.senderAddress);
    Serial.print(frame.receiverAddress);
    Serial.print(frame.destinationAddress);
    Serial.print(frame.frameType);
    Serial.print(frame.frameData.requestData);
    Serial.println();
    Serial.println("***************************");
}

void Send::sendHeartbeatFrame(Frame &frame)
{
    LoRa.beginPacket();
    LoRa.print(frame.senderAddress);
    LoRa.print(frame.receiverAddress);
    LoRa.print(frame.destinationAddress);
    LoRa.print(frame.frameType);
    LoRa.endPacket();
    Serial.print("SendHeartbeat:");
    Serial.print(frame.senderAddress);
    Serial.print(frame.receiverAddress);
    Serial.print(frame.destinationAddress);
    Serial.print(frame.frameType);
    Serial.println();
    Serial.println("***************************");
}

void Send::sendResponseFrame(Frame &frame)
{
    LoRa.beginPacket();
    LoRa.print(frame.senderAddress);
    LoRa.print(frame.receiverAddress);
    LoRa.print(frame.destinationAddress);
    LoRa.print(frame.frameType);
    LoRa.print(frame.frameData.responseData);
    LoRa.endPacket();
    Serial.print("SendResponse: ");
    Serial.print(frame.senderAddress);
    Serial.print(frame.receiverAddress);
    Serial.print(frame.destinationAddress);
    Serial.print(frame.frameType);
    Serial.print(frame.frameData.responseData);
    Serial.println();
}

void Send::sendMasterNodeLoginFrame(Frame &frame)
{
    LoRa.beginPacket();
    LoRa.print(frame.senderAddress);
    LoRa.print(frame.receiverAddress);
    LoRa.print(frame.destinationAddress);
    LoRa.print(frame.frameType);
    LoRa.endPacket();
    Serial.print("MasterNodeOnline: ");
    Serial.print(frame.senderAddress);
    Serial.print(frame.receiverAddress);
    Serial.print(frame.destinationAddress);
    Serial.print(frame.frameType);
    Serial.println();
    Serial.println("***************************");
}

void Send::sendRetransmissionFrame(Frame &frame)
{
    LoRa.beginPacket();
    LoRa.print(frame.senderAddress);
    LoRa.print(frame.receiverAddress);
    LoRa.print(frame.destinationAddress);
    LoRa.print(frame.frameType);
    LoRa.endPacket();
    Serial.print("SendRetransmission: ");
    Serial.print(frame.senderAddress);
    Serial.print(frame.receiverAddress);
    Serial.print(frame.destinationAddress);
    Serial.print(frame.frameType);
    Serial.println();
    Serial.println("***************************");
}

void Send::sendTopologyChangeFrame(Frame &frame)
{
    LoRa.beginPacket();
    LoRa.print(frame.senderAddress);
    LoRa.print(frame.receiverAddress);
    LoRa.print(frame.destinationAddress);
    LoRa.print(frame.frameType);
    LoRa.print(frame.frameData.TopologyChangeData);
    LoRa.endPacket();
    Serial.print("SendTopologyChange:");
    Serial.print(frame.senderAddress);
    Serial.print(frame.receiverAddress);
    Serial.print(frame.destinationAddress);
    Serial.print(frame.frameType);
    Serial.print(frame.frameData.TopologyChangeData);
    Serial.println();
    Serial.println("***************************");
}

void Send::sendNeedACK(Frame &frame, int retryTimes, int timeout, const char currentAddress)
{
    int currentNode; // 当前节点下标 ，也就是发送节点
    int destinationNode; // 目的地址下标
    for (int i = 0; i < retryTimes; i++)
    {
        // 发送帧
        sendFrame(frame);
        // 等待确认消息
        unsigned long startTime = millis();
        while (millis() - startTime < timeout)
        {
            if (receive->receiveACK(currentAddress))
            {
                Serial.println("ACK");
                return;
            }
        }
    }
    // 如果没有收到确认消息，打印一个错误消息
    Serial.println("Did not receive ACK");
    switch (currentAddress)
    {
    case 's':
        currentNode = 1;
        break;
    case 'b':
        currentNode = 2;
        break;
    case 't':
        currentNode = 3;
        break;
    default:
        currentNode = 0;
        break;
    }
    switch (frame.destinationAddress[0])  // 使用第一个字符
    {
    case 's':
        destinationNode = 1;
        break;
    case 'b':
        destinationNode = 2;
        break;
    case 't':
        destinationNode = 3;
        break;
    default:
        destinationNode = 0;
        break;
    }
    Frame TopologyChangeFrame; //准备更新拓扑结构的帧
    String TopologyString = ""; // 准备拓扑帧中的数据
    switch (frame.frameType)
    {
    // 对于请求帧， 如果没有收到ACK，就判断拓扑结构中自己是否在。
    case REQUEST_FRAME:
        if (Topology[currentNode])
        {
            Serial.print(currentAddress);
            Serial.println("is in the topology");
        }
        else
        {
            Serial.print(currentAddress);
            Serial.println("not in the topology");
        }
        break;
    // 对于心跳帧，如果没有收到ACK，主节点就删除对应的目的地址
    case HEARTBEAT_FRAME:
        Serial.print("Delete the destination address: ");
        Serial.println(frame.destinationAddress);
        Topology[destinationNode] = false;
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
        for (int i = 1; i < 4; i++)
        {
            if (Topology[i] == true)
            {
                TopologyChangeFrame.initTopologyChangeFrame("m", white[i-1], white[i-1], TOPOLOGY_CHANGE_FRAME, TopologyString);
                sendNeedACK(TopologyChangeFrame, 3, 2000, currentAddress);
            }
        }
        break;
    // 对于数据帧，如果主节点没有收到ACK，就删除终端节点，反之，如果终端节点没有收到ACK，就删除主节点
    // 对于重传帧，如果没有收到ACK，就删除对应的目的地址
    case RETRANSMISSION_FRAME:
        Serial.print("Delete the destination address: ");
        Serial.println(frame.destinationAddress);
        Topology[destinationNode] = false;
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
        for (int i = 1; i < 4; i++)
        {
            if (Topology[i] == true)
            {
                TopologyChangeFrame.initTopologyChangeFrame("m", white[i-1], white[i-1], TOPOLOGY_CHANGE_FRAME, TopologyString);
                sendNeedACK(TopologyChangeFrame, 3, 1000, currentAddress);
            }
        }
        break;
    // 对于拓扑变化帧，如果没有收到ACK，就删除对应的目的地址
    case TOPOLOGY_CHANGE_FRAME:
        Serial.print("Delete the destination address: ");
        Serial.println(frame.destinationAddress);
        Topology[destinationNode] = false;
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
        for (int i = 1; i < 4; i++)
        {
            if (Topology[i] == true)
            {
                TopologyChangeFrame.initTopologyChangeFrame("m", white[i-1], white[i-1], TOPOLOGY_CHANGE_FRAME, TopologyString);
                sendNeedACK(TopologyChangeFrame, 3, 1000, currentAddress);
            }
        }
        break;    
    default:
        break;
    }
}

void Send::sendFrame(Frame &frame)
{
    switch (frame.frameType)
    {
        case REQUEST_FRAME:
            sendRequestFrame(frame);
            break;
        case RESPONSE_FRAME:
            sendResponseFrame(frame);
            break;
        case DATA_FRAME:
            sendDataFrame(frame);
            break;
        case HEARTBEAT_FRAME:
            sendHeartbeatFrame(frame);
            break;
        case MASTER_NODE_FRAME:
            sendMasterNodeLoginFrame(frame);
            break;
        case RETRANSMISSION_FRAME:
            sendRetransmissionFrame(frame);
            break;
        case TOPOLOGY_CHANGE_FRAME:
            sendTopologyChangeFrame(frame);
            break;
        default:
            Serial.println("Error: FrameType is not defined");
    }
}
