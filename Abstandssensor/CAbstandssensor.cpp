/*
 * CAbstandssensor.cpp
 *
 *  Created on: Jan 14, 2020
 *      Author: Bruce Chin
 */

#include <unistd.h>
#include "CAbstandssensor.h"
#include <iostream>
using namespace std;

#define IDENTIFICATION__MODEL_ID		0x00
#define SYSTEM__FRESH_OUT_OF_RESET		0x16
#define SYSRANGE__START 				0x18
#define RESULT__INTERRUPT_STATUS_GPIO 	0x4F
#define RESULT__RANGE_STATUS			0x4D		// braucht 4x bit shift nach rechts für Range Status
#define RESULT__RANGE_VAL 				0x62
#define SYSTEM__INTERRUPT_CLEAR 		0x15

// Global Variable for communication between threads
extern bool runLoop;
bool inArea = true;

CAbstandssensor::CAbstandssensor(unsigned int I2CBus, unsigned int I2CAddress)
: I2CDevice(I2CBus, I2CAddress)
{
	this->I2CAddress = I2CAddress;
	this->I2CBus = I2CBus;
	this->mRange = 0;
	this->mOperateMode = CAbstandssensor::Range_SingleShot;
	this->mRangeStatus = 0;
	this->mReady = 0;
	this->mInterrupt = 0;
	mSollWert = 0;
}

void CAbstandssensor::init()
{
	cout << "[*] CAbstandssensor: init . . . " << endl;
//	char reset = this->readRegister(SYSTEM__FRESH_OUT_OF_RESET);
	//if(reset == 0x01)
	{
		this->writeRegister(0x0207, 0x01);
		this->writeRegister(0x0208, 0x01);
		this->writeRegister(0x0096, 0x00);
		this->writeRegister(0x0097, 0xfd);
		this->writeRegister(0x00e3, 0x00);
		this->writeRegister(0x00e4, 0x04);
		this->writeRegister(0x00e5, 0x02);
		this->writeRegister(0x00e6, 0x01);
		this->writeRegister(0x00e7, 0x03);
		this->writeRegister(0x00f5, 0x02);
		this->writeRegister(0x00d9, 0x05);
		this->writeRegister(0x00db, 0xce);
		this->writeRegister(0x00dc, 0x03);
		this->writeRegister(0x00dd, 0xf8);
		this->writeRegister(0x009f, 0x00);
		this->writeRegister(0x00a3, 0x3c);
		this->writeRegister(0x00b7, 0x00);
		this->writeRegister(0x00bb, 0x3c);
		this->writeRegister(0x00b2, 0x09);
		this->writeRegister(0x00ca, 0x09);
		this->writeRegister(0x0198, 0x01);
		this->writeRegister(0x01b0, 0x17);
		this->writeRegister(0x01ad, 0x00);
		this->writeRegister(0x00ff, 0x05);
		this->writeRegister(0x0100, 0x05);
		this->writeRegister(0x0199, 0x05);
		this->writeRegister(0x01a6, 0x1b);
		this->writeRegister(0x01ac, 0x3e);
		this->writeRegister(0x01a7, 0x1f);
		this->writeRegister(0x0030, 0x00);

		// Recommended : Public registers - See data sheet for more detail
		this->writeRegister(0x0011, 0x10); // Enables polling for ‘New Sample ready’
											// when measurement completes
		this->writeRegister(0x010a, 0x30); // Set the averaging sample period
											// (compromise between lower noise and
											// increased execution time)
		this->writeRegister(0x003f, 0x46); // Sets the light and dark gain (upper
											// nibble). Dark gain should not be
											// changed.
		this->writeRegister(0x0031, 0xFF); // sets the # of range measurements after
											// which auto calibration of system is
											// performed
		this->writeRegister(0x002e, 0x01); // perform a single temperature calibration
										// of the ranging sensor
		this->writeRegister(0x0014, 0x04); 	// Configures interrupt on ‘New Sample
											// Ready threshold event’
	}


	this->writeRegister(SYSTEM__FRESH_OUT_OF_RESET, 0x00);
	this->writeRegister(SYSRANGE__START, mOperateMode);

	int sollWertArray[5];
	int i=0;
	do	{
		if(readRange(sollWertArray[i]))
		{
			mSollWert += sollWertArray[i];
			i++;
		}
	} while (i<5);
	mSollWert = mSollWert / 5.0;
}

