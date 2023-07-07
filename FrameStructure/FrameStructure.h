#ifndef FrameStructure_h
#define FrameStructure_h

#include "Arduino.h"
using namespace std;

// 帧标识
enum FrameType {
  REQUEST_FRAME = 1,
  HEARTBEAT_FRAME,
  RESPONSE_FRAME,
  MASTER_NODE_FRAME,
  DATA_FRAME,
  RETRANSMISSION_FRAME,
  TOPOLOGY_CHANGE_FRAME
};

// 请求帧数据
enum RequestFrameData {
  NETWORK_JOIN_REQUEST = 1,
  NETWORK_LEAVE_REQUEST
};

// 响应帧数据
enum ResponseFrameData {
  REQUEST_RESPONSE = 1,
  HEARTBEAT_RESPONSE,
  DATA_RESPONSE,
  MASTERJOIN_RESPONSE
};

// 帧结构
class Frame {
public:
    char senderAddress[2];
    char receiverAddress[2];
    char destinationAddress[2];
    FrameType frameType;
    union {
      RequestFrameData requestData;
      ResponseFrameData responseData;
      char TopologyChangeData[5];
      char dataFrameData[10];  // 数据帧的数据
    } frameData;
    int dataLength;
    void initDataFrame(const char* sender, const char* receiver, const char* destination, FrameType type, String& data);
    void initRequestFrame(const char* sender, const char* receiver, const char* destination, FrameType type, RequestFrameData data);
    void initResponseFrame(const char* sender, const char* receiver, const char* destination, FrameType type, ResponseFrameData data);
    void initTopologyChangeFrame(const char* sender, const char* receiver, const char* destination, FrameType type, String& data);
    void initHeartbeatFrame(const char* sender, const char* receiver, const char* destination, FrameType type);
    void initRetransmissionFrame(const char* sender, const char* receiver, const char* destination, FrameType type);
    void initMasterNodeFrame(const char* sender, const char* receiver, const char* destination, FrameType type);
};

#endif
