

#include "serialspectrometer.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif


static unsigned int SpectrumTestIndex = 0;
SerialSpectrometer* SerialSpectrometer::mInstance = NULL;





QVector<double> mDarkNoise;
QVector<double> mPixels;
QVector<double> mWavelengths;
QVector<double> mRamans;
QVector<double> mRawDatas;
double mWavelengthsCoef[4];

QVector<QString> mSpectrumParm;//光谱仪参数
QVector<QString> mLaserPowerParm;//激光器功率校准参数
QVector<QString> mLaserVoltageParm;//激光器电压校准参数
QVector<QString> mFactoryLaserInfoParm;//激光器电压校准参数

unsigned char tmpRxBuf[10000] = { 0 }; //收的临时buffer
int tmpRxIndex = 0;
unsigned char fullRxBuf[10000] = { 0 }; //收的全部buffer
unsigned char tmpTxBuf[10000] = { 0 };

double LocalSpectrum[4096]= { 0 };;

unsigned char decodeRxBuf[7000] = { 0 }; //解码DB后的buffer
unsigned char returnParamBuf[7000] = { 0 }; //解码后的buffer
int returnParamBufLength = 0;//解码后的buffer

SerialSpectrometer::SerialSpectrometer()
{
	sendingTick = 0;
	totalPixel = 0;
	wavelengthStatus = false;
	serialOpened = false;
}

QList<QString> serialPortList;
QList<bool> portStatus;

//QList<QString> SerialSpectrometer::serialInit()

void SerialSpectrometer::init()
{
	long integrationTime = 0;;
	int average=1;
	int boxCar=0;
	int laserStatus=0;
	int laserPower=1;
	memset(model, 0, 100);
	memset(serialNumber, 0, 100);

	memset(laserPowerCoef, 0, 20);
	memset(laserVoltageCoef, 0, 20);
	memset(factoryCoef, 0, 20);
	memset(fpgaVer, 0, 20);
	memset(compileVer, 0, 20);



	//读取串口信息
	foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
	{

		//这里相当于自动识别串口号之后添加到了cmb，如果要手动选择可以用下面列表的方式添加进去
		QSerialPort serial;
		serial.setPort(info);

		serialPortList.append(info.portName());


		if (serial.open(QIODevice::ReadWrite))
		{
			//关闭串口等待人为(打开串口按钮)打开
			portStatus.append(false); //false表示没有打开

			serial.close();
		}
		else
		{
			portStatus.append(true); //false表示已经打开了
		}
	}

	return;// portList;
}


int SerialSpectrometer::readData()
{
    //QByteArray rxData;
	qDebug("readData beg ");

	readingFlag = true;
	int numRead = 0;

	int readcount = 0;
	
	bool findEnd = false;

	do{
		unsigned char p[500] = { 0 };

		if (serialOpened == false)
            return false;

        numRead = mSerialPort.read(reinterpret_cast<char*>(p) , 500);
        if(numRead==0&&!mSerialPort.waitForReadyRead(200)){

            break;
        }


		if(numRead>0)
		{
			for (int i = 0; i < numRead; i++)
			{
				if (*(p+i) == 0xbd)
				{
					//数据开始。先清空
					memset(tmpRxBuf, 0, 10000);
					tmpRxIndex = 0;
					*tmpRxBuf = 0xbd;
					tmpRxIndex++;

				}
				else if (*(p + i) == 0xbe)
				{
					//数据开始。先清空
					memcpy(tmpRxBuf + tmpRxIndex, p+i, 1);
					tmpRxIndex++;
				
					findEnd = true;
				}
				else
				{
					//啥也不是那就copy数据
					memcpy(tmpRxBuf + tmpRxIndex, p+i, 1);
					tmpRxIndex++;
				}
			}
					
		}

		if (numRead == 0)
			readcount++;

		if (readcount > 50)
			findEnd = true;
		
	} while (findEnd==false);


	if (tmpRxIndex == 0)
		return 0;

	readingFlag = false;

	

	memset(fullRxBuf, 0, 10000);
	memcpy(fullRxBuf, tmpRxBuf, tmpRxIndex);

	qDebug("read done ,start to decode, total=%d byte", tmpRxIndex);

	bool ret = translateCommand(fullRxBuf, tmpRxIndex);
	
	waitingResponse = false;
	
	if (ret == false)
		numRead = 0;
	else
		numRead = tmpRxIndex;
	return numRead;
}



bool SerialSpectrometer::openSerialPort(long index, char* port, int baud)
{
	qDebug("open serial beg");
	bool ret = false;
	if (serialOpened == true)
		return true;

	if (port == 0)
		return false;

    int v =115200;// ConfigHelper::instance()->getBaudrate();

	if (v != 19200 && v != 115200)
	{
		qDebug("open serial fail");
		return false;
	}

    mSerialPort.setBaudRate(v); //切记这个波特率，只针对003

    mSerialPort.setPortName(port);




	try
    {
        mSerialPort.open(QIODevice::ReadWrite);
	}
	catch (const std::exception&)
	{
		qDebug("open serial fail");

		ret = false;
		return ret;
	}

	ret = true;
	serialOpened = true;
	waitingResponse = false;

	qDebug("open serial succ");

	//读取光谱仪参数
	double*d = (double*)malloc(sizeof(double) * 10000);
	int errCode = 0;
    getWavelengths(0, &errCode, d, 10000);

	qDebug("open serial succ");
	free(d);
	return ret;
}




void SerialSpectrometer::closeSerialPort(long index)
{

	qDebug("提示串口没有打开");

	if (!serialOpened) {
		////QMessageBox::about(NULL, "提示", "串口没有打开");
		return;
	}

	mSerialPort.close();
	serialOpened = false;
	waitingResponse = false;
}


