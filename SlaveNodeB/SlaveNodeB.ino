#include <SPI.h>
#include <LoRa.h>
#include <FrameStructure.h>
#include <Receive.h>
#include <Send.h>

using namespace std;
String data;
// 创建一个Receive类的实例
Receive receiver;
// 创建一个Send类的实例
extern Send* sender;
// 当前节点
char currentNode = 'b';

void setup() {
    Serial.begin(9600);
    while (!Serial);

    Serial.println("LoRa SlaveNodeB");

    if (!LoRa.begin(410E6)) {
        Serial.println("Starting LoRa failed!");
        while (1);
    }
}

void loop(){
    receiver.processPacket(currentNode);
    data = getData();
    if (data != "")
    {
        if (Topology[0])
        {
            // 从节点B上线请求
            if (data == "login")
            {
                login();
            }
            // 从节点B下线请求
            else if (data == "logout")
            {
                logout();
            }
        }
        else
        {
            Serial.println("MasterNode is offline");
        }
    }
}

// 从节点上网请求函数
void login()
{
    Serial.println("SlaveNode Login");
        // 组帧
    Frame LoginRequestFrame;
    if (Topology[1])
    {
        LoginRequestFrame.initRequestFrame("b", "s", "m", REQUEST_FRAME, NETWORK_JOIN_REQUEST);
        sender->sendNeedACK(LoginRequestFrame, 3, 2000, currentNode);
    }
    else
    {
        LoginRequestFrame.initRequestFrame("b", "m", "m", REQUEST_FRAME, NETWORK_JOIN_REQUEST);
        sender->sendNeedACK(LoginRequestFrame, 3, 2000, currentNode);
    }
}

// 从节点下线请求函数
void logout()
{
    Serial.println("SlaveNode Logout");
    // 组帧
    Frame LogoutRequestFrame;
    if (Topology[1])
    {
        LogoutRequestFrame.initRequestFrame("b", "s", "m", REQUEST_FRAME, NETWORK_LEAVE_REQUEST);
        sender->sendNeedACK(LogoutRequestFrame, 3, 2000, currentNode);
    }
    else
    {
        LogoutRequestFrame.initRequestFrame("b", "m", "m", REQUEST_FRAME, NETWORK_LEAVE_REQUEST);
        sender->sendNeedACK(LogoutRequestFrame, 3, 2000, currentNode);
    }
}

// 从串口接收数据
String getData()
{
    data = "";
    if (Serial.available())
    {
        data = Serial.readString();
    }
    return data;
}