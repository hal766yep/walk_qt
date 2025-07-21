#include "vqserialrxthread.h"
//#include "vansibash.h"
vQObjectRx::vQObjectRx(QObject *parent) : QObject(parent)
{

}
vQObjectRx::~vQObjectRx(void)
{

};
//打开接收的信号与槽，方便切换
void vQObjectRx::vConnectRx(void)
{
    connect(this->vSerial->qSerial,&QSerialPort::readyRead,
            this, &vQObjectRx::vReadSlot, Qt::QueuedConnection);
}
//断开接收的信号与槽，方便切换
void vQObjectRx::vDisConnectRx(void)
{
    disconnect(this->vSerial->qSerial,&QSerialPort::readyRead,
            this, &vQObjectRx::vReadSlot);
}

void vQObjectRx::vReadSlot(void)
{
    QByteArray vRxSerialBuff;
    static QByteArray vReadAll;
    int start_line1 = -1;
    int start_line2 = -1;
    if(this->vSerial!=NULL)
    {
        //获取原始数据
        if(this->vSerial->qSerial->isOpen())
        {
            vReadAll.append(this->vSerial->qSerial->readAll());
            //获取"\n" 和 "\r"的下标，这里让回车或者换行都另起一行
            start_line1 = vReadAll.lastIndexOf("\n");
            start_line2 = vReadAll.lastIndexOf("\r");
//            qDebug() << "start_line1" << start_line1;
//            qDebug() << "start_line2" << start_line2;
            if(this->vSerial->vSerialData->rxHexEnable)
            {
                vRxSerialBuff.append(vReadAll);
                vReadAll.clear();
            }
            else if(start_line1>=0)
            {
                //带"\n"一起复制进去
                vRxSerialBuff.append(vReadAll.mid(0, start_line1+1));
                vReadAll.remove(0, start_line1+1);
            }
            else if(start_line2>=0)
            {
                //带"\r"一起复制进去
                vRxSerialBuff.append(vReadAll.mid(0, start_line2+1));
                vReadAll.remove(0, start_line2+1);
            }
            this->vSerial->vSerialData->rxByteCnt += vRxSerialBuff.length(); //源程序有bug,没有回车换行不计数
        }
        if(vRxSerialBuff.isEmpty())
        {
            return;
        }
        //接收添加到缓冲
        if(this->vSerial->vSerialData->rxHexEnable)
        {
            //二进制格式，不做处理
            this->vSerial->vSerialData->RxBuff.append((vRxSerialBuff));
        }
        else
        {
            //是否加入时间戳
            if(!this->vSerial->vSerialData->rxTimeStampEnable)
            {
                // 接收添加到缓冲
                // QString::fromLocal8Bit能显示中文，原文是QString(CodeDecoding(vRxSerialBuff))
                this->vSerial->vSerialData->RxBuff.append(QString::fromLocal8Bit(vRxSerialBuff));
            }
            else
            {
                QString timeString;
                timeString = QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss.zzz]\n");
                //去掉\r ,\n替换为<br/>
                this->vSerial->vSerialData->RxBuff.append(
                    QString(QString::fromLocal8Bit(vRxSerialBuff))
                            .replace("\r","")
                            .replace("\n",timeString));
            }
        }
    }

}

vQserialRxThread::vQserialRxThread(QObject *parent) : QThread(parent)
{

}