void SerialSpectrometer::setScansToAverage(long index, int *errorCode, int numberOfScansToAverage)
{
	qDebug("open laser");

	if (!serialOpened) {
		//QMessageBox::about(NULL, "提示", "串口没有打开");
		return;
	}


	unsigned char cmd[7] = { 0 };
	cmd[0] = 0xBD;
	cmd[1] = CMD_CODE_SET_AVERAGE;
	cmd[2] = 0x00;
	cmd[3] = 2;
	cmd[4] = (unsigned char)((numberOfScansToAverage & 0xFF00) >> 8);
	cmd[5] = (unsigned char)(numberOfScansToAverage & 0xFF);
	cmd[6] = 0xBE;

	serialWriteCMD(cmd, 7);

	average = numberOfScansToAverage;
}

int SerialSpectrometer::getScansToAverage(long index, int *errorCode)
{
	qDebug("open laser");


	if (!serialOpened) {
		//QMessageBox::about(NULL, "提示", "串口没有打开");
		return 0;
	}


	unsigned char cmd[7] = { 0 };
	cmd[0] = 0xBD;
	cmd[1] = CMD_CODE_GET_AVERAGE;
	cmd[2] = 0x00; //len1
	cmd[3] = 0x00; //len2	
	cmd[4] = 0xBE;

	serialWriteCMD(cmd, 5);


	return average;
}

void SerialSpectrometer::setBoxcarWidth(long index, int *errorCode, int numberOfPixelsOnEitherSideOfCenter)
{
	qDebug("open laser");


	if (!serialOpened) {
		//QMessageBox::about(NULL, "提示", "串口没有打开");
		return;
	}


	unsigned char cmd[7] = { 0 };
	cmd[0] = 0xBD;
	cmd[1] = CMD_CODE_SET_BOXCAR;
	cmd[2] = 0x00;
	cmd[3] = 2;
	cmd[4] = (unsigned char)((numberOfPixelsOnEitherSideOfCenter & 0xFF00) >> 8);
	cmd[5] = (unsigned char)(numberOfPixelsOnEitherSideOfCenter & 0xFF);
	cmd[6] = 0xBE;

	serialWriteCMD(cmd, 7);

	boxCar = numberOfPixelsOnEitherSideOfCenter;
}

int SerialSpectrometer::getBoxcarWidth(long index, int *errorCode)
{
	qDebug("open laser");

	if (!serialOpened) {
		//QMessageBox::about(NULL, "提示", "串口没有打开");
		return 0;
	}


	unsigned char cmd[7] = { 0 };
	cmd[0] = 0xBD;
	cmd[1] = CMD_CODE_GET_BOXCAR;
	cmd[2] = 0x00; //len1
	cmd[3] = 0x00; //len2	
	cmd[4] = 0xBE;

	serialWriteCMD(cmd, 5);

	return boxCar;
}


void SerialSpectrometer::setGPIOX(long index, int *errorCode, int gpioId, int value)
{
	qDebug("open laser");


	if (!serialOpened) {
		//QMessageBox::about(NULL, "提示", "串口没有打开");
		return;
	}


	unsigned char cmd[7] = { 0 };
	cmd[0] = 0xBD;
	cmd[1] = CMD_CODE_SET_GPIOX;
	cmd[2] = 0x00;
	cmd[3] = 2;
	cmd[4] = (unsigned char)(gpioId & 0xFF);
	cmd[5] = (unsigned char)(value & 0xFF);
	cmd[6] = 0xBE;

	serialWriteCMD(cmd, 7);

}

int SerialSpectrometer::getGPIOX(long index, int *errorCode, int gpioId)
{
	qDebug("open laser");


	if (!serialOpened) {
		//QMessageBox::about(NULL, "提示", "串口没有打开");
		return 0;
	}


	unsigned char cmd[7] = { 0 };
	cmd[0] = 0xBD;
	cmd[1] = CMD_CODE_GET_GPIOX;
	cmd[2] = 0x00; //len1
	cmd[3] = 0x1; //len2	
	cmd[4] = (unsigned char)(gpioId & 0xFF);
	cmd[5] = 0xBE;

	serialWriteCMD(cmd, 6);

	return 0;
}



bool SerialSpectrometer::openLaser(long index)
{
	qDebug("open laser");

	unsigned char cmd[6] = { 0 };

	if (!serialOpened) {
		//QMessageBox::about(NULL, "提示", "串口没有打开");
		return true;
	}


	cmd[0] = 0xBD;
	cmd[1] = CMD_CODE_SET_LASER_ENABLE;
	cmd[2] = 0x00; //len1
	cmd[3] = 0x01; //len2
	cmd[4] = 0x01; //value
	cmd[5] = 0xBE;

	//mSerialPort.write((char*)cmd, 9);
	serialWriteCMD(cmd, 6);

	laserStatus = true;

	return true;
	
}

void SerialSpectrometer::closeLaser(long index)
{
	
	if (!serialOpened) {
		//QMessageBox::about(NULL, "提示", "串口没有打开");
		return;
	}


	unsigned char cmd[6] = { 0 };

	cmd[0] = 0xBD;
	cmd[1] = CMD_CODE_SET_LASER_ENABLE;
	cmd[2] = 0x00;
	cmd[3] = 0x01;
	cmd[4] = 0x00;
	cmd[5] = 0xBE;

	
	serialWriteCMD(cmd, 6);

	laserStatus = false;
	
}


