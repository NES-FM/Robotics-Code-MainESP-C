#ifndef PCF8574_h
#define PCF8574_h

#include "Wire.h"
#include "logger.h"

#include "Arduino.h"

//#define PCF8574_SOFT_INITIALIZATION

// Uncomment for low latency to get realtime data every time.
#define PCF8574_LOW_LATENCY

#ifdef PCF8574_LOW_LATENCY
	#define READ_ELAPSED_TIME 0
#else
	#define READ_ELAPSED_TIME 10
#endif

// Select an algorithm to manage encoder progression
#define BASIC_ENCODER_ALGORITHM
// #define MISCHIANTI_ENCODER_ALGORITHM
// #define SEQUENCE_ENCODER_ALGORITHM_REDUCED
// #define SEQUENCE_ENCODER_ALGORITHM
// #define POKI_ENCODER_ALGORITHM

//#define P0  	B00000001
//#define P1  	B00000010
//#define P2  	B00000100
//#define P3  	B00001000
//#define P4  	B00010000
//#define P5  	B00100000
//#define P6  	B01000000
//#define P7  	B10000000
//
#define P0  	0
#define P1  	1
#define P2  	2
#define P3  	3
#define P4  	4
#define P5  	5
#define P6  	6
#define P7  	7

#include <math.h>

class PCF8574 {
public:

	PCF8574(uint8_t address);
	PCF8574(uint8_t address, uint8_t interruptPin,  void (*interruptFunction)() );

	PCF8574(TwoWire *pWire, uint8_t address);
	PCF8574(TwoWire *pWire, uint8_t address, uint8_t interruptPin,  void (*interruptFunction)() );

	bool begin();
	void pinMode(uint8_t pin, uint8_t mode, uint8_t output_start = HIGH);

	void encoder(uint8_t pinA, uint8_t pinB);

	void attachInterrupt();
	void detachInterrupt();

	void readBuffer(bool force = true);
	uint8_t digitalRead(uint8_t pin, bool forceReadNow = false);
	struct DigitalInput {
		uint8_t p0;
		uint8_t p1;
		uint8_t p2;
		uint8_t p3;
		uint8_t p4;
		uint8_t p5;
		uint8_t p6;
		uint8_t p7;
	} digitalInput;


	DigitalInput digitalReadAll(void);

	bool digitalWriteAll(PCF8574::DigitalInput digitalInput);
	bool digitalWrite(uint8_t pin, uint8_t value);

#ifdef MISCHIANTI_ENCODER_ALGORITHM
	bool readEncoderValueMischianti(uint8_t pinA, uint8_t pinB, volatile long *encoderValue, bool reverseRotation = false);
	int8_t readEncoderValueMischianti(uint8_t pinA, uint8_t pinB);
#endif
#ifdef POKI_ENCODER_ALGORITHM
	bool readEncoderValuePoki(uint8_t pinA, uint8_t pinB, volatile long *encoderValue, bool reverseRotation = false);
	int8_t readEncoderValuePoki(uint8_t pinA, uint8_t pinB);
#endif

#ifdef SEQUENCE_ENCODER_ALGORITHM
	bool readEncoderValueSequence(uint8_t pinA, uint8_t pinB, volatile long *encoderValue, bool reverseRotation = false);
	int8_t readEncoderValueSequence(uint8_t pinA, uint8_t pinB);
#endif
#ifdef SEQUENCE_ENCODER_ALGORITHM_REDUCED
	bool readEncoderValueSequenceReduced(uint8_t pinA, uint8_t pinB, volatile long *encoderValue, bool reverseRotation = false);
	int8_t readEncoderValueSequenceReduced(uint8_t pinA, uint8_t pinB);
#endif
#ifdef BASIC_ENCODER_ALGORITHM
	bool readEncoderValue(uint8_t pinA, uint8_t pinB, volatile long *encoderValue, bool reverseRotation = false);
	int8_t readEncoderValue(uint8_t pinA, uint8_t pinB);
#endif

	int getLatency() const {
		return latency;
	}

	void setLatency(int latency = READ_ELAPSED_TIME) {
		this->latency = latency;
	}

	uint8_t getTransmissionStatusCode() const {
		return transmissionStatus;
	}

	bool isLastTransmissionSuccess(){
		// DEBUG_PRINT(F("STATUS --> "));
		// DEBUG_PRINTLN(transmissionStatus);
		return transmissionStatus==0;
	}
private:
	uint8_t _address;

	TwoWire *_wire;

	bool _usingInterrupt = false;
	uint8_t _interruptPin = 2;
	void (*_interruptFunction)(){};

	byte writeMode 			= 	0b00000000;
	byte writeModeUp		= 	0b00000000;
	byte readMode 			= 	0b00000000;
	byte readModePullUp 	= 	0b00000000;
	byte readModePullDown 	= 	0b00000000;
	byte byteBuffered 		= 	0b00000000;
	byte resetInitial		= 	0b00000000;
	byte initialBuffer		= 	0b00000000;
	unsigned long lastReadMillis = 0;

	byte writeByteBuffered = 0b00000000;

	volatile byte encoderValues = 0b00000000;

	uint8_t prevNextCode = 0;
	uint16_t store=0;

	int latency = READ_ELAPSED_TIME;

	bool checkProgression(byte oldValA, byte newValA, byte oldValB, byte newValB, byte validProgression);

//	byte validCW = B11100001;
//	byte validCCW = B01001011;
	byte validCW = 0b01001011;
	byte validCCW = 0b11100001;

	uint8_t transmissionStatus = 0;

	void setVal(uint8_t pin, uint8_t value);
	bool digitalWriteAllBytes(byte allpins);
};

#endif

