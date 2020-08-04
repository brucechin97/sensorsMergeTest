/*
 * CMausSensor.h
 *
 *  Created on: Feb 19, 2020
 *      Author: vmuser
 */

#ifndef HARDWARE_CMAUSSENSOR_H_
#define HARDWARE_CMAUSSENSOR_H_

#include "IRunnable.h"
#include "Global.h"
#include <string>
#include <iomanip>
#include <vector>
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <linux/input.h>
#include <cstdlib>
#include <iostream>
using namespace std;
#include <poll.h>

#define Maus_fd "/dev/input/event1" 		// File Descriptor fuer die Maus (vorher ueberpruefen, welcher Event Handler die Maus hat)

class CMausSensor : public IRunnable
{
public:
	void init();
	void run();

public:
	CMausSensor(const string& device);
	CMausSensor(const CMausSensor&) = delete;				// Constructor of this kind disabled
	CMausSensor& operator = (const CMausSensor&) = delete;	// assignment operator disabled
	~CMausSensor();

private:
	string mDevice;
	int fd = -1;
	struct input_event ie;
	struct pollfd fds;
	Float32 mDeltaX;
	Float32 mDeltaY;
};



#endif /* HARDWARE_CMAUSSENSOR_H_ */
