#include "cameraControlCmd.h"
//#include "MeltNode.h"
#include <maya/MSelectionList.h>
#include <maya/MGlobal.h>
#include <maya/MDagPath.h>
#include <maya/MFnTransform.h>
#include <maya/MItSelectionList.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MTime.h>
#include <maya/MAnimControl.h>
#include <assert.h>
#include <maya/M3dView.h>
#include <maya/MIOStream.h>
#include <maya/MFnCamera.h>


MStatus cameraControlCmd::doIt ( const MArgList &args )
{ 
	//initial to get the activity camera dag-path
	MStatus stat = M3dView::active3dView().getCamera(camera);

	if( MS::kSuccess == stat){
		redoIt();
	}
	else{
		cerr<< "error to getting the camera"<<endl;
	}

	return stat;

}

MStatus cameraControlCmd::undoIt()
{
	return dgMod.undoIt();
}


MStatus cameraControlCmd::redoIt()
{
	MPoint cameraPos(0.0, 0.0, 0.0, 1.0);
	MVector cameraDir(0.0, 1.0, 0.0);
	MVector wsUpDir(0.0, 1.0, 0.0);
	double horizFieldOfView = 100;


	

	MFnCamera fnCamera(camera);
	double aspectRaio = fnCamera.aspectRatio();

	fnCamera.set(cameraPos, cameraDir, wsUpDir, horizFieldOfView, aspectRaio);


	return dgMod.doIt();
}

