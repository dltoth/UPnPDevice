/**
 *
 */
 
#ifndef CUSTOMDEVICE_H
#define CUSTOMDEVICE_H

#include <UPnPDevice.h>

/** Leelanau Software Company namespace 
*  
*/
using namespace lsc;

class CustomDevice : public UPnPDevice {
  public:
    CustomDevice() :  UPnPDevice("deviceTarget") {setDisplayName("Custom Device");};
    CustomDevice(const char* target) : UPnPDevice(target) {setDisplayName("Custom Device");};

    DEFINE_RTTI;
    DERIVED_TYPE_CHECK(UPnPDevice);
    DEFINE_EXCLUSIONS(CustomDevice);         
};

class CustomService : public UPnPService {
  public:
    CustomService() :  UPnPService("serviceTarget") {setDisplayName("Custom Service");};
    CustomService(const char* target) : UPnPService(target) {setDisplayName("Custom Service");};

    DEFINE_RTTI;
    DERIVED_TYPE_CHECK(UPnPService);
    DEFINE_EXCLUSIONS(CustomService);         
};

#endif