void SerialSpectrometer::setIntegrationTime(long index, long arg1)
{
	qDebug("setIntegrationTime beg");
	if (!serialOpened) {
		//QMessageBox::about(NULL, "提示", "串口没有打开!");
		return;
	}

	unsigned char cmd[7] = { 0 };
	cmd[0] = 0xBD;
	cmd[1] = CMD_CODE_SET_SPECTRUM_INTE;
	cmd[2] = 0x00;
	cmd[3] = 2;
	cmd[4] = (unsigned char)((arg1 & 0xFF00) >> 8);
	cmd[5] = (unsigned char)(arg1 & 0xFF);
	cmd[6] = 0xBE;

	serialWriteCMD(cmd, 7);

	qDebug("setIntegrationTime return");

	integrationTime = arg1;
}

long SerialSpectrometer::getIntegrationTime(long index)
{
	//qDebug("get spec inte time");
	if (!serialOpened) {
		//QMessageBox::about(NULL, "提示", "串口没有打开");
		return 0 ;
	}

	unsigned char cmd[5] = { 0 };
	cmd[0] = 0xBD;
	cmd[1] = CMD_CODE_GET_SPECTRUM_INTE;
	cmd[2] = 0x00;
	cmd[3] = 0;
	cmd[4] = 0xBE;

	serialWriteCMD(cmd, 5);


	return integrationTime;
}




double* SerialSpectrometer::getSpectrum(long index)
{
	qDebug("getSpectrum beg");
	double *s = 0;

	QDateTime current_date_time = QDateTime::currentDateTime();
	QString ss = current_date_time.toString("hh:mm:ss.zzz ");
	//qDebug(ss.toStdString());
	

	if (!serialOpened) {
		//QMessageBox::about(NULL, "提示", "串口没有打开");
		return s;
	}

	unsigned char cmd[6] = { 0 };
	cmd[0] = 0xBD;
	cmd[1] = CMD_CODE_GET_SPECTRUM_DATA;
	cmd[2] = 0x00;
	cmd[3] = 1;
	cmd[4] = (uint8_t)SPEC_COLLECT_SINGLE_MODE;
	cmd[5] = 0xBE;

	

	if (serialWriteCMD(cmd, 6) > 0)
	{
        s =
            LocalSpectrum;
	}
	
	//waitForSerialResponse(CMD_CODE_GET_SPECTRUM_DATA, 5000);
	qDebug("getSpectrum end");
	
	return s;
}


int SerialSpectrometer::getFormattedSpectrumLength(long index)
{
	qDebug("getFormattedSpectrumLength beg");

	//QDateTime current_date_time = QDateTime::currentDateTime();
	//qDebug() << current_date_time.toString("hh:mm:ss.zzz ");
	

	if (!serialOpened) {
		//QMessageBox::about(NULL, "提示", "串口没有打开");
		return 0;
	}


	if (wavelengthStatus == false)
	{
		setIntegrationTime(0, 1);
		double* ret = getSpectrum(0);

		if (ret == 0)
			return 0;

		int i = 0;
		for (i = 0; i < 4096; i++)
		{
			if (*(ret + i) == 0)
				break;
		}
		//找到第一个不为0的点，那么就可以确定，有多少像素了
		return i;
	}
	else
		return totalPixel;

	qDebug("getFormattedSpectrumLength end");	
	return totalPixel;

}

int SerialSpectrometer::getWavelengths(long index, int *errorCode, double *wavelengths, int length)
{

	if (wavelengthStatus == false)
	{
        totalPixel = getFormattedSpectrumLength(0);//总像素
		if (totalPixel == 0)
			return 0 ;


		//读取各种参数
		ReadSpectrumCoef(0);

		//现在应该已经是成功的数据		
		for (int i = 0; i < totalPixel; i++) {
			double v = mWavelengthsCoef[0] + mWavelengthsCoef[1] * i + mWavelengthsCoef[2] * i*i + mWavelengthsCoef[3] * i*i*i;// +mWavelengthsCoef[4] * i*i*i*i + mWavelengthsCoef[5] * i*i*i*i*i*i;
			mWavelengths.append(v);
		}

		wavelengthStatus = true;

	}

	for (int i = 0; i < totalPixel; i++) 
		wavelengths[i] = mWavelengths[i];

	return totalPixel;
}


const char* SerialSpectrometer::getSerialNumber(long index) //c++11 const char
{
	qDebug("getSerialNumber beg");

	if (!serialOpened) {
		//QMessageBox::about(NULL, "提示", "串口没有打开");

        return "";
	}

	unsigned char cmd[5] = { 0 };
	cmd[0] = 0xBD;
	cmd[1] = (unsigned char)CMD_CODE_HARDWARE_VER;
	cmd[2] = 0x00;
	cmd[3] = 0;
	cmd[4] = 0xBE;

	serialWriteCMD(cmd, 5);

	
	
	memcpy(serialNumber, tmpRxBuf, tmpRxIndex);
	

	qDebug("getSerialNumber end");
	
	return serialNumber;
}


void SerialSpectrometer::getModel(long index)
{
	//qDebug("getModel beg");

	if (!serialOpened) {
		//QMessageBox::about(NULL, "提示", "串口没有打开");
		return;
	}

	unsigned char cmd[5] = { 0 };
	cmd[0] = 0xBD;
	cmd[1] = (unsigned char)CMD_CODE_HARDWARE_VER;
	cmd[2] = 0x00;
	cmd[3] = 0;
	cmd[4] = 0xBE;

	serialWriteCMD(cmd, 5);

	//
	//qDebug("getModel end");

}



