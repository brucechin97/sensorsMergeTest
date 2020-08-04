/*
 * CMaus.h
 *
 *  Created on: Dec 20, 2019
 *      Author: Bruce Chin
 */

#ifndef CMAUSSENSORDATA_H_
#define CMAUSSENSORDATA_H_

#include "Global.h"
#include <cmath>					// sinus Funktion
#define _USE_MATH_DEFINES			// Definierter Wert von Pi
#include <math.h>
#include <pthread.h>

using namespace std;

#define radius 44.5				// Abstand zwischen Maussensor und Rotationsachse [mm]
//#define Faktor_pxToMM -34.9
//#define Faktor_pyToMM 34.058
#define Faktor_pxToMM -46.33
#define Faktor_pyToMM 76.229

class CMausSensorData
{
public:
	CMausSensorData();
	CMausSensorData(const CMausSensorData&) = delete;
	CMausSensorData operator = (const CMausSensorData&) = delete;
	~CMausSensorData() = default;
	int setData(Float32 pX, Float32 pY);
	int getData(Float32& pX, Float32& pY);

/*public:
static constexpr Float32 radius{44.5F};					// Abstand zwischen Maussensor und Rotationsachse [mm]
static constexpr Float32 Faktor_pxToMM{34.9F};
static constexpr Float32 Faktor_pyToMM{-34.058F};
*/

private:
	Float32 mRelX;
	Float32 mRelY;
	pthread_mutex_t mMutex;
};



#endif /* CMAUS_H_ */
