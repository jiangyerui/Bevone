#include "globaldata.h"
#include <QDebug>

//#define debug
//#define test

int CurNet ;
bool g_modTemp[3]={0,0,0};
bool g_modLeak[3]={0,0,0};
setNodeData leakData[3];
setNodeData tempData[3];
bool g_login = false;
bool g_printType = false;
bool g_smsType = false;
QString g_hostModel = "00";
int g_cmdNum  = 0;
bool g_resetCmd[3]={0,0,0};
bool g_modSetCmd[3]={0,0,0};
uint idMax = 0;
uint netMax = 0;
uint cmdMax = 0;
uint idNum = 1;
uint modNum[3][1024];
Exe_Cmd exeCmd[NETNUM][CMDEXENUM];
Module mod[NETNUM][IDNUM];
CmdNum cmdNum[NETNUM];

bool g_powerStatus = false;
bool g_bpowerStatus = false;
SerialCmdNum serialCmdNum[NETNUM];
SerialExeCmd serialExeCmd[NETNUM][CMDEXENUM];
QList<CanFrame> canCmd_1_List;
QList<CanFrame> canCmd_2_List;
GlobalData::GlobalData()
{

}


void GlobalData::initCmdType(QString type)
{
    //在没有数据库的情况下默认可以使用net1,可以配接32个点
    //qDebug()<<"type = "<<type;
    if(type.isEmpty() == TRUE)
    {
        netMax = 2;
        idMax = 32;
    }
    else
    {
        //网络数
        if(type.mid(3,1).toInt() == 1)
        {
            netMax = 2;
        }
        else
        {
            netMax = 3;
        }

        switch (type.mid(4,1).toInt()) {
        case 1:
            idMax = 32;
            break;
        case 2:
            idMax = 64;
            break;
        case 3:
            idMax = 128;

            break;
        case 4:
            idMax = 256;
            break;
        case 5:
            idMax = 512;
            break;
        case 6:
            idMax = 1024;
            break;
        default:
            break;
        }
    }

    uchar temp[1];

    for(uint net = 1;net < netMax; net++)
    {
        for(uint id = 1;id <= idMax; id++)
        {
            //初始化can巡检命令
            temp[0] = CMD_SE_STATE;
            MySqlite db;
            if(db.getNodeNum(net ,id))
            {
                //qDebug()<<"net = "<<net<<"  id = "<<id;
                addCmd(net,id,temp[0],temp,1);

            }
            //初始化ZigBee巡检命令
            //addCmdSerial(net,id,CMD_SE_STATE,15);
        }
    }


}

//#define TEST

void GlobalData::initData()
{
    for(int net = 1;net < NETNUM;net++)
    {
        for(int i = 0;i< CMDEXENUM;i++)
        {
            exeCmd[net][i].needExe = 0;
            exeCmd[net][i].canFrame.can_id  = 0;
            exeCmd[net][i].canFrame.can_dlc = 0;
            exeCmd[net][i].sendTime = 0;
            exeCmd[net][i].curTime  = 0;
            exeCmd[net][i].dropped  = false;
            for(int j = 0 ;j < 8;j++)
            {
                exeCmd[net][i].canFrame.data[j] = 0;
            }
        }
        cmdNum[net].cmdNum = 1;
    }

    for(int net = 1;net < NETNUM;net++)
    {
        for(int id = 1;id < IDNUM;id++)
        {
            mod[net][id].used = false;
            mod[net][id].id = 0;
            mod[net][id].net = 0;
            mod[net][id].type = 0;
#ifdef TEST
            mod[net][id].used = true;
            mod[net][id].id = id;
            mod[net][id].net = 1;
            mod[net][id].type = 2;

#endif

            mod[net][id].alarmTem = 0;
            mod[net][id].temData  = 0;
            mod[net][id].rtData   = 0;
            mod[net][id].baseData = 0;
            mod[net][id].alarmData= 0;
            mod[net][id].alarmTemSet  = 0;
            mod[net][id].alarmDataSet = 0;
            mod[net][id].alarmFlag = FALSE;
            mod[net][id].dropFlag  = FALSE;
            mod[net][id].normalFlag = FALSE;
            mod[net][id].errorFlag  = FALSE;
            mod[net][id].insertAlarm = FALSE;
            mod[net][id].insertDrop  = FALSE;
            mod[net][id].insertError = FALSE;
            mod[net][id].insertNormal = FALSE;
            mod[net][id].alaTemLock   = FALSE;
            mod[net][id].alaDataLock  = FALSE;
            mod[net][id].dropTimes = 0;
            mod[net][id].leakTimes = 0;
            mod[net][id].tempTimes = 0;
        }
    }
}

