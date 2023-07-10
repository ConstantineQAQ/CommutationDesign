#include <SPI.h>
#include <LoRa.h>
#include <FrameStructure.h>
#include <Receive.h>
#include <Send.h>

using namespace std;
// 增加频段宏定义
#define Frency 410E6

// 白名单设置
/**
 * s:中继节点一号
 * b:中继节点二号
 * t:终端节点
*/
extern const char* white[3];
extern bool Topology[4];
String data;
char currentNode = 'm';

// 创建一个Receive类的实例
Receive receiver;
// 创建一个Send类的实例
extern Send* sender;
unsigned long lastSendTime = 0; // 上次发送心跳帧的时间
const unsigned long sendInterval = 20000; // 发送心跳帧的间隔，单位是毫秒

void send_to_terminal(FrameType frame_type, String data = "");

void setup() {
    LoRa.setPins(A3);
    Serial.begin(9600);
    while (!Serial);
    Serial.println("LoRa MasterNode");
    if (!LoRa.begin(410E6)) 
    {
        Serial.println("Starting LoRa failed!");
        while (1);
    }
    // 设置扩频因子
    // LoRa.setSpreadingFactor();
}

void loop(){
    // 需要发送的数据
    data = getData();
    // 接收数据并处理
    receiver.processPacket(currentNode);
    // 主节点一旦上线，就开始不断向拓扑内的所有节点发送心跳帧
    // 使用非阻塞的方式发送心跳帧
    if (Topology[0])
    {
        // 发送心跳帧
        unsigned long currentTime = millis(); // 获取当前时间
        // 如果距离上次发送心跳帧的时间已经超过了设定的间隔
        if (currentTime - lastSendTime >= sendInterval) {
            // 向拓扑内所有节点发送心跳帧
            Frame HeartbeatFrame;
            for (int i = 1; i < 4; i++)
            {
                if (Topology[i])
                {
                    // 发送给中继的心跳帧
                    if (i == 1)
                    {
                        HeartbeatFrame.initHeartbeatFrame("m", white[i-1], white[i-1], HEARTBEAT_FRAME);
                    }
                    // 发送给中继B的心跳帧
                    else if (i == 2)
                    {
                        HeartbeatFrame.initHeartbeatFrame("m", Topology[i-1] == 1 ? white[i-2] : white[i-1], white[i-1], HEARTBEAT_FRAME);
                    }
                    // 发送给终端的心跳帧
                    else if (i == 3)
                    {
                        send_to_terminal(HEARTBEAT_FRAME);
                    }
                    sender->sendNeedACK(HeartbeatFrame, 3, 1000, currentNode);
                }
            }
            // 更新上次发送心跳帧的时间
            lastSendTime = currentTime;
        }
    }
    if (data != "")
    {
        if (data == "init")
        {
            initMasterNode();
            return;
        }
        // 主节点上线
        if (Topology[0])
        {
            // 要发送数据前先打印拓扑
            Serial.println("Topology: ");
            for(int i = 0; i < 4; i++)
            {
                Serial.print(Topology[i]);
            }
            send_to_terminal(DATA_FRAME, data);
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
    for (int i = 1; i < 4; i++)
    {
        if (i == 1)
        {
            MasterOnlineframe.initMasterNodeFrame("m",white[i-1],white[i-1],MASTER_NODE_FRAME);
        }
        else if (i == 2)
        {
            MasterOnlineframe.initMasterNodeFrame("m", Topology[i-1] == 1 ? white[i-2] : white[i-1], white[i-1], MASTER_NODE_FRAME);
        }
        else if (i == 3)
        {
            send_to_terminal(MASTER_NODE_FRAME);
        }
        sender->sendNeedACK(MasterOnlineframe, 3, 2000, currentNode);
        // 为了防止帧丢失，每发送一帧就延时一秒
        delay(1000);
    }
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

void send_to_terminal(FrameType frame_type, String data = "")
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
            // 组帧发送数据帧
            Frame frame;
            switch (frame_type)
            {
                case DATA_FRAME:
                    frame.initDataFrame("m", "s", "t", frame_type, data);
                    break;
                case HEARTBEAT_FRAME:
                    frame.initHeartbeatFrame("m", "s", "t", frame_type);
                    break;
                case MASTER_NODE_FRAME:
                    frame.initMasterNodeFrame("m", "s", "t", frame_type);
                    break;
                default:
                    break;
            }
            sender->sendNeedACK(frame, 3, 2000, currentNode);
        }
        else if (count == 1)
        {
            // 判断哪一个中继节点在线
            Frame frame;
            switch (frame_type)
            {
                case DATA_FRAME:
                    frame.initDataFrame("m", Topology[1] == 1 ? "s" : "b", "t", frame_type, data);
                    break;
                case HEARTBEAT_FRAME:
                    frame.initHeartbeatFrame("m", Topology[1] == 1 ? "s" : "b", "t", frame_type);
                    break;
                case MASTER_NODE_FRAME:
                    frame.initMasterNodeFrame("m", Topology[1] == 1 ? "s" : "b", "t", frame_type);
                    break;
                default:
                    break;
            }
            sender->sendNeedACK(frame, 3, 2000, currentNode);
        }
    }
    // 如果没有一个中继节点在线，则直接发送数据响应给对应节点
    else
    {
        // 组帧发送数据帧
        Frame frame;
        switch (frame_type)
        {
            case DATA_FRAME:
                frame.initDataFrame("m", "t", "t", frame_type, data);
                break;
            case HEARTBEAT_FRAME:
                frame.initHeartbeatFrame("m", "t", "t", frame_type);
                break;
            case MASTER_NODE_FRAME:
                frame.initMasterNodeFrame("m", "t", "t", frame_type);
                break;
            default:
                break;
        }
        sender->sendNeedACK(frame, 3, 2000, currentNode);
    }   
}