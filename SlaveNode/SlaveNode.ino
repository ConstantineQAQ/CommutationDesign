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
char currentNode = 's';

void setup() {
    Serial.begin(9600);
    while (!Serial);

    Serial.println("LoRa SlaveNode");

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
        // 从节点上线请求
        if (data == "login")
        {
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
    Serial.println("SlaveNode Login");
        // 组帧
    Frame LoginRequestFrame;
    LoginRequestFrame.initRequestFrame("s", "m", "m", REQUEST_FRAME, NETWORK_JOIN_REQUEST);
    sender->sendNeedACK(LoginRequestFrame, 3, 2000, currentNode);
}

// 从节点下线请求函数
void logout()
{
    Serial.println("SlaveNode Logout");
    // 组帧
    Frame LogoutRequestFrame;
    LogoutRequestFrame.initRequestFrame("s", "m", "m", REQUEST_FRAME, NETWORK_LEAVE_REQUEST);
    sender->sendNeedACK(LogoutRequestFrame, 3, 2000, currentNode);
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