void GlobalData::addCmd(int net,uint id,uint cmd, uchar *data, uchar lon)
{
    if(net == 1)
    {
        CanFrame canFrame;
        canFrame.can_dlc = lon;
        canFrame.can_id  = id;
        canFrame.data[0] = cmd;
        for(int i = 1;i<lon;i++)
        {
            canFrame.data[i] = data[i];
        }
        canCmd_1_List.append(canFrame);
    }
    else if(net == 2)
    {
        CanFrame canFrame;
        canFrame.can_dlc = lon;
        canFrame.can_id  = id;
        canFrame.data[0] = cmd;
        for(int i = 1;i<lon;i++)
        {
            canFrame.data[i] = data[i];
        }
        canCmd_2_List.append(canFrame);
    }

}

void GlobalData::deleteCmd(int net,  uint id, uint cmd)
{
    for(int i = 0;i<CMDEXENUM;i++)
    {
        int  needExe = exeCmd[net][i].needExe;
        uint can_id  = exeCmd[net][i].canFrame.can_id;
        uint can_cmd = exeCmd[net][i].canFrame.data[0];

        if(needExe == 1 && can_id == id && can_cmd == cmd)
        {
            exeCmd[net][i].needExe = 0;
            exeCmd[net][i].canFrame.can_id  = 0;
            exeCmd[net][i].canFrame.can_dlc = 0;
            for(int j = 0 ;j < 8;j++)
            {
                exeCmd[net][i].canFrame.data[j] = 0;
            }
        }
    }
}

void GlobalData::addCmdSerial(uint net, uint id, uint cmd,uint lon)
{
//    if(serialCmdNum[net].CmdNum == CMDEXENUM)
//        serialCmdNum[net].CmdNum = 1;

//    serialExeCmd[net][serialCmdNum[net].CmdNum].needExe = 1;
//    serialExeCmd[net][serialCmdNum[net].CmdNum].id = id;
//    serialExeCmd[net][serialCmdNum[net].CmdNum].net = net;
//    serialExeCmd[net][serialCmdNum[net].CmdNum].cmd = cmd;
//    serialExeCmd[net][serialCmdNum[net].CmdNum].lon = lon;
//    for(uint i = 0;i<lon;i++)
//    {
//        serialExeCmd[net][serialCmdNum[net].CmdNum].data[i] = data[i];
//    }
//    serialCmdNum[net].CmdNum++;
    uchar data[15];
    memset(data,0,sizeof(data));

    data[0]  = 0xAA;
    data[1]  = 0x00;
    data[2]  = CMD_SE_STATE;
    if(id <= 255 )
    {
        data[3]  = 0x00;
        data[4]  = id;
    }
    else
    {
        data[3]  = id >> 8;
        data[4]  = id & 0xFF;
    }

    data[5]  = 0x08;
    data[6]  = 0x00;
    data[7]  = 0x00;
    data[8]  = 0x00;
    data[9]  = 0x00;
    data[10] = 0x00;
    data[11] = 0x00;
    data[12] = 0x00;
    data[13] = 0x00;
    data[14] = 0x00;
    for(int i = 2;i<14;i++)
    {
        data[14] += data[i];
    }

    serialExeCmd[net][id].needExe = 1;
    serialExeCmd[net][id].id  = id;
    serialExeCmd[net][id].net = net;
    serialExeCmd[net][id].cmd = cmd;
    serialExeCmd[net][id].lon = lon;
    for(uint i = 0;i<lon;i++)
    {
        serialExeCmd[net][id].data[i] = data[i];
    }
}

void GlobalData::deleteCmdSerial(uint net, uint id, uint cmd)
{
    for(int i = 0;i<CMDEXENUM;i++)
    {
        uint needExe = serialExeCmd[net][i].needExe ;
        uint s_id  = serialExeCmd[net][i].id;
        uint s_cmd = serialExeCmd[net][i].cmd;
        uint s_net = serialExeCmd[net][i].net;

        if(needExe == 1 && s_id == id && s_cmd == cmd && s_net == net)
        {
            serialExeCmd[net][i].needExe = 0;
            serialExeCmd[net][i].id  = 0;
            serialExeCmd[net][i].net = 0;
            serialExeCmd[net][i].cmd = 0;
            serialExeCmd[net][i].lon = 0;
            for(int j = 0 ;j <15;j++)
            {
                serialExeCmd[net][i].data[j] = 0;
            }
        }
    }    
}