void SerialSpectrometer::setLaserVoltage(long index, double arg1)
{
	//qDebug("setLaserVoltage end");

	if (!serialOpened) {
		//QMessageBox::about(NULL, "提示", "串口没有打开!");
		return;
	}


	unsigned char cmd[13] = { 0 };
	double v = arg1;

	cmd[0] = 0xBD;
	cmd[1] = CMD_CODE_SET_LASER_VOLTAGE;
	cmd[2] = 0x00;
	cmd[3] = 8;
	memcpy(&cmd[4], &v, 8);
	cmd[12] = 0xBE;
			
	//mSerialPort.write((char*)cmd, 17);
	serialWriteCMD(cmd, 13);

	laserVoltage = arg1;
}


void SerialSpectrometer::setLaserSwitch(long index, int *errorCode, int state)
{

	//qDebug("setLaserSwitch beg");

	if (!serialOpened) {
		//QMessageBox::about(NULL, "提示", "串口没有打开!");
		return;
	}

	unsigned char cmd[13] = { 0 };

	cmd[0] = 0xBD;
	cmd[1] = CMD_CODE_SET_LASER_ENABLE;
	cmd[2] = 0x00;
	cmd[3] = 1;
	cmd[4] = state & 0x1;
	cmd[5] = 0xBE;

	//mSerialPort.write((char*)cmd, 17);
	serialWriteCMD(cmd, 6);

	laserStatus = state;

}

void SerialSpectrometer::setLaserPower(long index, int *errorCode, int power)
{
	//qDebug("setLaserPower beg");

	if (!serialOpened) {
		//QMessageBox::about(NULL, "提示", "串口没有打开!");
		return;
	}


	unsigned char cmd[13] = { 0 };

	double v = power;

	cmd[0] = 0xBD;
	cmd[1] = CMD_CODE_SET_LASER_POW;
	cmd[2] = 0x00;
	cmd[3] = 8;
	
	memcpy(&cmd[4], &v, 8);
	cmd[12] = 0xBE;

	
	serialWriteCMD(cmd, 13);

	//补丁，因为有的老版本，激光关闭的情况下，设置功率，激光会自动打开
	setLaserSwitch(0, errorCode, laserStatus);

	laserPower = power;
}

int SerialSpectrometer::getLaserPower(long index, int *errorCode, int* targetPower)
{
	return laserPower;
}

int SerialSpectrometer::getLaserState(long index, int *errorCode, int* targetState)
{
	return laserStatus;
}



void SerialSpectrometer::ReadSpectrumCoef(long index)
{
	qDebug("ReadSpectrumCoef beg");

	if (!serialOpened) {
		//QMessageBox::about(NULL, "提示", "串口没有打开!");
		return;
	}

	for (int i = 0; i < 6; i++)
	{
		unsigned char cmd[6] = { 0 };
		cmd[0] = 0xBD;
		cmd[1] = CMD_CODE_GET_SPECTRUM_COEF;
		cmd[2] = 0x00;
		cmd[3] = 1;
		cmd[4] = i;
		cmd[5] = 0xBE;

		serialWriteCMD(cmd, 6);

	}
	
	qDebug("ReadSpectrumCoef beg");

}

void SerialSpectrometer::WriteSpectrumCoef(long index)
{
	qDebug("WriteSpectrumCoef beg");

	if (!serialOpened) {
		//QMessageBox::about(NULL, "提示", "串口没有打开!");
		return;
	}

	/*QList<QLineEdit*> lineEdit = //ui.groupBoxSpectrum->findChildren<QLineEdit *>();
	//QList<QLabel*> label = ui->groupBoxLaser->findChildren<QLabel *>();

	for (int i = 0; i < lineEdit.size(); i++) {
		QString text = lineEdit[i]->text(); 
		int len = text.toStdString().length();
		//        if (len > 15)
		//        {
		//            QMessageBox::information(0, tr("Error"), QString("参数[%1]长度超过15!").arg(QString::number(i+1,10)));
		//            return;
		//        }

		//qDebug() << i  << lineEdit[i] << lineEdit[i]->text();
	}

	//ui.progressBarSpectrumCoef->setVisible(true);
	//ui.progressBarSpectrumCoef->setRange(0, lineEdit.size());//19

	for (int i = 0; i < lineEdit.size(); i++) {
		if (i == 17) {
			unsigned char cmd[25] = { 0 };
			QString str = lineEdit[i]->text();
			QStringList ret = str.split(',');
			//qDebug() << str;
			int darkvalue = ret[2].toInt();
			int satvalue = ret[3].toInt();


			if (ret[0] == "Enabled") cmd[5] = 1;
			if (ret[1] == "Enabled") cmd[6] = 1;


			cmd[7] = (0xff & darkvalue);
			cmd[8] = (0xff00 & darkvalue) >> 8;
			cmd[9] = (0xff & satvalue);
			cmd[10] = (0xff00 & satvalue) >> 8;


			cmd[0] = 0xBD;
			cmd[1] = CMD_CODE_SET_SPECTRUM_COEF;
			cmd[2] = 0x00;
			cmd[3] = 17;
			cmd[4] = i;
			cmd[21] = 0xBE;
			serialWriteCMD(cmd, 22);
		}
		else {
			QString str = lineEdit[i]->text();
			//qDebug() << str;
			char*  ch;
			QByteArray ba = str.toLatin1(); // must
			ch = ba.data();

			unsigned char cmd[22] = { 0 };
			cmd[0] = 0xBD;
			cmd[1] = CMD_CODE_SET_SPECTRUM_COEF;
			cmd[2] = 0x00;
			cmd[3] = 17;
			cmd[4] = i;
			memcpy(&cmd[5], ch, 16);
			cmd[21] = 0xBE;

			serialWriteCMD(cmd, 22);
		}

		

		//ui.progressBarSpectrumCoef->setValue(i + 1);
	}*/

	qDebug("WriteSpectrumCoef end");

}

