#include "vqserialtxthread.h"

vQObjectTx::vQObjectTx(QObject *parent) : QObject(parent)
{
    this->vTimerOutTime = 100;
    this->vQTimer.setTimerType(Qt::PreciseTimer);
    connect(&this->vQTimer, &QTimer::timeout, this, &vQObjectTx::vTxTimeOut);
}
vQObjectTx::~vQObjectTx(void)
{

};
//打开接收的信号与槽，方便切换
void vQObjectTx::vConnectTx(void)
{
    connect(this, &vQObjectTx::vTxMessage,
            this->vSerial, &vSerialPort::vWriteData,
            Qt::QueuedConnection);
}
//断开接收的信号与槽，方便切换
void vQObjectTx::vDisConnectTx(void)
{
    disconnect(this,&vQObjectTx::vTxMessage,
            this->vSerial,&vSerialPort::vWriteData);
}
void vQObjectTx::vTimerSet(qint32 cnt)
{
    this->vTimerOutTime = cnt;
}
void vQObjectTx::vTimerStart(void)
{
    if(!this->vQTimer.isActive())
    {
        this->vQTimer.start(this->vTimerOutTime);
    }
}
void vQObjectTx::vTimerStop(void)
{
    if(this->vQTimer.isActive())
    {
         this->vQTimer.stop();
    }
}

//根据发送的模式选择发送的数据，就是发送数据槽函数，名字有点误导
void vQObjectTx::vTxTimeOut(void)
{
    if(this->vSerial!=NULL)
    {
        switch(this->vSerialTxMode)
        {
            case SerialOrd:
            {
                if((*this->textEditTx)!="")
                {
                    //源程序这里对16进制发送不进行编码处理，这里进行编码，是为了单片机能够更好的接收数据
                    if(this->vSerial->vSerialData->txHexEnable)
                    {
                        emit vTxMessage(CodeCoding((*textEditTx)+LineFeed));
                    }
                    else
                    {
                        emit vTxMessage(CodeCoding((*textEditTx)+LineFeed));
                    }
                }
            };break;
            case SerialMul:
            {
                if(vLineEditTxCtr())
                {
                    if((*this->lineEditTx)!="")
                    {
                        //源程序这里对16进制发送不进行编码处理，这里进行编码，是为了单片机能够更好的接收数据
                        if(this->vSerial->vSerialData->txHexEnable)
                        {
                            emit vTxMessage((CodeCoding(*lineEditTx)+LineFeed));
                        }
                        else
                        {
                            emit vTxMessage(CodeCoding((*lineEditTx)+LineFeed));
                        }
                    }
                }
            };break;
            case SerialAgr:
            {
                //按协议发送此处已在别处更改槽函数，此处不再配置，但是保留框架
            };break;
            default:break;
        }
    }
}
//周期性调用,切换需要发送的数据,多次发送
bool vQObjectTx::vLineEditTxCtr(void)
{
    static bool lastFindPos = 0;
    if(this->MultPleMaxCnt != nullptr)
    {
        QMap<qint32, bool*>::iterator findTx;
        findTx = LineEditMap->find(this->timerCnt+1);
        if(findTx == LineEditMap->end())
        {
            this->timerCnt = 0;
            return false;
        }
        while(!(*findTx.value()))//勾选vlineEdit的进不来
        {
            this->timerCnt++;
            qDebug() << "timerCnt" << timerCnt;
            if(this->timerCnt >= (*this->MultPleMaxCnt)) //查询一轮到头了，重新从头发送
            {
                //如果第一次查询直到最后也没有查询到，那么在此需要查询2次，定位到环形列表
                if(lastFindPos==0)
                {
                    lastFindPos = 1;
                    this->timerCnt = 0;
                    return(vLineEditTxCtr());
                }
                else
                {
                    //重新从0开始查询，仍然无值，说明整个列表无值，返回false
                    lastFindPos = 0;
                    this->timerCnt = 0;
                    return false;
                }
            }
            findTx = LineEditMap->find(this->timerCnt+1);
            if(findTx == LineEditMap->end())
            {
                this->timerCnt = 0;
                return false;
            }
        }
        //如果到没被勾选的vlineEdit到不了这里，但timerCnt会++，直到存在能够发送的vlineEdit，跑到vTxTimeOut函数
        this->lineEditTx = &LineEditData[this->timerCnt];
        this->timerCnt++;
        if(this->timerCnt >= (*this->MultPleMaxCnt))
        {
            this->timerCnt = 0;
            return false;
        }
        //查询到了，清除标志
        lastFindPos = 0;
        return true;
    }
    return false;
}
void vQObjectTx::vLineEditTxOne(qint32 num)
{
    //超出地址，则直接返回
    if(num>=*this->MultPleMaxCnt)
    {
        return;
    }
    this->lineEditTx = &LineEditData[num];
    if((*this->lineEditTx)!="")
    {
        //源程序这里对16进制发送不进行编码处理，这里进行编码，是为了单片机能够更好的接收数据
        if(this->vSerial->vSerialData->txHexEnable)
        {
           emit vTxMessage(CodeCoding((*lineEditTx)+LineFeed));
        }
        else
        {
           emit vTxMessage(CodeCoding((*lineEditTx)+LineFeed));
        }
    }
}
void vQObjectTx::vTxAddrSet(QByteArray * dataAddr)
{
    this->textEditTx = dataAddr;
}
void vQObjectTx::vAddrSet(qint32 * CntAddr,QByteArray * dataAddr,QMap<qint32,bool*> *mapAddr)
{
    this->MultPleMaxCnt = CntAddr;
    this->LineEditData = dataAddr;
    this->LineEditMap = mapAddr;
}
vQserialTxThread::vQserialTxThread(QObject *parent) : QThread(parent)
{

}

