
#include "cameraControlCmd.h"


MStatus cameraControlCmd::doIt( const MArgList& args)
//
// Description
//     Gets the zoomCamera for the current 3d view and calls
//     the redoIt command to set the focal length.
//
// Note
//     The doit method should collect whatever information is
//     required to do the task, and store it in local class data.
//     It should finally call redoIt to make the command happen.
//
{
	CamParams params;
	MStatus status;
	 for ( int i = 0; i < args.length(); i++ )
	 {
        if ( MString( "-tx" ) == args.asString( i, &status ) && MS::kSuccess == status )
        {
            double tmp = args.asDouble( ++i, &status );
            if ( MS::kSuccess == status )
                params.tranlateX = tmp;
        }
        else if ( MString( "-ty" ) == args.asString( i, &status ) && MS::kSuccess == status )
        {
            double tmp = args.asDouble( ++i, &status );
            if ( MS::kSuccess == status )
				params.translateY = tmp;
        }
		else if ( MString( "-tz" ) == args.asString( i, &status ) && MS::kSuccess == status )
        {
            double tmp = args.asDouble( ++i, &status );
            if ( MS::kSuccess == status )
				params.translateY = tmp;
        }
		else if ( MString( "-rx" ) == args.asString( i, &status ) && MS::kSuccess == status )
        {
            double tmp = args.asDouble( ++i, &status );
            if ( MS::kSuccess == status )
				params.rotation[0] = tmp;
        }
				else if ( MString( "-ry" ) == args.asString( i, &status ) && MS::kSuccess == status )
        {
            double tmp = args.asDouble( ++i, &status );
            if ( MS::kSuccess == status )
				params.rotation[1] = tmp;
        }
		else if ( MString( "-rz" ) == args.asString( i, &status ) && MS::kSuccess == status )
        {
            double tmp = args.asDouble( ++i, &status );
            if ( MS::kSuccess == status )
				params.rotation[2] = tmp;
        }
		else if ( MString( "-kf" ) == args.asString( i, &status ) && MS::kSuccess == status )
        {
            double tmp = args.asInt( ++i, &status );
            if ( MS::kSuccess == status )
				params.frame = tmp;
        }
		else if ( MString( "-fov" ) == args.asString( i, &status ) && MS::kSuccess == status )
        {
            double tmp = args.asInt( ++i, &status );
            if ( MS::kSuccess == status )
				params.fov = tmp;
        }
        else
        {
            MString msg = "Invalid flag: ";
            msg += args.asString( i );
            displayError( msg );
            return MS::kFailure;
        }
	 }

	// Get the current zoomCamera
	//
	MStatus stat = M3dView::active3dView().getCamera( camera );	
	
	//TODO: set camera KeyFrame or do other things
	return stat;
}
