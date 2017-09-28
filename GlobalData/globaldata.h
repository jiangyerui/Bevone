#ifndef GLOBALDATA_H
#define GLOBALDATA_H
#include <QString>
#include "CanPort/canport.h"
#include "MySqlite/mysqlite.h"
#define BUFNUM  1000
#define CMD_RE_STATE    0X02//探测器回复状态
#define CMD_RE_SET      0X04//探测器回复设置
#define CMD_RE_RESET    0X06//探测器回复复位
#define CMD_RE_OFF      0XFE//探测器回复单个静音关闭成功
#define CMD_RE_ON       0XFD//探测器回复单个静音开启成功

#define CMD_SE_STATE    0X01//读取探测器状态
#define CMD_SE_SET      0X03//设置探测器
#define CMD_SE_RESET    0X05//复位探测器
#define CMD_SE_OFF      0X07//探测器单个静音关闭
#define CMD_SE_ON       0X08//探测器全部静音开启
#define CMD_SE_ALLOFF   0X09//探测器全部静音关闭
#define CMD_SE_ALLON    0X0A//探测器全部静音开启
#define CMD_SE_CHECK    0X0B//探测器自检
//module status
#define NORMAL 0x00//正常
#define ALARM  0x01//报警
#define ERROR  0x02//故障
#define DROP   0x03//掉线


#define NETNUM  3  //网络个数
#define IDNUM   1024//节点个数
//2017.7.13修改
#define CMDEXENUM   1100//命令缓存
#define SERIALNUM   20//模块序列号

#define MODULE_CUR  2//漏电
#define MODULE_TEM  3//温度
#define MODULE_ELE  4//电弧
#define MODULE_PYR  5//热解粒子
#define MODULE_LIM  6//限流式保护器
#define MODULE_LIN  7//线型感温

#define CAN_CMD  0

#define ALLID 0


extern bool g_printType;
extern bool g_smsType;
extern QString g_hostModel;
extern int g_haveCmd;
extern uint idMax;
extern uint netMax;
extern uint cmdMax;
extern bool g_login;
extern bool g_resetCmd;

//struct can_frame {
//	  canid_t can_id;  /* 32 bit CAN_ID + EFF/RTR/ERR flags */
//	  __u8    can_dlc; /* frame payload length in byte (0 .. CAN_MAX_DLEN) */
//	  __u8    data[CAN_MAX_DLEN] __attribute__((aligned(8)));
//};

//节点状态
struct Module{
    bool used;    //是否存在
    uchar net;
    uchar id;
    uchar type;
    uchar flag;    //状态标志
    uchar alarmTem;//温度阈值
    uchar temData; //温度适时
    uint rtData;   //实时数据
    uint baseData; //固有漏电
    uint alarmData;//报警阈值

    uint alarmTemSet; //温度报警值
    uint alarmDataSet;//漏电报警值
    bool alaTemLock;  //温度报警锁定值
    bool alaDataLock; //漏电报警锁定值

    bool dropFlag;
    bool alarmFlag;
    bool errorFlag;
    bool normalFlag;

    bool insertDrop;
    bool insertAlarm;
    bool insertError;
    bool insertNormal;
    bool sent;
};

extern Module mod[NETNUM][IDNUM];
//执行命令缓存
struct Exe_Cmd
{
    bool dropped;
    uint sendTime;
    uchar needExe;
    struct can_frame canFrame;
};
extern Exe_Cmd exeCmd[NETNUM][CMDEXENUM];
struct CmdNum{
    int cmdNum;
};
extern CmdNum cmdNum[NETNUM];
//发送缓存
typedef struct can_frame CanFrame;

//ZigBee数据结构
struct SerialExeCmd
{
    uint id;
    uint net;
    uint cmd;
    uint lon;
    bool dropped;
    uint sendTime;
    uchar needExe;
    uchar data[15];
};
struct SerialCmdNum{
    int CmdNum;
};
extern SerialCmdNum serialCmdNum[NETNUM];
extern SerialExeCmd serialExeCmd[NETNUM][CMDEXENUM];

class GlobalData
{
public:

    GlobalData();
    static void initCmdType(QString type);
    static void initData();
    static void addCmd(int net, uint id, uint cmd, uchar *data, uchar lon);
    static void deleteCmd(int net, uint id, uint cmd);
    static void addCmdSerial(uint net, uint id, uint cmd, uint lon);
    static void deleteCmdSerial(uint net, uint id, uint cmd);
};

#endif // GLOBALDATA_H