void CAbstandssensor::run()
{
	cout << "[*] CAbstandssensor: run . . . " << endl;
	int Abstand;

	while(runLoop)
	{
		if (readRange())
		{
//		  // check if the robot is still in drawing area
//		  if(mRange <=  (mSollWert + 6.0) && mRange >=  (mSollWert - 6.0))
//			{
//				inArea = true;
//			}
//			else
//				inArea = false;
//
//		  Abstand = mRange;
		}
////		cout<<"Abstand: "<<Abstand<<endl;

		usleep(5000);
	}
}



/***************************************************************************************************/
// Methode für Messungen
void CAbstandssensor::setRangeMode(CAbstandssensor::MODE pMode)
{
	mOperateMode = pMode;
}


bool CAbstandssensor::readRange()		// continuous
{
	// check if device is ready for new measurement
	this->readRegister(RESULT__RANGE_STATUS, mReady);
	if(mReady & 0x01)
		this->writeRegister(SYSRANGE__START, mOperateMode);

	// check if measurement is done (interrupt flag is set when measurement is complete)
	this->readRegister(RESULT__INTERRUPT_STATUS_GPIO, mInterrupt);

//	if ((mInterrupt & 0x04) == true)
	if (mInterrupt & 0x04)
	{
		// read range in mm
		this->readRegister(RESULT__RANGE_VAL, mRange);

		// clear interrupt
		this->writeRegister(SYSTEM__INTERRUPT_CLEAR, 0x07);
		return true;
	}
	return false;
}

bool CAbstandssensor::readRange(int& pRange)		// single shot range measurement
{
	// check if device is ready for new measurement
	this->readRegister(RESULT__RANGE_STATUS, mReady);
	if(mReady & 0x01)
	{
		this->writeRegister(SYSRANGE__START, mOperateMode);
	}

//	usleep(5000);

	this->readRegister(RESULT__INTERRUPT_STATUS_GPIO, mInterrupt);

	// check if measurement is done (interrupt flag is set when measurement is complete)
//	if ((mInterrupt & 0x04) == true)
	if (mInterrupt & 0x04)
	{
		// read range in mm
		this->readRegister(RESULT__RANGE_VAL, mRange);

		// clear interrupt
		this->writeRegister(SYSTEM__INTERRUPT_CLEAR, 0x07);
		pRange = mRange;
		return true;
	}

	return false;
}

int CAbstandssensor::readRangeStatus(unsigned char& pStatus)
{
	this->readRegister(RESULT__RANGE_STATUS, mRangeStatus);
	pStatus=(mRangeStatus>>4);
	return 0;
}

int CAbstandssensor::getError()
{
	if (mRangeStatus==VL6180X_ERROR_SYSERR_1)
		cout<<"System Error\n";

	else if (mRangeStatus==VL6180X_ERROR_SYSERR_5)
		cout<<"System Error\n";

	else if (mRangeStatus==VL6180X_ERROR_ECEFAIL)
		cout<<"Early convergence estimate fail\n";

	else if (mRangeStatus==VL6180X_ERROR_NOCONVERGE)
		cout<<"No target detected\n";

	else if (mRangeStatus==VL6180X_ERROR_RANGEIGNORE)
		cout<<"Ignore range reading\n";

	else if (mRangeStatus==VL6180X_ERROR_RAWUFLOW)
		cout<<"Raw reading underflow\n";

	else if (mRangeStatus==VL6180X_ERROR_RAWOFLOW)
		cout<<"Raw reading overflow\n";

	else if (mRangeStatus==VL6180X_ERROR_RANGEUFLOW)
		cout<<"Range reading underflow\n";

	else if (mRangeStatus==VL6180X_ERROR_RANGEOFLOW)
		cout<<"Range reading overflow\n";

	return 0;
}


