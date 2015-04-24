//
// Copyright (C) 2001 David Gould 
// 
#ifndef CAMERACONTROLCMD_H
#define CAMERACONTROLCMD_H

#include <maya/MPxCommand.h>
#include <maya/MDGModifier.h>
#include <maya/MDagPath.h>
#include <maya/MPoint.h>
#include <maya/MVector.h>

class cameraControlCmd : public MPxCommand 
{
public:
	virtual MStatus	doIt ( const MArgList& );
	virtual MStatus undoIt();
	virtual MStatus redoIt();
	virtual bool isUndoable() const { return true; }

	static void *creator() { return new cameraControlCmd; }
	static MSyntax newSyntax();

	MStatus setCameraPos(MPoint &cameraPos, MVector &cameraDir, MVector &wsUpDir, double horizFieldOfView, double aspectRatio);

	MStatus getCameraParameters();

private:
	MDagPath camera;		//used to store the activity camera node path
	MDGModifier dgMod;


};

#endif