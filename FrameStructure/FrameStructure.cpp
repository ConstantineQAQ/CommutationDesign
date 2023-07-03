#include "FrameStructure.h"

// 初始化帧
void Frame::initDataFrame(const char* sender, const char* receiver, const char* destination, FrameType type, String& data) {
  strncpy(senderAddress, sender, sizeof(senderAddress));
  strncpy(receiverAddress, receiver, sizeof(receiverAddress));
  strncpy(destinationAddress, destination, sizeof(destinationAddress));
  frameType = type;
  strncpy(frameData.dataFrameData, data.c_str(), sizeof(frameData.dataFrameData)); 
  dataLength = sizeof(*this);
}


// 初始化请求帧
void Frame::initRequestFrame(const char* sender, const char* receiver, const char* destination, FrameType type, RequestFrameData data) {
  strncpy(senderAddress, sender, sizeof(senderAddress));
  strncpy(receiverAddress, receiver, sizeof(receiverAddress));
  strncpy(destinationAddress, destination, sizeof(destinationAddress));
  frameType = type;
  frameData.requestData = data;
  dataLength = sizeof(*this);
}

// 初始化响应帧
void Frame::initResponseFrame(const char* sender, const char* receiver, const char* destination, FrameType type, ResponseFrameData data) {
  strncpy(senderAddress, sender, sizeof(senderAddress));
  strncpy(receiverAddress, receiver, sizeof(receiverAddress));
  strncpy(destinationAddress, destination, sizeof(destinationAddress));
  frameType = type;
  frameData.responseData = data;
  dataLength = sizeof(*this);
}

// 初始化拓扑变化帧
void Frame::initTopologyChangeFrame(const char* sender, const char* receiver, const char* destination, FrameType type, String& data) {
  strncpy(senderAddress, sender, sizeof(senderAddress));
  strncpy(receiverAddress, receiver, sizeof(receiverAddress));
  strncpy(destinationAddress, destination, sizeof(destinationAddress));
  frameType = type;
  strncpy(frameData.dataFrameData, data.c_str(), sizeof(frameData.dataFrameData)); 
  dataLength = sizeof(*this);
}

// 初始化心跳帧
void Frame::initHeartbeatFrame(const char* sender, const char* receiver, const char* destination, FrameType type) {
  strncpy(senderAddress, sender, sizeof(senderAddress));
  strncpy(receiverAddress, receiver, sizeof(receiverAddress));
  strncpy(destinationAddress, destination, sizeof(destinationAddress));
  frameType = type;
  dataLength = sizeof(*this);
}

// 初始化重传帧
void Frame::initRetransmissionFrame(const char* sender, const char* receiver, const char* destination, FrameType type) {
  strncpy(senderAddress, sender, sizeof(senderAddress));
  strncpy(receiverAddress, receiver, sizeof(receiverAddress));
  strncpy(destinationAddress, destination, sizeof(destinationAddress));
  frameType = type;
  dataLength = sizeof(*this);
}

// 初始化主节点帧
void Frame::initMasterNodeFrame(const char* sender, const char* receiver, const char* destination, FrameType type) {
  strncpy(senderAddress, sender, sizeof(senderAddress));
  strncpy(receiverAddress, receiver, sizeof(receiverAddress));
  strncpy(destinationAddress, destination, sizeof(destinationAddress));
  frameType = type;
  dataLength = sizeof(*this);
}
