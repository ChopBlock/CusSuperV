
#ifndef SERIAL_SPECTROMETER_H
#define SERIAL_SPECTROMETER_H



//#include "CommDef.h"
#include <vector>
#include <stdlib.h>
#include<string>
#include<sstream>
#include <iostream>
#include <chrono>
#include <thread>
#include<QtSerialPort/QSerialPort>
#include<QtSerialPort/QSerialPortInfo>
//#include "serialwrapper.h"
//#include "qt.hpp"
#include<QDateTime>


#define UART_DEBUG
#define USART_ANDROID_PACKS_NUMBER				30

#define TOTAL_PIXEL_NUM							2048
#define SPECTRUM_MAX_COEFS						21
#define MAX_COEFFS_WIDTH						16

#define LASER_MAX_COEFS							8
#define LASER_FAC_MAX_COEFS						7

typedef enum
{
	CMD_CODE_RESET = 0x10,
	CMD_CODE_STATUS = 0x11,
	CMD_CODE_HARDWARE_VER,
	CMD_CODE_SOFTWARE_VER,
	CMD_CODE_FPGA_VER,
	CMD_CODE_LASTED_COMPILE_TIME,//0x15
	CMD_CODE_RESERVE_1,
	CMD_CODE_BATTERY_TEMP,
	CMD_CODE_BATTERY_VOL,
	CMD_CODE_TEC_TEMP,
	CMD_CODE_BOARD_TEMP,//0x1A
	CMD_CODE_SPECTRUM_STATUS,
	CMD_CODE_GET_SPECTRUM_COEF = 0x1C,
	CMD_CODE_SET_SPECTRUM_COEF = 0x1D,
	CMD_CODE_SET_SPECTRUM_INTE = 0x1E,
	CMD_CODE_GET_SPECTRUM_INTE = 0x1F,//0x1F
	CMD_CODE_GET_SPECTRUM_DATA = 0x20,
	CMD_CODE_LASER_STATUS = 0x21,
	CMD_CODE_SET_LASER_ENABLE = 0x22,
	CMD_CODE_SET_LASER_VOLTAGE = 0x23,
	CMD_CODE_GET_LASER_VOLTAGE = 0x24,
	CMD_CODE_SET_LASER_POW = 0x25,//0x25
	CMD_CODE_GET_LASER_POW = 0x26,
	CMD_CODE_SET_LASER_VOL_COEF,
	CMD_CODE_GET_LASER_VOL_COEF,
	CMD_CODE_SET_LASER_POW_COEF,
	CMD_CODE_GET_LASER_POW_COEF,//0x2A
	CMD_CODE_SET_LASER_FAC_COEF,
	CMD_CODE_GET_LASER_FAC_COEF,
	CMD_CODE_USART_BAUD_RATE,

	CMD_CODE_SET_AVERAGE = 0x2F,
	CMD_CODE_GET_AVERAGE = 0x30,


	CMD_CODE_TRIGGER_MODE = 0x40,
	CMD_CODE_TRIGGER_DELAY = 0x41,
	CMD_TRIGGER_HIGH_LEVEL_DELAY = 0x42,

	CMD_CODE_GET_ONE_PIXEL_VALUE = 0x50,
	CMD_CODE_GET_SOME_PIXEL_VALUE = 0x51,

	CMD_CODE_SET_BOXCAR = 0x52,
	CMD_CODE_GET_BOXCAR = 0x53,
	CMD_CODE_SET_GPIOX = 0x54,
	CMD_CODE_GET_GPIOX = 0x55,


	CMD_CODE_PASSWORD
}CMD_CODES_TYPE;


typedef enum
{
	SPEC_COLLECT_IDLE,
	SPEC_COLLECT_SINGLE_MODE,
	SPEC_COLLECT_DARK_MODE,
	SPEC_COLLECT_CONTINUE_MODE,
	SPEC_COLLECT_ERROR
}SPEC_CODES_COLLECT_TYPE;

typedef enum
{
	PLOT_X_PIXEL,
	PLOT_X_WAVELENGTHS,
	PLOT_X_RAMAN
}PLOT_X_TYPE;

