/*
|| create by rambo
*/

#include "tcpServerTestCmd.h"

serv tcpServerTestCmd::mayaTcpServer;
MDagPath tcpServerTestCmd::camera;
///class serv
//this function is a new thread to listen to a client and read command or sent command to the client
//when this function end the connection will gracefully closed.
void serv::on_connect(connection& new_connection)
{
	//////////////////
	//
	//init the MFnAnimCurve variables
	///////////////////
	MStatus status;
	//Init key the camera frame's variables

	MString cameraName = "test";
	cout << "new a new camera to store the keyframes. Camera name is test. "  << endl;
	MFnCamera cameraFn;		//this is use the camerafn to create a new camera, and we can also use the camera to get MFnCamera
	/*
		TODO:
		    MDagPath camera;
			M3dView::active3dView().getCamera(camera);
			MFnCamera cameraFn(camera);
	*/
	MObject cameraObj = cameraFn.create(&status);
	MFnDagNode cameraFnDagNode(cameraObj, &status);
	MDagPath cameraDagPath;
	cameraFn.getPath(cameraDagPath);
	cameraFn.setName(cameraName, &status);

	//get thecamera transform for move
	MFnTransform fnTransCamera(cameraDagPath);				//the function set of this camera's transfom
	MObject camLowestTransObj =  cameraDagPath.transform();
	MFnTransform camTrans(camLowestTransObj);
	MVector translationVector;
		
	// The MFnAnimCurves are the objects that "inject" a value into the correct attribute of 
	// the camera for each frame.
	
	cout<< "Initialize the key frame variables :"<<endl;
	// fn.create (tranform, object, ...)

	MFnAnimCurve fnAnimCurves_tx;
	MFnAnimCurve fnAnimCurves_ty;
	MFnAnimCurve fnAnimCurves_tz;
	MFnAnimCurve fnAnimCurves_rx;
	MFnAnimCurve fnAnimCurves_ry;
	MFnAnimCurve fnAnimCurves_rz;

	MString attrName( "translateX");
	const MObject attributeMObjectTx = cameraFnDagNode.attribute (attrName, &status);		//this variable must be const!
	fnAnimCurves_tx.create ( cameraDagPath.transform(), attributeMObjectTx, NULL, &status);	//transform() method can get the lowest transform of this camera

	attrName.set( "translateY");
	const MObject attributeMObjectTy = cameraFnDagNode.attribute (attrName, &status);
	fnAnimCurves_ty.create ( cameraDagPath.transform(), attributeMObjectTy, NULL, &status);

	attrName.set( "translateZ");
	const MObject attributeMObjectTz = cameraFnDagNode.attribute (attrName, &status);
	fnAnimCurves_tz.create ( cameraDagPath.transform(), attributeMObjectTz, NULL, &status);

	attrName.set( "rotateX");
	const MObject attributeMObjectRx = cameraFnDagNode.attribute (attrName, &status);
	fnAnimCurves_rx.create ( cameraDagPath.transform(), attributeMObjectRx, NULL, &status);

	attrName.set( "rotateY");
	const MObject attributeMObjectRy = cameraFnDagNode.attribute (attrName, &status);
	fnAnimCurves_ry.create ( cameraDagPath.transform(), attributeMObjectRy, NULL, &status);

	attrName.set( "rotateZ");
	const MObject attributeMObjectRz = cameraFnDagNode.attribute (attrName, &status);
	fnAnimCurves_rz.create ( cameraDagPath.transform(), attributeMObjectRz, NULL, &status);


	//store the connection
	clientCams = &new_connection;

	cout << "the ip of the new client connection is:"<< new_connection.get_foreign_ip()<<endl;
	
	bool bLineEnd = false;		//To mark if received /n
	char buffer;				//Read a word from the connection every time.
	int iDataLen = 0;			//the datalength
	memset(bufferCmdLine, 0, MAX_COMMAND_LINE);	//refresh the command line to 0
	//recieve command from the connection, parse cmd and do sth
	while (new_connection.read(&buffer,1) > 0)
    {
		//first read this alphabetic to bufferCmdLine; 
		bufferCmdLine[iDataLen++] = buffer;
		//find out if is a line end, Parse this command line
		if('\n' == buffer)
		{
			MString cmdLine (bufferCmdLine);
			MStringArray cmdWords;

			cout << cmdLine << endl;
			cmdLine.split(';', cmdWords);

			if(cmdWords[0] == "camera")
			{
				//set camera keyframe;
				CameraProperties camParam;
				camParam.frame = cmdWords[1].asDouble();
			
				camParam.translateX = -cmdWords[2].asDouble() ;	//flip the translate x
				camParam.translateY = cmdWords[3].asDouble() ;
				camParam.translateZ = cmdWords[4].asDouble() ;
				double qx = cmdWords[5].asDouble();
				double qy = cmdWords[6].asDouble();
				double qz = cmdWords[7].asDouble();
				double qw = cmdWords[8].asDouble();
				camParam.fov = cmdWords[9].asDouble();
				//transfer this quaternion value to right hand euler
				
				transQuaternionFromLeftToRightHand(qx, qy, qz, qw);
				MQuaternion rotateQuaternion(qx, qy, qz, qw);
				//this may has some problem
				MEulerRotation rotateEuler = rotateQuaternion.asEulerRotation();
				camParam.eulerRotation[0] = rotateEuler.x;
				camParam.eulerRotation[1] = rotateEuler.y;
				camParam.eulerRotation[2] = rotateEuler.z;
	
				fnAnimCurves_tx.addKeyframe(MTime(camParam.frame, MTime::uiUnit()), camParam.translateX );
				fnAnimCurves_ty.addKeyframe(MTime(camParam.frame, MTime::uiUnit()), camParam.translateY );
				fnAnimCurves_tz.addKeyframe(MTime(camParam.frame, MTime::uiUnit()), camParam.translateZ );
				fnAnimCurves_rx.addKeyframe(MTime(camParam.frame, MTime::uiUnit()), camParam.eulerRotation[0] );
				fnAnimCurves_ry.addKeyframe(MTime(camParam.frame, MTime::uiUnit()), camParam.eulerRotation[1] );
				fnAnimCurves_rz.addKeyframe(MTime(camParam.frame, MTime::uiUnit()), camParam.eulerRotation[2] );
				
				/////////move the camera
				//tcpServerTestCmd::CameraMove(camParam);
				//MFnCamera fnCamera(camera);						//get the function set of this camera
				cout << "camera path :" << cameraFn.fullPathName() << endl;
				//set this camera's transform
				translationVector[0] = camParam.translateX;
				translationVector[1] = camParam.translateY;
				translationVector[2] = camParam.translateZ;
				//MVector transVectorInCentimeters(camParam.translateX*100, camParam.translateY*100, camParam.translateZ*100);

				//setTranslation (const MVector &vec, MSpace::Space space)
				status = camTrans.setTranslation(translationVector, MSpace::kTransform);		//send the reference of the first parameter
				if(status == MStatus::kSuccess){
					cout <<"this translation is success" << endl;
				}
				//set this camera's rotation
				camTrans.setRotation(camParam.eulerRotation, MTransformationMatrix::RotationOrder::kXYZ);

			}
			else
			{
				;
			}
			//refresh the bufferCmdLine, and iDataLen.
			memset(bufferCmdLine, 0, MAX_COMMAND_LINE);
			iDataLen = 0;
		}

    }

	
}