void SerialSpectrometer::ReadLaserPowerCoef(long index)
{
	if (!serialOpened) {
		//QMessageBox::about(NULL, "提示", "串口没有打开!");
		return;
	}

	for (int i = 0; i < 8; i++)
	{
		unsigned char cmd[6] = { 0 };
		cmd[0] = 0xBD;
		cmd[1] = (unsigned char)CMD_CODE_GET_LASER_POW_COEF;
		cmd[2] = 0x00;
		cmd[3] = 1;
		cmd[4] = i;
		cmd[5] = 0xBE;

		serialWriteCMD(cmd, 6);
		
	}

	qDebug("ReadLaserPowerCoef end");
}

void SerialSpectrometer::WriteLaserPowerCoef(long index)
{
	if (!serialOpened) {
		//QMessageBox::about(NULL, "提示", "串口没有打开!");
		return;
	}

	/*QList<QLineEdit*> lineEdit = //ui.groupBoxLaserPower->findChildren<QLineEdit *>();
	QList<QLabel*> label = //ui.groupBoxLaserPower->findChildren<QLabel *>();

	for (int i = 0; i < lineEdit.size(); i++) {
		QString text = lineEdit[i]->text();
		int len = text.toStdString().length();
		if (len > 15)
		{
		    QMessageBox::information(0, tr("Error"), QString("参数[%1]长度超过15!").arg(QString::number(i+1,10)));
		    return;
		}

		//qDebug() << i  << lineEdit[i] << lineEdit[i]->text();
	}

	//ui.progressBarLaserCoef->setVisible(true);
	//ui.progressBarLaserCoef->setRange(0, lineEdit.size());//8

	for (int i = 0; i < lineEdit.size(); i++) {
		QString str = lineEdit[i]->text();
		//qDebug() << str;
		char*  ch;
		QByteArray ba = str.toLatin1(); // must
		ch = ba.data();

		unsigned char cmd[23] = { 0 };
		cmd[0] = 0xBD;
		cmd[1] = (unsigned char)CMD_CODE_SET_LASER_POW_COEF;
		cmd[2] = 0x00;
		cmd[3] = 17;
		cmd[4] = i;
		memcpy(&cmd[5], ch, 16);
		cmd[22] = 0xBE;

		serialWriteCMD(cmd, 23);

		QThread::msleep(500);
		QApplication::processEvents();

		//ui.progressBarLaserCoef->setValue(i + 1);
	}*/

	//ui.progressBarLaserCoef->setVisible(false);
	//ui.statusBar->showMessage(tr("激光器功率校准参数写入完成!"), 3000);
}

void SerialSpectrometer::ReadLaserVoltageCoef(long index)
{
	if (!serialOpened) {
		//QMessageBox::about(NULL, "提示", "串口没有打开!");
		return;
	}

	for (int i = 0; i < 8; i++)
	{
		unsigned char cmd[6] = { 0 };
		cmd[0] = 0xBD;
		cmd[1] = (unsigned char)CMD_CODE_GET_LASER_VOL_COEF;
		cmd[2] = 0x00;
		cmd[3] = 1;
		cmd[4] = i;
		cmd[5] = 0xBE;

		serialWriteCMD(cmd, 6);

		
	}

	qDebug("ReadLaserVoltageCoef end");

}



void SerialSpectrometer::WriteLaserVoltageCoef(long index)
{
	if (!serialOpened) {
		//QMessageBox::about(NULL, "提示", "串口没有打开!");
		return;
	}
	
	
	for (int i = 0; i < 7; i++)
	{
		/*QList<QLineEdit*> lineEdit = //ui.groupBoxLaserVoltage->findChildren<QLineEdit *>();
			QList<QLabel*> label = //ui.groupBoxLaserVoltage->findChildren<QLabel *>();

			for (int i = 0; i < lineEdit.size(); i++) {
				QString text = lineEdit[i]->text();
				int len = text.toStdString().length();
				if (len > 15)
				{
					QMessageBox::information(0, tr("Error"), QString("参数[%1]长度超过15!").arg(QString::number(i + 1, 10)));
					return;
				}

				//qDebug() << i  << lineEdit[i] << lineEdit[i]->text();
			}

		//ui.progressBarLaserCoef->setVisible(true);
		//ui.progressBarLaserCoef->setRange(0, lineEdit.size());//8

		for (int i = 0; i < lineEdit.size(); i++) {
			QString str = lineEdit[i]->text();
			//qDebug() << str;
			char*  ch;
			QByteArray ba = str.toLatin1(); // must
			ch = ba.data();

			unsigned char cmd[25] = { 0 };
			cmd[0] = 0xBD;
			cmd[1] = (unsigned char)CMD_CODE_SET_LASER_VOL_COEF;
			cmd[2] = 0x00;
			cmd[3] = 17;
			cmd[4] = i;
			memcpy(&cmd[5], ch, 16);
			cmd[21] = 0xBE;

			serialWriteCMD(cmd, 22);

			QThread::msleep(500);
			QApplication::processEvents();

			//ui.progressBarLaserCoef->setValue(i + 1);
		}

		//ui.progressBarLaserCoef->setVisible(false);
		//ui.statusBar->showMessage(tr("激光器电压校准参数写入完成!"), 3000);

		*/
	}
	

	qDebug("WriteLaserVoltageCoef end");
}