typedef enum
{
	ERROR_CODE_SUM = -20,
	ERROR_CODE_EEPROM,
	ERROR_CODE_PACK_NODE,
	ERROR_CODE_PACK_MEMERY,
	ERROR_CODE_PACK,
	ERROR_CODE_PACK_COUNT,
	ERROR_CODE_PACK_VALID_COUNT,
	ERROR_CODE_MEMERY,
	ERROR_CODE_VALID_DATA,
	ERROR_CODE_SPECTRUM_FAIL,
	ERROR_OK = 1
}ERROR_CODES_TYPE;		// the order of enums should match the error code order above




class SerialSpectrometer 
{

	SerialSpectrometer();
	~SerialSpectrometer() {};

	
	static SerialSpectrometer* mInstance;

public:
	static SerialSpectrometer *instance() {
		if (mInstance == NULL) {
			mInstance = new SerialSpectrometer();
		}
		return mInstance;
	}

public:
	bool serialOpened;


	void init();

	
	/////////////////////////////////////
	//SPECTROMETER
	/////////////////////////////////////
	bool openSerialPort(long index,char* port, int baud);
	void closeSerialPort(long index);
		

	/////////////////////////////////////
	//LASER
	/////////////////////////////////////
	bool openLaser(long index);
	void closeLaser(long index);

	void   setScansToAverage(long index, int *errorCode, int numberOfScansToAverage);//add by edwiin
	int    getScansToAverage(long index, int *errorCode);//add by edwiin
	void   setBoxcarWidth(long index, int *errorCode, int numberOfPixelsOnEitherSideOfCenter);//add by edwiin
	int    getBoxcarWidth(long index, int *errorCode);//add by edwiin
	void setGPIOX(long index, int *errorCode, int gpioId, int value);
	int	getGPIOX(long index, int *errorCode, int gpioId);
	long   getIntegrationTime(long index);
	int getWavelengths(long index, int *errorCode, double *wavelengths, int length);
	double* getSpectrum(long index);
	int getFormattedSpectrumLength(long index);//获得光谱仪像素点
	void setIntegrationTime(long index, long time);
	void setTriggerMode(long index, int *errorCode, int mode);

	void ReadSpectrumCoef(long index);
	void WriteSpectrumCoef(long index);
const	char* getSerialNumber(long index);
	void getModel(long index);

	void setTecEnable(long index, bool flag);

	void setLaserVoltage(long index, double arg1);

	void setLaserSwitch(long index, int *errorCode, int state);

	void setLaserPower(long index, int *errorCode, int power);
	int getLaserPower(long index, int *errorCode, int* targetPower);

	int getLaserState(long index, int *errorCode, int* targetState);
	
	void setTriggerDelay(long index, int *errorCode, long delay);
	void setTriggerHighLevelDelay(long index, int *errorCode, long delay);


	void ReadLaserPowerCoef(long index);
	void WriteLaserPowerCoef(long index);
	
	void ReadLaserVoltageCoef(long index);
	void WriteLaserVoltageCoef(long index);
	

	/////////////////////////////////////
	//OTHER
	/////////////////////////////////////
	void LaserFactoryReadInfo(long index);
	void LaserFactoryWriteInfo(long index);

	void GetFPGASoftwareVer(long index);
	void GetFinalCompTime(long index);
	void GetHexSoftwareVer(long index);
	
	//光谱仪有多少点
	int totalPixel;


private:
	
	int serialWriteCMD(unsigned char* buffer, unsigned int len);
	int readData();
	int copyDataToBuffer(char*destData, char*srcData, int destMaxLen);

	bool translateCommand(unsigned char *out1buf, int len);//翻译
	unsigned short decoding(unsigned char *src, unsigned short srclen, unsigned char *des);//反解码
	
private:

    //serial::Serial mSerialPort;
    QSerialPort mSerialPort;
	

	//测试程序运行时间 
	long start_time;
	long end_time;
	bool waitingResponse;

	long sendingTick;
	bool readingFlag;
	bool sendingFlag;

	double laserPower;
	double laserVoltage;
	int laserStatus;
	int average;
	int boxCar;
	int triggerMode;;
	long integrationTime;


	int laserCoef;


	char model[20];
	char serialNumber[20];
	double laserPowerCoef[20];
	double laserVoltageCoef[20];
	double factoryCoef[20];

	char fpgaVer[20];
	char compileVer[20];

	int wavelengthStatus;
	
};
#endif //SERIALSPECTROMETER_H





		
	
