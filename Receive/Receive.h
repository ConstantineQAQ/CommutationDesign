#ifndef Receive_h
#define Receive_h

#include "Arduino.h"
#include <SPI.h>
#include <LoRa.h>
#include <FrameStructure.h>
#include <Send.h>

class Receive {
    public:
        String processPacket(const char currentAddress);
    private:
        Send sender;
        // 处理请求帧
        void processRequestFrame(const String& ReceiveData);
        // 处理入网请求
        void processNetworkJoinRequest(const String& ReceiveData);
        // 处理退网请求
        void processNetworkLeaveRequest(const String& ReceiveData);
        // 处理心跳帧
        void processHeartbeatFrame(const String& ReceiveData);
        // 处理响应帧
        void processResponseFrame(const String& ReceiveData);
        // 处理请求响应
        void processRequestResponse(const String& ReceiveData);
        // 处理心跳响应
        void processHeartbeatResponse(const String& ReceiveData);
        // 处理数据响应
        void processDataResponse(const String& ReceiveData);
        //处理主节点可服务帧响应
        void processMasterNodeJoinResponse(const String& ReceiveData);
        // 处理主节点可服务帧
        void processMasterNodeJoinRequest(const String& ReceiveData);
        // 处理数据帧
        String processDataFrame(const String& ReceiveData, const char destinationAddress, const char receiveAddress);
        // 处理告知重传帧
        void processRetransmissionFrame(const String& ReceiveData);
        // 处理拓扑变化帧
        void processTopologyChangeFrame(const String& ReceiveData);
        // 处理没有从节点的拓扑变化帧
        void TopologyNoSlave();
        // 处理只有一个从节点的拓扑变化帧
        void TopologyOneSlave();
        // 处理有两个从节点的拓扑变化帧
        void TopologyTwoSlave();
        // 处理有三个从节点的拓扑变化帧
        void TopologyNoTerminal();

};

#endif