void SerialSpectrometer::LaserFactoryReadInfo(long index)
{
	if (!serialOpened) {
		//QMessageBox::about(NULL, "提示", "串口没有打开!");
		return;
	}	

	for (int i = 0; i < 7; i++)
	{
		unsigned char cmd[6] = { 0 };

		cmd[0] = 0xBD;
		cmd[1] = (unsigned char)CMD_CODE_GET_LASER_FAC_COEF;
		cmd[2] = 0x00;
		cmd[3] = 1;
		cmd[4] = i;
		cmd[5] = 0xBE;

		serialWriteCMD(cmd, 6);

	}

	qDebug("LaserFactoryReadInfo end");

}

void SerialSpectrometer::LaserFactoryWriteInfo(long index)
{
	if (!serialOpened) {
		//QMessageBox::about(NULL, "提示", "串口没有打开!");
		return;
	}


/*	QList<QLineEdit*> lineEdit = //ui.groupBoxLaserFactory->findChildren<QLineEdit *>();
	QList<QLabel*> label = //ui.groupBoxLaserFactory->findChildren<QLabel *>();

	for (int i = 0; i < lineEdit.size(); i++) {
		QString text = lineEdit[i]->text();
		int len = text.toStdString().length();
		if (len > 15)
		{
			switch (i)
			{
			case 0: QMessageBox::information(0, tr("Error"), QString("[ %1 ]长度超过15!").arg("型号")); break;
			case 1: QMessageBox::information(0, tr("Error"), QString("[ %1 ]长度超过15!").arg("序列号码")); break;
			case 2: QMessageBox::information(0, tr("Error"), QString("[ %1 ]长度超过15!").arg("中心波长")); break;
			case 3: QMessageBox::information(0, tr("Error"), QString("[ %1 ]长度超过15!").arg("日期")); break;
			case 4: QMessageBox::information(0, tr("Error"), QString("[ %1 ]长度超过15!").arg("最小功率")); break;
			case 5: QMessageBox::information(0, tr("Error"), QString("[ %1 ]长度超过15!").arg("最大功率")); break;
			case 6: QMessageBox::information(0, tr("Error"), QString("[ %1 ]长度超过15!").arg("是否校准")); break;
			default:break;
			}
			return;
		}

		//qDebug() << i  << lineEdit[i] << lineEdit[i]->text();
	}

	//ui.progressBarFactoryCoef->setVisible(true);
	//ui.progressBarFactoryCoef->setRange(0, lineEdit.size());//8

	for (int i = 0; i < lineEdit.size(); i++) {
		QString str = lineEdit[i]->text();
		qDebug() << str;
		char*  ch;
		QByteArray ba = str.toLatin1(); // must
		ch = ba.data();

		unsigned char cmd[22] = { 0 };

		cmd[0] = 0xBD;
		cmd[1] = (unsigned char)CMD_CODE_SET_LASER_FAC_COEF;
		cmd[2] = 0x00;
		cmd[3] = 17;
		cmd[4] = i;
		memcpy(&cmd[5], ch, 16);
		cmd[21] = 0xBE;
		serialWriteCMD(cmd, 22);



		//ui.progressBarFactoryCoef->setValue(i + 1);
	}
	*/
	

}






void SerialSpectrometer::GetFPGASoftwareVer(long index)
{
	if (!serialOpened) {
		//QMessageBox::about(NULL, "提示", "串口没有打开!");
		return;
	}
	
	unsigned char cmd[5] = { 0 };
	cmd[0] = 0xBD;
	cmd[1] = CMD_CODE_FPGA_VER;
	cmd[2] = 0x00;
	cmd[3] = 0;
	cmd[4] = 0xBE;

	serialWriteCMD(cmd, 5);
	

	qDebug("GetFPGASoftwareVer end");
}

void SerialSpectrometer::GetFinalCompTime(long index)
{
	if (!serialOpened) {
		//QMessageBox::about(NULL, "提示", "串口没有打开!");
		return;
	}
	
	unsigned char cmd[5] = { 0 };
	cmd[0] = 0xBD;
	cmd[1] = (unsigned char)CMD_CODE_LASTED_COMPILE_TIME;
	cmd[2] = 0x00;
	cmd[3] = 0;
	cmd[4] = 0xBE;

	serialWriteCMD(cmd, 5);


	qDebug("GetFPGASoftwareVer end");
}

void SerialSpectrometer::GetHexSoftwareVer(long index)
{
	if (!serialOpened) {
		//QMessageBox::about(NULL, "提示", "串口没有打开!");
		return;
	}
	
	unsigned char cmd[5] = { 0 };
	cmd[0] = 0xBD;
	cmd[1] = (unsigned char)CMD_CODE_SOFTWARE_VER;
	cmd[2] = 0x00;
	cmd[3] = 0;
	cmd[4] = 0xBE;

	serialWriteCMD(cmd, 5);

	qDebug("GetHexSoftwareVer end");
}

char sendingData[1000] = { 0 };
int sendingDataLength = 0;




