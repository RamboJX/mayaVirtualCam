#include "cmdDispatch.h"


#include <maya/MGlobal.h>
#include <maya/MString.h>
#include <maya/MStringArray.h>
#include <maya/MPoint.h>
#include <maya/MVector.h>
#include <maya/MTime.h>
#include <maya/MDagPath.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnCamera.h>
#include <maya/MFnAnimCurve.h>
#include <maya/M3dView.h>
#include <maya/MQuaternion.h>
#include <maya/MEulerRotation.h>

#define MAX_CMD_LENGTH 16
#define MAX_PARAM_LENGTH 16

using namespace std;

Server cmdDispatch::m_tcpServ;
cmdUtil cmdDispatch::m_cmdUtil;
MDagPath cmdDispatch::cameraToSet;
//cameraAnimCurveFn cmdDispatch::activeCameraACFn;


MStatus cmdDispatch::doIt ( const MArgList &args )
{ 
	cout<<"i am sure to here!!!!";
	//initial to get the activity camera dag-path
	MStatus stat = M3dView::active3dView().getCamera(cameraToSet);
		/*
	if( MStatus::kSuccess == m_cmdUtil.scanDag( MItDag::kBreadthFirst, MFn::kCamera, false))
	{
		for(int i = 0; i < m_cmdUtil.cameraDagArray.length() ; i++ )
		{
			MFnDagNode dagNode(m_cmdUtil.cameraDagArray[i]);
			if( dagNode.name() == "testShape")
			{
				cameraToSet = m_cmdUtil.cameraDagArray[i];
			}
		}
		//	cameraToSet = m_cmdUtil.cameraDagArray[4];
		MString camerapath = cameraToSet.fullPathName();
		MGlobal::displayInfo(camerapath);
		
		initCameraAnimCurve();
		this->tcpRoutine();
	}
	else
	{
		cout<<"scan camera error";
		return MStatus::kFailure;
	}*/

	//create a camera to key the trace

	//MStatus status;
	//MString camera_name;
	//camera_name = "test";
	//MFnCamera cameraFn;
	//MObject camera = cameraFn.create(&status);
	//if (status == MS::kFailure)
	//	printf("Error creating the camera.\n");

	//// Create various objects to access the cameras components.
	//MFnDagNode fnSet( camera, &status);
	//if ( MS::kSuccess != status )
	//	printf("Failure to create function set\n");
	////MDagPath dp;
	//if (cameraFn.getPath(cameraToSet) != MS::kSuccess) 
	//	printf("Failure creating dagpath\n");

	//// set the name of the camera
	//if (camera_name.length() > 0) {
	//	fnSet.setName(camera_name, &status);
	//	if (status == MS::kFailure) printf("Error setting the camera name.\n");
	//}

	this->tcpRoutine();
	return MStatus::kSuccess;
}


MStatus cmdDispatch::tcpRoutine()
{
	//build a socket
	m_tcpServ.CSocket();
	//bind the socket
	m_tcpServ.CBind();
	m_tcpServ.CListen();
	HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)answerThread, NULL, 0, NULL);
	if(hThread == NULL)
	{
		MGlobal::displayError( MString( "create thread error" ) );	
		m_tcpServ.CClose();
		return MStatus::kFailure;
	}
	return MStatus::kSuccess;
}