void serv::transQuaternionFromLeftToRightHand(double &x, double &y, double &z, double &w)
{
	MQuaternion leftHandRotation(x, y, z, w);
	MMatrix leftRotationMatrix =  leftHandRotation.asMatrix();
	/*
	double transMatrixSx[4][4] = {-1, 0, 0, 0,
									0, 1, 0, 0,
									0, 0, 1, 0,
									0, 0, 0, 1};
	double transMatrixSz[4][4] = {1, 0, 0, 0,
									0, 1, 0, 0,
									0, 0, -1, 0,
									0, 0, 0, 1};
	MMatrix transferMatrixSx(transMatrixSx);
	MMatrix transferMatrixSz(transMatrixSz);
	MTransformationMatrix righthandRotationMatrix = transferMatrixSx * leftRotationMatrix * transferMatrixSz;
	*/

	leftRotationMatrix[0][0] = -leftRotationMatrix[0][0];
	leftRotationMatrix[1][0] = -leftRotationMatrix[1][0];
	leftRotationMatrix[3][0] = -leftRotationMatrix[3][0];
	leftRotationMatrix[2][1] = -leftRotationMatrix[2][1];
	leftRotationMatrix[2][2] = -leftRotationMatrix[2][2];
	leftRotationMatrix[2][3] = -leftRotationMatrix[2][3];

	MTransformationMatrix righthandRotationMatrix = leftRotationMatrix;
	righthandRotationMatrix.getRotationQuaternion(x, y, z, w);
}


//send a command to the connection.
void serv::sendCmdLineTo(connection& clientCam, string cmdLine)
{
	clientCam.write(cmdLine.c_str(), cmdLine.length());
}


//Parse the bufferCmdLine
void serv::parseCmdLine()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////
/////		class : tcpServerTestCmd
/////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MStatus tcpServerTestCmd::doIt (const MArgList &)
{
	try
	{
		cout<<"starting the tcp server "<<endl;
		tcpServerTestCmd::mayaTcpServer.set_max_connections(MAX_CONNECTION_NUM);
		tcpServerTestCmd::mayaTcpServer.set_listening_port(3000);
		cout<<"I am listening port 3000"<<endl;
		tcpServerTestCmd::mayaTcpServer.start_async ();
		cout << "I am start to async listen" <<endl;

	}
	catch(exception& e)
	{
		cout << e.what();
	}

	return MStatus::kSuccess;
}

/*
bool tcpServerTestCmd::CameraMove(CameraProperties camParam)
{

	MStatus stat = M3dView::active3dView().getCamera(camera);
	if(MS::kSuccess == stat){
		MFnCamera fnCamera(camera);						//get the function set of this camera
		MFnTransform fnTransCamera(camera);				//the function set of this camera's transfom
		MObject camObj1 =  camera.transform();
		MFnTransform camTrans(camObj1);
		cout << "camera path :" << fnCamera.fullPathName() << endl;
		//set this camera's transform
	
		translationVector[0] = camParam.translateX;
		translationVector[1] = camParam.translateY;
		translationVector[2] = camParam.translateZ;
		//MVector transVectorInCentimeters(camParam.translateX*100, camParam.translateY*100, camParam.translateZ*100);
		camTrans.setTranslation(translationVector, MSpace::kWorld);		//send the reference of the first parameter
	
		//set this camera's rotation
		camTrans.setRotation(camParam.eulerRotation, MTransformationMatrix::RotationOrder::kXYZ);
	}
	else{
		cerr << "Error to move the camera" <<endl;
	}
	return true;
}

*/