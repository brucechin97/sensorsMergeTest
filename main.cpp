/*
 * main.cpp
 *
 *  Created on: Nov 8, 2019
 *      Author: vmuser
 *
 *      06.12.: 	change CTafelwagen methods to setVelocities() ...
 *      			check CControlComp.cpp
 *
 *
 */

//#include <CControl.h>
#include <iostream>
#include <string>
//#include "CTafelwagen.h"
#include <unistd.h>
#include "CThread.h"
#include "CBinarySemaphore.h"
//#include "CRelativeMoveData.h"
//#include "CAbsoluteMoveData.h"
//#include "CComm.h"
#include "CMausSensor.h"
#include "CMausSensorData.h"
#include "sys/time.h"
#include "CAbstandssensor.h"

// global, DS (shared by Threads)

bool runLoop = true;
extern bool inArea;
//CAbsoluteMoveData absolutMoveData;

CBinarySemaphore cartMoveSem(false, true);		// isFull false --> locked
CBinarySemaphore cartCommandSem(true, true);	// isFull true --> free

extern CMausSensorData mMausSensorData;

int main()
{
//	CComm commControl;
//	CThread commThread{&commControl, CThread::PRIORITY_NORM};
//
//	CControl robotControl;
//	CThread controlThread{&robotControl, CThread::PRIORITY_HIGH};
	struct timeval 				mTik;					// tik for synchronization (20ms)
	struct timeval				mTok;					// tok for synchronization (20ms)
	struct timezone				mTimezone;
	static constexpr Float32 	sTs{0.02F};

	Float32 mRelX, mRelY = 0.0;
	Float32 abs_X, abs_Y, alpha = 0.0;
	Float32 rawX, rawY = 0.0;

	CAbstandssensor mAbstandssensor(2, 0x29);
	CThread abstandssensorThread{&mAbstandssensor, CThread::PRIORITY_LOW};

	CMausSensor mausSensor(Maus_fd);
	CThread mausThread{&mausSensor, CThread::PRIORITY_ABOVE_NORM};

//controlThread.start();
//	commThread.start();
	mausThread.start();
	abstandssensorThread.start();

//	controlThread.join();
//	commThread.join();

	while(true)
		{
			gettimeofday(&mTik, &mTimezone);
			UInt64 tik_abs = mTik.tv_sec * 1000000 + mTik.tv_usec;

			mMausSensorData.getData(mRelX, mRelY);
			rawX += mRelX;
			rawY += mRelY;

			mRelX = mRelX / Faktor_pxToMM;
			mRelY = mRelY / Faktor_pyToMM;
//			alpha = alpha + mRelY / radius;
//			abs_X = abs_X - mRelX * sin(alpha);
//			abs_Y = abs_Y + mRelX * cos(alpha);
			abs_X += mRelX;
			abs_Y += mRelY;

			cout << "raw value X: "<< rawX << "\t raw value Y: "<< rawY<<endl;
			cout<<"Absolute pos. X: "<<abs_X<<"mm\tAbsolute pos. Y = "<<abs_Y<<"mm\t";
			cout<<"Angle: "<< alpha*180/M_PI << alpha << endl;
			cout<<"Robot in drawing area:" << inArea << endl << endl;

			gettimeofday(&mTok, &mTimezone);
			UInt64 tok_abs = mTok.tv_sec * 1000000 + mTok.tv_usec;
			UInt64 sleep = tok_abs - tik_abs;
			cout<<"Execution time:"<<sleep<<endl;
			usleep( (sTs * 1000000 ) - sleep);
		}

	abstandssensorThread.join();
	mausThread.join();
	return 0;
}
