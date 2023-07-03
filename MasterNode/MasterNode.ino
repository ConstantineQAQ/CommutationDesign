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
char white[20] = {'s', 'b', 't'};
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
        // 打印拓扑
        Serial.println("Topology: ");
        for(int i = 0; i < 4; i++)
        {
            Serial.print(Topology[i]);
        }
        // 先判断有几个中继节点在线
        int count = 0;
        for (int i = 1; i < 3; i++)
        {
            if (Topology[i])
            {
                count++;
            }
        }
        Serial.println(count);
        // 如果有中继节点在线
        if (count)
        {
            Serial.println("condition 1");
            // 两个节点都在线
            if (count == 2)
            {
                // 组帧发送数据帧
                Frame DataFrame;
                DataFrame.initDataFrame("m", "s", "t", DATA_FRAME, data);
                sender.sendDataFrame(DataFrame);
            }
            else if (count == 1)
            {
                Serial.println("condition 2");
                // 判断哪一个中继节点在线
                if (Topology[1])
                {
                    Serial.println("condition 3");
                    // 组帧发送数据帧
                    Frame DataFrame;
                    Serial.println(white[0]);
                    DataFrame.initDataFrame("m", "s", "t", DATA_FRAME, data);
                    sender.sendDataFrame(DataFrame);
                }
                else if (Topology[2])
                {
                    // 组帧发送数据帧
                    Frame DataFrame;
                    DataFrame.initDataFrame("m", "b", "t", DATA_FRAME, data);
                    sender.sendDataFrame(DataFrame);
                }
            }
        }
        // 如果没有一个中继节点在线，则直接发送数据响应给对应节点
        else
        {
            // 组帧发送数据帧
            Frame DataFrame;
            DataFrame.initDataFrame("m", "t", "t", DATA_FRAME, data);
            sender.sendDataFrame(DataFrame);
        }    
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