int SerialSpectrometer::serialWriteCMD(unsigned char* buffer, unsigned int len)
{
	if (serialOpened == false)
	{
		//QMessageBox::about(NULL, "提示", "串口没有打开!");
		return 0;
	}

	unsigned int i = 0;

	QDateTime time = QDateTime::currentDateTime();   //获取当前时间  

	sendingFlag = true;

	memset(tmpTxBuf, 0, 10000);

	tmpTxBuf[0] = 0xBD;

	int index = 1;
	for (int i = 1; i < len-1; i++)
	{
		if ((*(buffer + i)) == 0xBD)
		{
			*(tmpTxBuf + index) = 0xDB;
			index++;
			*(tmpTxBuf + index) = 0xE7;
			index++;
		}
		else if ((*(buffer + i)) == 0xBE)
		{
			*(tmpTxBuf + index) = 0xDB;
			index++;
			*(tmpTxBuf + index) = 0xE8;
			index++;
		}
		else if ((*(buffer + i)) == 0xDB)
		{
			*(tmpTxBuf + index) = 0xDB;
			index++;
			*(tmpTxBuf + index) = 0x00;
			index++;
		}
		else
		{
			*(tmpTxBuf + index) = *(buffer + i);
			index++;
		}
	}


	tmpTxBuf[index] = 0xBE;
	index++;
	
	
    mSerialPort.flush();
    mSerialPort.write(reinterpret_cast<char*>(tmpTxBuf), (unsigned int) index);

	qDebug("write done->%d byte", index);
	sendingFlag = false;
	waitingResponse = true;

    QDateTime time1 = QDateTime::currentDateTime();   //获取当前时间
    sendingTick  = time.msecsTo(time1);   //将当前时间转为时间戳
	
	 
	return readData();

}




//////////////////////////////////////////////////////////////////////////
//decode
//////////////////////////////////////////////////////////////////////////




unsigned short SerialSpectrometer::decoding(unsigned char *src, unsigned short srclen, unsigned char *des)//反解码
{
	unsigned short templen = srclen; //接收数据字节
	unsigned short i = 0, j = 0;

	for (i = 0; i < templen; i++)
	{
		if (i == 0)
		{
			*(des + (j++)) = *src;
		}
		else if (i == (templen - 1))
		{
			*(des + (j++)) = *(src + templen - 1);
		}
		else  //反解码数据
		{
			if (((*(src + i)) == 0xDB) && (((*(src + i + 1)) == 0xE7)))
			{
				*(des + (j++)) = 0xBD;
				i++;
			}
			else if (((*(src + i)) == 0xDB) && (((*(src + i + 1)) == 0xE8)))
			{
				*(des + (j++)) = 0xBE;
				i++;
			}
			else if (((*(src + i)) == 0xDB) && (((*(src + i + 1)) == 0x00)))
			{
				*(des + (j++)) = 0xDB;
				i++;
			}
			else
			{
				*(des + (j++)) = (*(src + i));
			}
		}
	}

	return (j);
}


bool SerialSpectrometer::translateCommand(unsigned char *out1buf, int len)
{
	unsigned short decodelen = 0;
	bool ret = true;

	memset(decodeRxBuf, 0, 7000);

	//返回值放在decodeRxBuf里面，长度是decodelen
	decodelen = decoding(out1buf, len, decodeRxBuf);


	//判断数据长度
	if (decodelen == ((decodeRxBuf[2] << 8) | decodeRxBuf[3]) + 5)
	{
		int cmdType = *(decodeRxBuf+1);

		switch (cmdType)
		{
		case CMD_CODE_GET_SPECTRUM_INTE:             //cmd len_h cmd_l data
		{
			unsigned short tmpVal = 0;
			tmpVal = (short)((decodeRxBuf[4] << 8) | decodeRxBuf[5]);
			qDebug("emit GetIntegrationDone");

			//emit GetIntegrationDone();
			break;
		}
		case CMD_CODE_SET_SPECTRUM_INTE:             //cmd len_h cmd_l data
		{
			qDebug("emit SetIntegrationDone");
			//emit SetIntegrationDone();
			break;
		}

		case CMD_CODE_GET_SPECTRUM_DATA:
		{
			//qDebug("spec mode:%d", _thisNodePack->buffer[3]);
			qDebug("emit GetSpectrumDone");

			//spectrum length is len
			int len = (decodeRxBuf[2] << 8) + decodeRxBuf[3] - 1;
			unsigned char*p = decodeRxBuf + 5;

			for (int i = 0; i < len / 2; i++)
			{
				LocalSpectrum[i] = ((*p) << 8) + (*(p + 1));
				p = p + 2;
			}



			break;
		}
		case CMD_CODE_GET_SPECTRUM_COEF:
		{
			//qDebug("get spec coef");
			unsigned char tmpWavelengths[16] = { 0 };
			double tmpWavelengthsValue = -1;
			memcpy(tmpWavelengths, &decodeRxBuf[5], 16);

            std::istringstream istr(reinterpret_cast<char*>(tmpWavelengths) );
			istr >> tmpWavelengthsValue;


			char tp[20] = { 0 };
			QString ss = tp;

			switch (decodeRxBuf[4])
			{
			case 0://serialnumber
				memset(serialNumber, 0, 20);
				copyDataToBuffer(serialNumber, (char*)&decodeRxBuf[5], 20);
				break;

			case 1://param0			
				copyDataToBuffer(tp, (char*)&decodeRxBuf[5], 20);
				ss = tp;
				mWavelengthsCoef[0] = ss.toDouble();
				break;
			case 2://param1			
				copyDataToBuffer(tp, (char*)&decodeRxBuf[5], 20);
				ss = tp;
				mWavelengthsCoef[1] = ss.toDouble();
				break;
			case 3://param2			
				copyDataToBuffer(tp, (char*)&decodeRxBuf[5], 20);
				ss = tp;
				mWavelengthsCoef[2] = ss.toDouble();
				break;
			case 4://param3			
				copyDataToBuffer(tp, (char*)&decodeRxBuf[5], 20);
				ss = tp;
				mWavelengthsCoef[3] = ss.toDouble();
				break;
			case 17:
			{
				//第一位,Auto Nulling Enable
				QString tmpStr1 = QString::number(decodeRxBuf[5]);
				//第二位,Temperature Compensation
				QString tmpStr2 = QString::number(decodeRxBuf[6]);
				//第三位,Dark Val
				int tmpDarkVal = decodeRxBuf[8] << 8 | decodeRxBuf[7];
				QString tmpStr3 = QString::number(tmpDarkVal);
				//第四位,Saturation Val
				int tmpSatuVal = decodeRxBuf[10] << 8 | decodeRxBuf[9];
				QString tmpStr4 = QString::number(tmpSatuVal);
				QString tmpStr = tmpStr1.append(",").append(tmpStr2).append(",").append(tmpStr3).append(",").append(tmpStr4);

				//mSpectrumParm[decodeRxBuf[4]] = tmpStr;

				break;
			}

			qDebug("emit SpectrumCoefficientDone");
	
			}
			break;
		}
		case CMD_CODE_GET_LASER_POW_COEF:
		{
			mLaserPowerParm[decodeRxBuf[4]] = QString::fromStdString((char*)&decodeRxBuf[5]);
			//if (decodeRxBuf[3] == LASER_MAX_COEFS - 1) {
				//emit LaserPowerParmDone(mLaserPowerParm);
			//}
			break;
		}
		case CMD_CODE_GET_LASER_VOL_COEF:
		{
			mLaserVoltageParm[decodeRxBuf[4]] = QString::fromStdString((char*)&decodeRxBuf[5]);
			//if (decodeRxBuf[3] == LASER_MAX_COEFS - 1) {
				//emit LaserVoltageParmDone(mLaserVoltageParm);
			//}
			break;
		}
		case CMD_CODE_GET_LASER_FAC_COEF:
		{
			mFactoryLaserInfoParm[decodeRxBuf[4]] = QString::fromStdString((char*)&decodeRxBuf[5]);
			//if (decodeRxBuf[3] == LASER_FAC_MAX_COEFS - 1) {
				//emit LaserInfoParmDone(mFactoryLaserInfoParm);
			//}
			break;
		}
		case CMD_CODE_FPGA_VER:
		{
			unsigned short tmpVal = 0;
			memcpy(&tmpVal, &(decodeRxBuf[4]), sizeof(unsigned short));
			//emit FpgaVerDone(mFactoryLaserInfoParm);
			break;
		}
		case CMD_CODE_LASTED_COMPILE_TIME:
		{
			qDebug("CMD_CODE_LASTED_COMPILE_TIME");
			char tmpBuffer[32] = { 0 };
			memcpy(tmpBuffer, &(decodeRxBuf[4]), 32);
			//emit GetLastCompileTime(tmpBuffer);
			break;
		}
		case CMD_CODE_TRIGGER_MODE:
		{
			break;
		}
		case CMD_CODE_TRIGGER_DELAY:
		{
			break;
		}
		case CMD_TRIGGER_HIGH_LEVEL_DELAY:
		{
			break;
		}

		default:
		{
			ret = false;
			qDebug("data_pack decode error");
			break;
		}
		}// end of switch
	}
	else
	{
		qDebug("data_pack is error");
	}

	return ret;

}


