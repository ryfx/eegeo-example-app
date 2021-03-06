// Copyright eeGeo Ltd (2012-2016), All Rights Reserved

#pragma once

#import <Foundation/Foundation.h>
#include "IndoorAtlasLocationService.h"
#include "iOSAlertBoxFactory.h"
#include "ISingleOptionAlertBoxDismissedHandler.h"
#include <map>
#include <string>

@interface IndoorAtlasLocationManagerObjC : NSObject

-(instancetype) init: (ExampleApp::InteriorsPosition::SdkModel::IndoorAtlas::IndoorAtlasLocationService*) indoorAtlasLocationService
  iOSAlertBoxFactory: (Eegeo::UI::NativeAlerts::iOS::iOSAlertBoxFactory*) iOSAlertBoxFactory;
-(void) startUpdatingLocation: (NSString*) apiKey
                    apiSecret: (NSString*) apiSecret
                     floorMap: (std::map<int, std::string>) floorMap;
-(void) stopUpdatingLocation;

@end
