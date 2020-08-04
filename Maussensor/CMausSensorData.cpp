/*
 * CMaus.cpp
 *
 *  Created on: Feb 7, 2020
 *      Author: Bruce Chin
 */

#include "CMausSensorData.h"

CMausSensorData::CMausSensorData()
: mRelX(0.0), mRelY(0.0)
{
}

int CMausSensorData::setData(Float32 pX, Float32 pY)
{
	pthread_mutex_lock(&mMutex);
	mRelX += pX;
	mRelY += pY;
	pthread_mutex_unlock(&mMutex);
	return 0;
}

int CMausSensorData::getData(Float32& pX, Float32& pY)			// kann alle 20ms aufgerufen werden
{
	pthread_mutex_lock(&mMutex);
	pX = mRelX;
	pY = mRelY;
	mRelX = 0;
	mRelY = 0;
	pthread_mutex_unlock(&mMutex);
	return 0;
}