int SerialSpectrometer::copyDataToBuffer(char*destData, char*srcData, int destMaxLen)
{
	int index = 0;

	for (int i = 0; i < destMaxLen; i++)
	{
		char d = *(srcData + i);

		if (d != 0)
		{
			*(destData + index) = d;
			index++;
		}
		else
			return index;

	}

	return index;
}



void  SerialSpectrometer::setTriggerMode(long index, int *errorCode, int mode)
{

	if (!serialOpened) {
		//QMessageBox::about(NULL, "提示", "串口没有打开!");
		return;
	}


	unsigned char cmd[7] = { 0 };

	cmd[0] = 0xBD;
	cmd[1] = (unsigned char)CMD_CODE_TRIGGER_MODE;
	cmd[2] = 00;
	cmd[3] = 02;
	cmd[4] = 00;
	cmd[5] = mode & 0xFF;
	cmd[6] = 0xBE;

	serialWriteCMD(cmd, 7);


	qDebug("setTriggerMode end");
	triggerMode = mode;
}


void SerialSpectrometer::setTriggerDelay(long index, int *errorCode, long delay)
{
	if (!serialOpened) {
		//QMessageBox::about(NULL, "提示", "串口没有打开!");
		return;
	}

	unsigned char cmd[7] = { 0 };

	cmd[0] = 0xBD;
	cmd[1] = (unsigned char)CMD_CODE_TRIGGER_DELAY;
	cmd[2] = 00;
	cmd[3] = 02;
	cmd[4] = (delay & 0xFF) >> 8;
	cmd[5] = (delay & 0xFF);
	cmd[6] = 0xBE;

	serialWriteCMD(cmd, 7);

	
	qDebug("setTriggerDelay end");
}


void SerialSpectrometer::setTriggerHighLevelDelay(long index, int *errorCode, long delay)
{
	if (!serialOpened) {
		//QMessageBox::about(NULL, "提示", "串口没有打开!");
		return;
	}

	
	unsigned char cmd[7] = { 0 };

	cmd[0] = 0xBD;
	cmd[1] = (unsigned char)CMD_TRIGGER_HIGH_LEVEL_DELAY;
	cmd[2] = 00;
	cmd[3] = 02;
	cmd[4] = (delay & 0xFF) >> 8;
	cmd[5] = (delay & 0xFF);
	cmd[6] = 0xBE;

	serialWriteCMD(cmd, 7);

	

	qDebug("setTriggerHighLevelDelay end");
}


