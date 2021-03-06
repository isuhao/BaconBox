/**
 * @file
 * @ingroup Input
 */
#ifndef BB_ACCELEROMETER_H
#define BB_ACCELEROMETER_H

#include <sigly.h>

#include "BaconBox/Input/InputDevice.h"
#include "BaconBox/Input/Accelerometer/AccelerometerSignalData.h"
#include "BaconBox/Input/Accelerometer/AccelerometerState.h"
#include "BaconBox/vmath.h"

namespace BaconBox {
	/**
	 * Represents an accelerometer device. Can be activated or deactivated at
	 * will because it can use up a lot of battery on some platforms.
	 * @ingroup Input
	 */
	class Accelerometer : public InputDevice {
		friend class InputManager;
	public:
		/**
		 * Signal sent when the accelerometer's values change, which happens
		 * very frequently (pretty close to every update).
		 */
		sigly::Signal1<AccelerometerSignalData> change;
		
		/**
		 * Connect a function callback to the buttonPress signal.
		 * @param pclass Pointer to the instance concerned by the signal.
		 * @param pmemfun Pointer to the function to call.
		 */
		template <class desttype>
		static void connectChange(desttype* pclass, void (desttype::*pmemfun)(AccelerometerSignalData)) {
			Accelerometer* accelerometer = getDefault();
			if(accelerometer) {
				accelerometer->change.connect(pclass, pmemfun);
			}
		}

		/**
		 * Gets a pointer to the default accelerometer device.
		 * @return Pointer to the default accelerometer device. Null if none are
		 * available.
		 */
		static Accelerometer* getDefault();

		/**
		 * Activates the signal sending when the accelerometer's value are
		 * updated.
		 */
		void activateSignals();
		
		/**
		 * Deactivates the signal sending when the accelerometer's value are
		 * updated.
		 */
		void deactivateSignals();
		
		/**
		 * Checks if the signal emitting is active or not.
		 * @return True if the accelerometer is emitting signals, false if not.
		 */
		bool areSignalsActive() const;
		
		/**
		 * Activates the updating of the accelerometer. When activated, the
		 * accelerometer fetches data about it at every update.
		 */
		virtual void activate();
		
		/**
		 * Deactivates the updating of the accelerometer. When deactivated, the
		 * acceleration values of the accelerometer's state are never updated.
		 */
		virtual void deactivate();
		
		/**
		 * Checks if the accelerometer is active.
		 * @return True if the accelerometer is updating its acceleration
		 * values, false if not.
		 */
		bool isActive() const;
		
		/**
		 * Gets the accelerometer's state.
		 * @return Accelerometer's state containing its acceleration values.
		 */
		const AccelerometerState& getState() const;
	
		const vmath::Vector3f getCalibratedAcceleration();
		void calibrateFromSelf();
		void calibrateFromVector(const vmath::Vector3f &vec);
		const vmath::Vector3f & getAcceleration();
		void setCalibrationMatrix(const vmath::Matrix4f & calibrationMatrix);
		const vmath::Matrix4f & getCalibrationMatrix();

	protected:
		VMATH_NAMESPACE::Matrix4f calibrationMatrix;
		/// Accelerometer's state containing its acceleration values (x, y z).
		AccelerometerState state;
		void setState(float xAcceleration, float yAcceleration, float zAcceleration);
		
		/**
		 * Default constructor.
		 */
		Accelerometer();
		
		/**
		 * Destructor.
		 */
		virtual ~Accelerometer();

		/**
		 * Abstract update method. This will be called to update the device's
		 * state.
		 */
		virtual void updateDevice();
		
	private:
		
		/**
		 * Is set at true if the accelerometer is actively updating its
		 * acceleration values.
		 */
		bool active;
		
		/**
		 * Is set to true if the accelerometer is sending signals each time it
		 * updates its acceleration values.
		 */
		bool signalsActive;
	};
}

#endif
