#ifndef ISE_EC_BLE_H
#define ISE_EC_BLE_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <ECSalinity.h>

// https://www.uuidgenerator.net/
#define UFIRE_ISE_UUID "4805d2d0-af9f-42c1-b950-eae78304c408"

#define EC_UUID "ca0331f9-e237-4f81-b9d4-6b2facabfceb"
#define TEMP_UUID "aee115cf-26f0-4096-8914-686b32f123fd"

#define K_UUID "15e657ad-9e9f-4a59-ab2e-fad0f63285a4"
#define OFFSET_UUID "097335d9-60dd-4194-b606-2fdcb9c37330"
#define HIGH_REF_UUID "1dadca6b-3ecc-41bd-a116-f77248975310"
#define HIGH_READ_UUID "e5c4e636-85d9-4da2-a39b-82b5364ea103"
#define LOW_REF_UUID "1baa566e-4657-4080-a580-d236af1c6bd9"
#define LOW_READ_UUID "b2e6fa56-ba50-4913-8b3e-906715dc5a40"
#define TEMP_COMP_UUID "eb245c07-da24-45bd-9d88-5f6e3cc76a23"
#define DUAL_POINT_UUID "374dc054-299c-44a6-8d6f-66e6dd412567"
#define VERSION_UUID "61b9f392-52a9-4127-9048-c130e54f49b4"

class ServerCallback : public BLEServerCallbacks {
public:

  void onConnect(BLEServer *pServer) {}

  void onDisconnect(BLEServer *pServer) {
    ESP.restart();
  }
};

class mSCallback : public BLECharacteristicCallbacks, EC_Salinity {
  void onRead(BLECharacteristic *pCharacteristic) {
    String smV = String(EC_Salinity::measureEC());

    pCharacteristic->setValue(smV.c_str());
    pCharacteristic->notify();
  }
};

class tempCallback : public BLECharacteristicCallbacks, EC_Salinity {
  void onRead(BLECharacteristic *pCharacteristic) {
    String s = String(EC_Salinity::measureTemp());

    pCharacteristic->setValue(s.c_str());
    pCharacteristic->notify();
  }
};

class kCallback : public BLECharacteristicCallbacks, EC_Salinity {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string sK = pCharacteristic->getValue();
    float K        = ::atof(sK.c_str());

    EC_Salinity::setK(K);
  }

  void onRead(BLECharacteristic *pCharacteristic) {
    String sK = String(EC_Salinity::getCalibrateOffset());

    pCharacteristic->setValue(sK.c_str());
  }
};

class offsetCallback : public BLECharacteristicCallbacks, EC_Salinity {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string sOffset = pCharacteristic->getValue();
    float offset        = ::atof(sOffset.c_str());

    EC_Salinity::calibrateProbe(offset, EC_Salinity::tempCoefEC);
  }

  void onRead(BLECharacteristic *pCharacteristic) {
    String sOffset = String(EC_Salinity::getCalibrateOffset());

    pCharacteristic->setValue(sOffset.c_str());
  }
};

class highRefCallback : public BLECharacteristicCallbacks, EC_Salinity {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string shighRef = pCharacteristic->getValue();
    float highRef        = ::atof(shighRef.c_str());

    EC_Salinity::calibrateProbeHigh(highRef, EC_Salinity::tempCoefEC);
  }

  void onRead(BLECharacteristic *pCharacteristic) {
    String sHighRef = String(EC_Salinity::getCalibrateHighReference());

    pCharacteristic->setValue(sHighRef.c_str());
  }
};

class lowRefCallback : public BLECharacteristicCallbacks, EC_Salinity {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string s = pCharacteristic->getValue();
    float f       = ::atof(s.c_str());

    EC_Salinity::calibrateProbeLow(f, EC_Salinity::tempCoefEC);
  }

  void onRead(BLECharacteristic *pCharacteristic) {
    String s = String(EC_Salinity::getCalibrateLowReference());

    pCharacteristic->setValue(s.c_str());
  }
};

class highReadCallback : public BLECharacteristicCallbacks, EC_Salinity {
  void onRead(BLECharacteristic *pCharacteristic) {
    String s = String(EC_Salinity::getCalibrateHighReading());

    pCharacteristic->setValue(s.c_str());
  }
};

class lowReadCallback : public BLECharacteristicCallbacks, EC_Salinity {
  void onRead(BLECharacteristic *pCharacteristic) {
    String s = String(EC_Salinity::getCalibrateLowReading());

    pCharacteristic->setValue(s.c_str());
  }
};

class tcCallback : public BLECharacteristicCallbacks, EC_Salinity {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string s = pCharacteristic->getValue();
    float f       = ::atof(s.c_str());

    EC_Salinity::useTemperatureCompensation(f);
  }

  void onRead(BLECharacteristic *pCharacteristic) {
    String s = String(EC_Salinity::usingTemperatureCompensation());

    pCharacteristic->setValue(s.c_str());
  }
};

class dpCallback : public BLECharacteristicCallbacks, EC_Salinity {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string s = pCharacteristic->getValue();
    float f       = ::atof(s.c_str());

    EC_Salinity::useDualPoint(f);
  }

  void onRead(BLECharacteristic *pCharacteristic) {
    String s = String(EC_Salinity::usingDualPoint());

    pCharacteristic->setValue(s.c_str());
  }
};

class versionCallback : public BLECharacteristicCallbacks, EC_Salinity {
  void onRead(BLECharacteristic *pCharacteristic) {
    String s = String(EC_Salinity::getVersion());

    pCharacteristic->setValue(s.c_str());
  }
};

class uFire_EC_BLE : public EC_Salinity {
public:

  uFire_EC_BLE(uint8_t sda,
               uint8_t scl) : EC_Salinity(sda, scl) {}

  uFire_EC_BLE(uint8_t sda,
               uint8_t scl, uint8_t i2c_address) : EC_Salinity(sda, scl, i2c_address) {}

  void startBLE();
  void measureEC();
  void measureTemp();
  bool connected();

private:

  BLEServer *pServer;
  BLEService *pService;
  BLECharacteristic *pmS_Characteristic;
  BLECharacteristic *poffset_Characteristic;
  BLECharacteristic *ptemp_Characteristic;
  BLECharacteristic *pk_Characteristic;
  BLECharacteristic *phigh_ref_Characteristic;
  BLECharacteristic *plow_ref_Characteristic;
  BLECharacteristic *phigh_read_Characteristic;
  BLECharacteristic *plow_read_Characteristic;
  BLECharacteristic *ptc_Characteristic;
  BLECharacteristic *pdp_Characteristic;
  BLECharacteristic *pversion_Characteristic;
};

#endif // ifndef ISE_EC_BLE_H