bool cmdDispatch::answerThread(){

//======================first intial the camera fn=============================================
	printf("\nRunning cameraPath.\n");
//	int i;
	MString camera_name, file_name;

	// Parse the arguments.
	camera_name = "test";
	// check to see if a file was specified.

	MStatus status;
	MFnCamera cameraFn;

	// create the camera
	MObject camera = cameraFn.create(&status);
	if (status == MS::kFailure) printf("Error creating the camera.\n");


	// Create various objects to access the cameras components.
	MFnDagNode fnSet( camera, &status);
	if ( MS::kSuccess != status ) printf("Failure to create function set\n");
	MDagPath dp;
	if (cameraFn.getPath(dp) != MS::kSuccess) printf("Failure creating dagpath\n");

	// set the name of the camera
	if (camera_name.length() > 0) {
		fnSet.setName(camera_name, &status);
		if (status == MS::kFailure) printf("Error setting the camera name.\n");
	}

	// The MFnAnimCurves are the objects that "inject" a value into the correct attribute of 
	// the camera for each frame.

	// Create the MFnAnimCurves
	MString attrName( "translateX");
	const MObject attrtx = fnSet.attribute (attrName, &status);
	if ( MS::kSuccess != status ) printf( "Failure to find attribute translateX\n");

	MFnAnimCurve acFnSettx;
	acFnSettx.create ( dp.transform(), attrtx, NULL, &status);
	if ( MS::kSuccess != status ) printf("Failure creating MFnAnimCurve function set (translateX)\n");

	attrName.set( "translateY");
	const MObject attrty = fnSet.attribute (attrName, &status);
	if ( MS::kSuccess != status ) printf( "Failure to find attribute translateY\n");

	MFnAnimCurve acFnSetty;
	acFnSetty.create ( dp.transform(), attrty, NULL, &status);
	if ( MS::kSuccess != status ) printf("Failure creating MFnAnimCurve function set (translateX)\n");

	attrName.set( "translateZ");
	const MObject attrtz = fnSet.attribute (attrName, &status);
	if ( MS::kSuccess != status ) printf( "Failure to find attribute translateZ\n");

	MFnAnimCurve acFnSettz;
	acFnSettz.create ( dp.transform(), attrtz, NULL, &status);
	if ( MS::kSuccess != status ) printf("Failure creating MFnAnimCurve function set (translateY)\n");


	attrName.set( "rotateX");
	const MObject attrrx = fnSet.attribute (attrName, &status);
	if ( MS::kSuccess != status ) printf( "Failure to find attribute rotateX\n");

	MFnAnimCurve acFnSetrx;
	acFnSetrx.create ( dp.transform(), attrrx, NULL, &status);
	if ( MS::kSuccess != status ) printf("Failure creating MFnAnimCurve function set (translateZ)\n");


	attrName.set( "rotateY");
	const MObject attrry = fnSet.attribute (attrName, &status);
	if ( MS::kSuccess != status ) printf( "Failure to find attribute rotateY\n");

	MFnAnimCurve acFnSetry;
	acFnSetry.create ( dp.transform(), attrry, NULL, &status);
	if ( MS::kSuccess != status ) printf("Failure creating MFnAnimCurve function set (rotateY)\n");


	attrName.set( "rotateZ");
	const MObject attrrz = fnSet.attribute (attrName, &status);
	if ( MS::kSuccess != status ) printf( "Failure to find attribute rotateZ\n");

	MFnAnimCurve acFnSetrz;
	acFnSetrz.create ( dp.transform(), attrrz, NULL, &status);
	if ( MS::kSuccess != status ) printf("Failure creating MFnAnimCurve function set (rotateZ)\n");



	//// Create the MFnAnimCurves for camera shape.

	//MFnDagNode fnSetcam( dp, &status);
	//if ( MS::kSuccess != status ) printf("Failure to create camera function set.\n");

	//attrName.set( "horizontalFilmAperture" );
	//const MObject attrhfa = fnSetcam.attribute( attrName, &status);
	//if ( MS::kSuccess != status ) printf( "Failure to find attribute hfa\n");

	//MFnAnimCurve acFnSethfa;
	//acFnSethfa.create ( dp.node(), attrhfa, NULL, &status);
	//if ( MS::kSuccess != status ) printf("Failure creating MFnAnimCurve function set hfa\n");

	//attrName.set( "verticalFilmAperture" );
	//const MObject attrvfa = fnSetcam.attribute( attrName, &status);
	//if ( MS::kSuccess != status ) printf( "Failure to find attribute vfa\n");

	//MFnAnimCurve acFnSetvfa;
	//acFnSetvfa.create ( dp.node(), attrvfa, NULL, &status);
	//if ( MS::kSuccess != status ) printf("Failure creating MFnAnimCurve function set vfa\n");

	//attrName.set( "focalLength" );
	//const MObject attrfl = fnSetcam.attribute( attrName, &status);
	//if ( MS::kSuccess != status ) printf( "Failure to find attribute fl\n");

	//MFnAnimCurve acFnSetfl;
	//acFnSetfl.create ( dp.node(), attrfl, NULL, &status);
	//if ( MS::kSuccess != status ) printf("Failure creating MFnAnimCurve function set focal length\n");

	//attrName.set( "fStop" );
	//const MObject attrfs = fnSetcam.attribute( attrName, &status);
	//if ( MS::kSuccess != status ) printf( "Failure to find attribute fstop\n");

	//MFnAnimCurve acFnSetfs;
	//acFnSetfs.create ( dp.node(), attrfs, NULL, &status);
	//if ( MS::kSuccess != status ) printf("Failure creating MFnAnimCurve function set fstop\n");

	//attrName.set( "focusDistance" );
	//const MObject attrfd = fnSetcam.attribute( attrName, &status);
	//if ( MS::kSuccess != status ) printf( "Failure to find attribute fd\n");

	//MFnAnimCurve acFnSetfd;
	//acFnSetfd.create ( dp.node(), attrfd, NULL, &status);
	//if ( MS::kSuccess != status ) printf("Failure creating MFnAnimCurve function set fd\n");

	while(1)
	{
		while( m_tcpServ.CAccept() != 1 )	// when accept a new connect
			;
		while( strcmp(m_tcpServ.bufRecv , "exit")!=0 )
		{
			if( m_tcpServ.CRecv() == 1)
			{
			//===	parseAndDispatch();==========================

				MString cmdLine(m_tcpServ.bufRecv);
				MStringArray cmdWords;

				cmdLine.split(';',cmdWords);

				cout<<"this is the received command line:";
				cout<<cmdLine<<endl;
				//=============parse====================
				if( cmdWords[0] == "camera")
				{
					//==============setCamera(cmdWords, cameraToSet);===============================
						double timeFrame  = cmdWords[TIME_FRAME].asDouble();
						cameraParam cameraPara;
						cameraPara.tranlateX = cmdWords[TRANS_X].asFloat();
						cameraPara.translateY = cmdWords[TRANS_Y].asFloat();
						cameraPara.translateZ = cmdWords[TRANS_Z].asFloat();

						//from quaternion to Euler
						double quat_x = cmdWords[QUATERNION_X].asFloat();
						double quat_y = cmdWords[QUATERNION_Y].asFloat();
						double quat_z = cmdWords[QUATERNION_Z].asFloat();
						double quat_w = cmdWords[QUATERNION_W].asFloat();

						MQuaternion quater(quat_x, quat_y, quat_z, quat_w);
						MEulerRotation euler = quater.asEulerRotation();

						cameraPara.rotateX = euler.x;
						cameraPara.rotateY = euler.y;
						cameraPara.rotateZ = euler.z;


						//MGlobal::viewFrame(mTimeFrame);


						acFnSettx.addKeyframe(MTime(timeFrame,MTime::uiUnit()),cameraPara.tranlateX);
						acFnSetty.addKeyframe(MTime(timeFrame,MTime::uiUnit()),cameraPara.translateY);
						acFnSettz.addKeyframe(MTime(timeFrame,MTime::uiUnit()),cameraPara.translateZ);

						acFnSetrx.addKeyframe(MTime(timeFrame,MTime::uiUnit()),cameraPara.rotateX);
						acFnSetry.addKeyframe(MTime(timeFrame,MTime::uiUnit()),cameraPara.rotateY);
						acFnSetrz.addKeyframe(MTime(timeFrame,MTime::uiUnit()),cameraPara.rotateZ);

						//key camera trace
							// add a key frame for each paramater to the correct MFnAnimCurve
					/*	activeCameraACFn.acFnSetTx.addKeyframe(mTimeFrame,cameraPara.tranlateX);
						activeCameraACFn.acFnSetTy.addKeyframe(mTimeFrame,cameraPara.translateY);
						activeCameraACFn.acFnSetTz.addKeyframe(mTimeFrame,cameraPara.translateZ);
						activeCameraACFn.acFnSetRx.addKeyframe(mTimeFrame,cameraPara.rotateX);
						activeCameraACFn.acFnSetRy.addKeyframe(mTimeFrame,cameraPara.rotateY);
						activeCameraACFn.acFnSetRz.addKeyframe(mTimeFrame,cameraPara.rotateZ);*/
						//activeCameraACFn.acFnSetHfa.addKeyframe(mTimeFrame,cdata.hfa);
						//activeCameraACFn.acFnSetVfa.addKeyframe(mTimeFrame,cdata.vfa);
						//activeCameraACFn.acFnSetFl.addKeyframe(mTimeFrame,cdata.fl);
						//activeCameraACFn.acFnSetFs.addKeyframe(mTimeFrame,cdata.fstop);
						//activeCameraACFn.acFnSetFd.addKeyframe(mTimeFrame,cdata.fd);
	
						//set the camera status for the cmd received
						//m_cmdUtil.cameraSet(wsEyeLocation, wsViewDirection, wsUpDirection, horizFieldOfView, aspectRatio);
						//MGlobal::viewFrame(mTimeFrame);
				}
				else 
					MGlobal::displayInfo( cmdLine );


			}
			else if( m_tcpServ.CRecv() == -1 ) // receive over, client close
			{
				break;
			}
			//else receive the next line; ok?
		}
		//m_tcpServ.CSend();
	}
}


//
//bool cmdDispatch::setCamera(MStringArray cmdWords, MDagPath camera)
//{
//	//double timeFrame  = cmdWords[TIME_FRAME].asDouble();
//	//MPoint wsEyeLocation(cmdWords[LOC_X].asDouble(), cmdWords[LOC_Y].asDouble(), cmdWords[LOC_Z].asDouble(), 1.0);
//	//MVector wsViewDirection(cmdWords[VIEW_DIR_X].asDouble(), cmdWords[VIEW_DIR_Y].asDouble(), cmdWords[VIEW_DIR_Z].asDouble());
//	//MVector wsUpDirection(cmdWords[UP_DIR_X].asDouble(), cmdWords[UP_DIR_Y].asDouble(), cmdWords[UP_DIR_Z].asDouble());
//
//	//double horizFieldOfView = cmdWords[HOV].asDouble();
//	//double aspectRatio = cmdWords[ASPECTRATIO].asDouble();
//
//
//	return true;
//}
