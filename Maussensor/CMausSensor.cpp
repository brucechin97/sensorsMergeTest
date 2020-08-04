/*
 * CMausSensor.cpp
 *
 *  Created on: Feb 19, 2020
 *      Author: vmuser
 */

#include "CMausSensor.h"
#include "CMausSensorData.h"
//#include "CMaus.h"

extern bool runLoop;
CMausSensorData mMausSensorData;				// globales Objekt Container für die Kommunikation zwischen Threads
//CMaus Sensor_Maus(Maus_fd);

CMausSensor::CMausSensor(const string& device)
: mDeltaX(0), mDeltaY(0)
{
	mDevice = device;

	if((fd = open(mDevice.c_str(), O_RDONLY)) < 0)
		{
			cout << "Failed to open " << mDevice << endl;
		}
	fds.fd = fd;
	fds.events = POLLIN | POLLPRI;
}

CMausSensor::~CMausSensor()
{
	close(fd);
}

void CMausSensor::init()
{
	cout << "[*] CMausSensor: init . . . " << endl;
}

void CMausSensor::run()
{
	cout << "[*] CMausSensor: run . . . " << endl;
	while(runLoop)
	{
//		if(poll(&fds, 1, 5) > 0){
	//		if(fds.revents & POLLIN){
			if (read(fd, &ie, sizeof(struct input_event)))	// read Event Handler
			{
					if(ie.type == EV_REL)		// if Event Type = relative Bewegung
					{
						switch (ie.code)
						{
						case REL_X:				// Event war eine relative Bewegung in x-Achse
							mDeltaX = ie.value;
							break;

						case REL_Y:				// Event war eine relative Bewegung in y-Achse
							mDeltaY  = ie.value;
							break;

						default:
							cout<<"No relative movement.\n";
						}
						mMausSensorData.setData(mDeltaX, mDeltaY);
					}
			}
	}
}
