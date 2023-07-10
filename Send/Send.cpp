#include "Send.h"


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
    Serial.print("FrameData: ");
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
    Serial.print("SendRequest: ");
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
    Serial.print("SendHeartbeat: ");
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
    Serial.print("SendTopologyChange: ");
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
    for (int i = 0; i < retryTimes; i++)
    {
        // 发送帧
        sendFrame(frame);
        // 等待确认消息
        unsigned long startTime = millis();
        while (millis() - startTime < timeout)
        {
            if (receive->receiveACK(currentAddress,frame.destinationAddress[0]))
            {
                Serial.println("Successfully receive ACK");
                return;
            }
        }
    }
    // 如果没有收到确认消息，打印一个错误消息
    Serial.println("Did not receive ACK");
    handle_no_ack(frame, currentAddress);
}

void Send::handle_no_ack(Frame &frame, const char currentAddress)
{
    // 获取目的地址下标
    int destinationNode = getAddressIndex(frame.destinationAddress);
    Frame TopologyChangeFrame; //准备更新拓扑结构的帧
    String TopologyString = ""; // 准备拓扑帧中的数据
    switch (frame.frameType)
    {
        // 对于请求帧， 如果没有收到ACK，就把主节点删掉
        case REQUEST_FRAME:
            Serial.println("Delete the master node");
            Topology[0] = false;
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
                    sendNeedACK(TopologyChangeFrame, 3, 2000, currentAddress);
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
                    sendNeedACK(TopologyChangeFrame, 3, 2000, currentAddress);
                }
            }
            break;    
        default:
            Serial.println("Error: No such frame type");
            break;
    }
}

int Send::getAddressIndex(char address) {
    switch (address) {
        case 's':
            return 1;
        case 'b':
            return 2;
        case 't':
            return 3;
        default:
            return 0;
    }
}