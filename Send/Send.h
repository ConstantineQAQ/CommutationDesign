#ifndef Send_h
#define Send_h

#include "Arduino.h"
#include <SPI.h>
#include <LoRa.h>
#include <FrameStructure.h>
#include <Receive.h>

using namespace std;
extern bool Topology[4];
extern const char* white[3];
class Receive;// 前置声明

class Send {
    public:
        // 判断收否有接受信息,初始化Receive
        Receive* receive;  // 使用指针而不是对象
        // 从串口中读取的数据
        String val;
        // 发送数据帧
        void sendDataFrame(Frame& frame);
        // 发送请求帧
        void sendRequestFrame(Frame& frame);
        // 发送心跳帧
        void sendHeartbeatFrame(Frame& frame);
        // 发送响应帧
        void sendResponseFrame(Frame& frame);
        // 发送主节点上线帧
        void sendMasterNodeLoginFrame(Frame& frame);
        // 发送告知重传帧
        void sendRetransmissionFrame(Frame& frame);
        // 发送拓扑变化帧
        void sendTopologyChangeFrame(Frame& frame);
        // 发送需要ACk的帧
        void sendNeedACK(Frame& frame, int retryTimes, int timeout, const char currentAddress);
        // 发送帧
        void sendFrame(Frame& frame);
        // 处理错误
        void handle_no_ack(Frame& frame, const char currentAddress);
        // 获取下标
        int getAddressIndex(char address);
};

#endif
