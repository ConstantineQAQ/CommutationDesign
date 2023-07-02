#ifndef Send_h
#define Send_h

#include "Arduino.h"
#include <SPI.h>
#include <LoRa.h>
#include <FrameStructure.h>
using namespace std;

class Send {
    public:
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
};

#endif
