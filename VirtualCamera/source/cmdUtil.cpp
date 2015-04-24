#include "cmdUtil.h"

#include <maya/MPoint.h>
#include <maya/MVector.h>
#include <maya/MFnCamera.h>
#include <maya/M3dView.h>
#include <maya/MDagPath.h>
#include <maya/MTime.h>

#include <maya/MFnDagNode.h>
#include <maya/MItDag.h>
#include <maya/MObject.h>
#include <maya/MDagPath.h>
#include <maya/MStatus.h>

#include <maya/MString.h>
#include <maya/MFnCamera.h>
#include <maya/MPoint.h>
#include <maya/MVector.h>
#include <maya/MMatrix.h>
#include <maya/MTransformationMatrix.h>

#include <maya/MFnLight.h>
#include <maya/MColor.h>
#include <maya/MFnNurbsSurface.h>
#include <maya/MIOStream.h>


bool cmdUtil::cameraSet(const MPoint & wsEyeLocation,
						const MVector & wsViewDirection, 
						const MVector & wsUpDirection,
						double 	horizFieldOfView,
						double 	aspectRatio )
{
	MDagPath camera;

	MStatus stat = M3dView::active3dView().getCamera(camera);

	if( MS::kFailure == stat){
		cerr<< "error to getting the camera"<<endl;
		return false;
	}


	MPoint cameraPos(0.0, 0.0, 0.0, 1.0);
	MVector cameraDir(0.0, 1.0, 0.0);
	MVector wsUpDir(0.0, 1.0, 0.0);
	//horizFieldOfView = 100;

	MFnCamera fnCamera(camera);
	double aspectRaio = fnCamera.aspectRatio();

	fnCamera.set(cameraPos, cameraDir, wsUpDir, horizFieldOfView, aspectRaio);


	return true;
}


MStatus cmdUtil::scanDag(const MItDag::TraversalType traversalType, MFn::Type filter, bool quiet)
//MStatus scanDag::doScan( const MItDag::TraversalType traversalType,
//	MFn::Type filter,
//	bool quiet)
{   
	MStatus status;

	MItDag dagIterator( traversalType, filter, &status);

	if ( !status) {
		status.perror("MItDag constructor");
		return status;
	}

	//	Scan the entire DAG and output the name and depth of each node

	if (traversalType == MItDag::kBreadthFirst)
		if (!quiet)
			cout << endl << "Starting Breadth First scan of the Dag";
		else
			if (!quiet)
				cout << endl << "Starting Depth First scan of the Dag";

	switch (filter) 
	{
		case MFn::kCamera:
			if (!quiet)
				cout << ": Filtering for Cameras\n";
			break;
		case MFn::kLight:
			if (!quiet)
				cout << ": Filtering for Lights\n";
			break;
		case MFn::kNurbsSurface:
			if (!quiet)
				cout << ": Filtering for Nurbs Surfaces\n";
			break;
		default:
			cout << endl;
	}

	int objectCount = 0;
	for ( ; !dagIterator.isDone(); dagIterator.next() ) {

		MDagPath dagPath;

		status = dagIterator.getPath(dagPath);
		if ( !status ) {
			status.perror("MItDag::getPath");
			continue;
		}

		MFnDagNode dagNode(dagPath, &status);
		if ( !status ) {
			status.perror("MFnDagNode constructor");
			continue;
		}

		if (!quiet)
			cout << dagNode.name() << ": " << dagNode.typeName() << endl;

		if (!quiet)
			cout << "  dagPath: " << dagPath.fullPathName() << endl;

		objectCount += 1;
		if (dagPath.hasFn(MFn::kCamera)) {  
			MFnCamera camera (dagPath, &status);
			// put the scaned camera to the dag path array
			cameraDagArray.append(dagPath);		
			if ( !status ) {
				status.perror("MFnCamera constructor");
				continue;
			}

			// Get the translation/rotation/scale data
			printTransformData(dagPath, quiet);

			// Extract some interesting Camera data
			if (!quiet)
			{
				cout << "  eyePoint: "
					<< camera.eyePoint(MSpace::kWorld) << endl;
				cout << "  upDirection: "
					<< camera.upDirection(MSpace::kWorld) << endl;
				cout << "  viewDirection: "
					<< camera.viewDirection(MSpace::kWorld) << endl;
				cout << "  aspectRatio: " << camera.aspectRatio() << endl;
				cout << "  horizontalFilmAperture: "
					<< camera.horizontalFilmAperture() << endl;
				cout << "  verticalFilmAperture: "
					<< camera.verticalFilmAperture() << endl;
			}
		} else if (dagPath.hasFn(MFn::kLight)) {
			MFnLight light (dagPath, &status);
			if ( !status ) {
				status.perror("MFnLight constructor");
				continue;
			}

			// Get the translation/rotation/scale data
			printTransformData(dagPath, quiet);

			// Extract some interesting Light data
			MColor color;

			color = light.color();
			if (!quiet)
			{
				cout << "  color: ["
					<< color.r << ", "
					<< color.g << ", "
					<< color.b << "]\n";
			}
			color = light.shadowColor();
			if (!quiet)
			{
				cout << "  shadowColor: ["
					<< color.r << ", "
					<< color.g << ", "
					<< color.b << "]\n";

				cout << "  intensity: " << light.intensity() << endl;
			}
		} else if (dagPath.hasFn(MFn::kNurbsSurface)) {
			MFnNurbsSurface surface (dagPath, &status);
			if ( !status ) {
				status.perror("MFnNurbsSurface constructor");
				continue;
			}

			// Get the translation/rotation/scale data
			printTransformData(dagPath, quiet);

			// Extract some interesting Surface data
			if (!quiet)
			{
				cout << "  numCVs: "
					<< surface.numCVsInU()
					<< " * "
					<< surface.numCVsInV()
					<< endl;
				cout << "  numKnots: "
					<< surface.numKnotsInU()
					<< " * "
					<< surface.numKnotsInV()
					<< endl;
				cout << "  numSpans: "
					<< surface.numSpansInU()
					<< " * "
					<< surface.numSpansInV()
					<< endl;
			}
		} else {
			// Get the translation/rotation/scale data
			printTransformData(dagPath, quiet);
		}
	}

	if (!quiet)
	{
		cout.flush();
	}
	//setResult(objectCount);
	return MS::kSuccess;
}


void cmdUtil::printTransformData(const MDagPath& dagPath, bool quiet)
{
	MStatus		status;
	MObject		transformNode = dagPath.transform(&status);
	// This node has no transform - i.e., it's the world node
	if (!status && status.statusCode () == MStatus::kInvalidParameter)
		return;
	MFnDagNode	transform (transformNode, &status);
	if (!status) {
		status.perror("MFnDagNode constructor");
		return;
	}
	MTransformationMatrix	matrix (transform.transformationMatrix());

	if (!quiet)
	{
		cout << "  translation: " << matrix.translation(MSpace::kWorld)
			<< endl;
	}
	double									threeDoubles[3];
	MTransformationMatrix::RotationOrder	rOrder;

	matrix.getRotation (threeDoubles, rOrder, MSpace::kWorld);
	if (!quiet)
	{
		cout << "  rotation: ["
			<< threeDoubles[0] << ", "
			<< threeDoubles[1] << ", "
			<< threeDoubles[2] << "]\n";
	}
	matrix.getScale (threeDoubles, MSpace::kWorld);
	if (!quiet)
	{
		cout << "  scale: ["
			<< threeDoubles[0] << ", "
			<< threeDoubles[1] << ", "
			<< threeDoubles[2] << "]\n";
	}
}