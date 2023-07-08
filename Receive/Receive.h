#ifndef Receive_h
#define Receive_h

#include "Arduino.h"
#include <SPI.h>
#include <LoRa.h>
#include <FrameStructure.h>
#include <Send.h>
class Send;// 前置声明

class Receive {
    public:
        String processPacket(const char currentAddress);
        bool receiveACK(const char currentAddress);
    private:
        Send* sender;
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
        //处理主节点可服务帧响应
        void processMasterNodeJoinResponse(const String& ReceiveData);
        // 处理主节点可服务帧
        void processMasterNodeJoinRequest(const String& ReceiveData, const char currentAddress);
        // 处理数据帧
        String processDataFrame(const String& ReceiveData, const char destinationAddress, const char receiveAddress);
        // 处理告知重传帧
        void processRetransmissionFrame(const String& ReceiveData);
        // 处理拓扑变化帧
        void processTopologyChangeFrame(const String& ReceiveData);
};

#endif