
#include "cameraControlCmd.h"


bool CameraControl:: CameraMove(CameraProperties camParam)
{
	MStatus stat = M3dView::active3dView().getCamera(camera);
	if(MS::kSuccess == stat){
		MFnCamera fnCamera(camera);						//get the function set of this camera
		MFnTransform fnTransCamera(camera);				//the function set of this camera's transfom
		
		//set this camera's transform
		MVector transVectorInCentimeters(camParam.tranlateX * 100.00, camParam.translateY * 100.00, camParam.translateZ * 100.00);
		fnTransCamera.setTranslation(transVectorInCentimeters, MSpace::kWorld);

		//set this camera's rotation
		fnTransCamera.setRotation(camParam.rotation, MTransformationMatrix::RotationOrder::kXYZ);
	}
	else{
		cerr << "Error to move the camera" <<endl;
	}
	return true;
}