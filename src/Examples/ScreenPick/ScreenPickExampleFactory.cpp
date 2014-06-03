//
//  ScreenPickExampleFactory.cpp
//  ExampleApp
//
//  Created by Scott on 21/05/2014.
//  Copyright (c) 2014 eeGeo. All rights reserved.
//

#include "ScreenPickExampleFactory.h"
#include "ScreenPickExample.h"

using namespace Examples;

ScreenPickExampleFactory::ScreenPickExampleFactory(Eegeo::EegeoWorld& world,
                                                   Eegeo::Camera::GlobeCamera::GlobeCameraController& globeCameraController)
: m_world(world)
, m_globeCameraController(globeCameraController)
{
    
}

IExample* ScreenPickExampleFactory::CreateExample() const
{
    return new Examples::ScreenPickExample(m_world.GetRenderContext(),
                                           m_world.GetCameraProvider(),
                                           m_world.GetTerrainHeightProvider(),
                                           m_world.GetCollisionMeshResourceProvider(),
                                           m_globeCameraController);
}

std::string ScreenPickExampleFactory::ExampleName() const
{
	return Examples::ScreenPickExample::GetName();
}
