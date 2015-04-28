////////////////////////////////////////////////////////////////////////
//
//  Description:
//      - control the camera move
//       
//
////////////////////////////////////////////////////////////////////////

#ifndef CAMERACONTROLCMD_H
#define CAMERACONTROLCMD_H


#include <maya/MIOStream.h>

#include <maya/MString.h>

#include <maya/MFnCamera.h>
#include <maya/MGlobal.h>
#include <maya/M3dView.h>
#include <maya/MDagPath.h>
#include "VCDataStructs.h"
#include <maya/MFnTransform.h>

struct CameraProperties{
	int frame;
	double tranlateX;
	double translateY;
	double translateZ;
	double rotation[3];
	double fov;

};

class CameraControl
{
public:
	bool SetCamera();
	bool CameraMove(CameraProperties camParam);
private:
	MDagPath camera;		//used to store the activity camera node path
};

#endif