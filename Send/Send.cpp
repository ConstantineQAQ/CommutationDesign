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
    Serial.println("***************************");
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
