#ifndef CMDUTIL_H
#define CMDUTIL_H

#include <maya/MPoint.h>
#include <maya/MVector.h>
#include <maya/MItDag.h>
#include <maya/MDagPathArray.h>

class cmdUtil{
public:
	bool cameraSet(	const MPoint & 	wsEyeLocation,
		const MVector & wsViewDirection, 
		const MVector & wsUpDirection,
		double 	horizFieldOfView,
		double 	aspectRatio );

	MStatus scanDag(const MItDag::TraversalType traversalType, MFn::Type filter, bool quiet);
	void printTransformData(const MDagPath& dagPath, bool quiet);

	MDagPathArray cameraDagArray;

};

#endif
