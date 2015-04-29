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

#include <maya/MPxCommand.h>
#include <maya/MFnPlugin.h>
#include <maya/MString.h>
#include <maya/MArgList.h>

#include <maya/MFnCamera.h>
#include <maya/MGlobal.h>
#include <maya/M3dView.h>
#include <maya/MDagPath.h>


struct CamParams{
	int frame;
	double tranlateX;
	double translateY;
	double translateZ;
	double rotation[3];
	double fov;

};


class cameraControlCmd : public MPxCommand
{
public:
	cameraControlCmd() {};
	virtual			~cameraControlCmd(){ }; 
	MStatus			doIt( const MArgList& args );
	
	static void*	creator(){ return new cameraControlCmd();};

private:
	// This is data that is necessary to redo/undo the command.
	//
	MDagPath		camera;
};


#endif