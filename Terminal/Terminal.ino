#include <SPI.h>
#include <LoRa.h>
#include <FrameStructure.h>
#include <Receive.h>
#include <Send.h>

using namespace std;
String ReceivedData;
String data;
// 白名单
extern const char* white[20];
// 创建一个Receive类的实例
Receive receiver;
// 创建一个Send类的实例
extern Send* sender;
// 当前节点
char currentNode = 't';
extern bool Topology[4];

void setup() 
{
    Serial.begin(9600);
    while(!Serial);
    pinMode(5, OUTPUT);
    Serial.println("LoRa Terminal");

    if (!LoRa.begin(410E6)) 
    {
        Serial.println("Starting LoRa failed!");
        while (1);
    }
}

void loop()
{
    ReceivedData = receiver.processPacket(currentNode);
    if (ReceivedData != "")
    {
        Serial.println(ReceivedData);
        // 从节点接收到请求
        if (ReceivedData == "move")
        {
            Serial.println("Received move Request");
            digitalWrite(5, HIGH);
            delay(1000);
            digitalWrite(5, LOW);
            delay(1000);
            // 组帧
            Frame RequestResponseFrame;
            RequestResponseFrame.initResponseFrame("t", "s", "m", RESPONSE_FRAME, DATA_RESPONSE);
            sender->sendResponseFrame(RequestResponseFrame);
            ReceivedData = ""; // 清空接收到的数据
        }
    }

    
    data = getSerialData();
    if (data != "")
    {
        // 从节点上线请求
        if (data == "login"){
            login();
        }
        // 从节点下线请求
        else if (data == "logout")
        {
            logout();
        }
    }
}

void sendRequest(RequestFrameData requestType, const String &message)
{
    // 先判断有几个中继节点在线
    int count = 0;
    for (int i = 1; i < 3; i++)
    {
        if (Topology[i])
        {
            count++;
        }
    }
    // 如果有中继节点在线
    if (count)
    {
        // 两个节点都在线
        if (count == 2)
        {
            Serial.println(message);
            // 组帧
            Frame RequestFrame;
            RequestFrame.initRequestFrame("t", "s", "m", REQUEST_FRAME, requestType);
            sender->sendNeedACK(RequestFrame, 3, 2000, currentNode);
        }
        else if (count == 1)
        {
            // 判断哪一个中继节点在线
            if (Topology[1])
            {
                Serial.println(message);
                // 组帧
                Frame RequestFrame;
                RequestFrame.initRequestFrame("t", "s", "m", REQUEST_FRAME, requestType);
                sender->sendNeedACK(RequestFrame, 3, 2000, currentNode);
            }
            else if (Topology[2])
            {
                Serial.println(message);
                // 组帧
                Frame RequestFrame;
                RequestFrame.initRequestFrame("t", "b", "m", REQUEST_FRAME, requestType);
                sender->sendNeedACK(RequestFrame, 3, 2000, currentNode);
            }
        }
    }
    // 如果没有一个中继节点在线，则直接发送数据响应给对应节点
    else
    {
        Serial.println(message);
        // 组帧
        Frame RequestFrame;
        RequestFrame.initRequestFrame("t", "m", "m", REQUEST_FRAME, requestType);
        sender->sendNeedACK(RequestFrame, 3, 2000, currentNode);
    }
}

// 从节点上网请求函数
void login()
{
    sendRequest(NETWORK_JOIN_REQUEST, "Terminal Login");
}

// 从节点下线请求函数
void logout()
{
    sendRequest(NETWORK_LEAVE_REQUEST, "Terminal Logout");
}

String getSerialData()
{
    // TODO
    data = "";
    if (Serial.available())
    {
        data = Serial.readString();
    }
    return data;
}
