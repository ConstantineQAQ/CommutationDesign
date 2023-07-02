#include <SPI.h>
#include <LoRa.h>
#include <FrameStructure.h>
#include <Receive.h>
#include <Send.h>

using namespace std;
String ReceivedData;
String data;
// 创建一个Receive类的实例
Receive receiver;
// 创建一个Send类的实例
Send sender;
// 当前节点
char currentNode = 't';

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
            sender.sendResponseFrame(RequestResponseFrame);
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

// 从节点上网请求函数
void login()
{
    Serial.println("Terminal Login");
        // 组帧
    Frame LoginRequestFrame;
    LoginRequestFrame.initRequestFrame("t", "s", "m", REQUEST_FRAME, NETWORK_JOIN_REQUEST);
    sender.sendRequestFrame(LoginRequestFrame);
}

// 从节点下线请求函数
void logout()
{
    Serial.println("Terminal Logout");
    // 组帧
    Frame LogoutRequestFrame;
    LogoutRequestFrame.initRequestFrame("t", "s", "m", REQUEST_FRAME, NETWORK_LEAVE_REQUEST);
    sender.sendRequestFrame(LogoutRequestFrame);
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