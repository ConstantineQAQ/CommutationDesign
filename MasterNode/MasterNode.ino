#include <SPI.h>
#include <LoRa.h>
#include <FrameStructure.h>
#include <Receive.h>
#include <Send.h>

using namespace std;

// 白名单设置
/**
 * s:中继节点一号
 * b:中继节点二号
 * t:终端节点
*/
extern char white[20];
extern bool Topology[4];
String data;
char currentNode = 'm';

// 创建一个Receive类的实例
Receive receiver;
// 创建一个Send类的实例
Send sender;

void setup() {
    LoRa.setPins(A3);
    Serial.begin(9600);
    while (!Serial);
    Serial.println("LoRa MasterNode");
    if (!LoRa.begin(410E6)) {
        Serial.println("Starting LoRa failed!");
        while (1);
    }
    initMasterNode();
}

void loop(){
    // 发送数据
    data = getData();
    // 接收数据并处理
    receiver.processPacket(currentNode);
    if (data != "")
    {
        // 组帧发送数据帧
        Frame DataFrame;
        DataFrame.initDataFrame("m", "s", "t", DATA_FRAME, data);
        sender.sendDataFrame(DataFrame);
    }
}

void initMasterNode()
{
    // 初始化拓扑
    Topology[0] = true;
    // 初始化主节点可服务帧
    Frame MasterOnlineframe;
    //组帧
    // test
    MasterOnlineframe.initMasterNodeFrame("m","a","a",MASTER_NODE_FRAME);
    sender.sendMasterNodeLoginFrame(MasterOnlineframe);
}

String getData()
{
    data = "";
    if (Serial.available())
    {
        data = Serial.readString();
    }
    return data;
}