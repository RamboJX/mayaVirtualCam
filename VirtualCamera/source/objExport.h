
#ifndef _OBJEXPORT_H
#define _OBJEXPORT_H



#include <string.h> 
#include <sys/types.h>
#include <maya/MStatus.h>
#include <maya/MPxCommand.h>
#include <maya/MString.h>
#include <maya/MStringArray.h>
#include <maya/MArgList.h>
#include <maya/MGlobal.h>
#include <maya/MSelectionList.h>
#include <maya/MItSelectionList.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MDagPath.h>
#include <maya/MDagPathArray.h>

#include <maya/MFnMesh.h>
#include <maya/MFnSet.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MItMeshVertex.h>
#include <maya/MItMeshEdge.h>
#include <maya/MFloatVector.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MFloatArray.h>
#include <maya/MObjectArray.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MPxFileTranslator.h>
#include <maya/MFnDagNode.h>
#include <maya/MItDag.h>
#include <maya/MDistance.h>
#include <maya/MIntArray.h>
#include <maya/MIOStream.h>





#if defined (_WIN32)
#define strcasecmp _stricmp
#elif defined  (OSMac_)
extern "C" int strcasecmp (const char *, const char *);
extern "C" Boolean createMacFile (const char *fileName, FSRef *fsRef, long creator, long type);
#endif

#define NO_SMOOTHING_GROUP      -1
#define INITIALIZE_SMOOTHING    -2
#define INVALID_ID              -1

//
// Edge info structure
//
typedef struct EdgeInfo {
	int                 polyIds[2]; // Id's of polygons that reference edge
	int                 vertId;     // The second vertex of this edge
	struct EdgeInfo *   next;       // Pointer to next edge
	bool                smooth;     // Is this edge smooth
} * EdgeInfoPtr;




//////////////////////////////////////////////////////////////

const char *const objOptionScript = "objExportOptions";
const char *const objDefaultOptions =
	"groups=1;"
	"ptgroups=1;"
	"materials=1;"
	"smoothing=1;"
	"normals=1;"
	;

//////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////
class ObjTranslator : public MPxFileTranslator {
public:
	ObjTranslator () {};
	virtual         ~ObjTranslator () {};
	static void*    creator();

	MStatus         reader ( const MFileObject& file,
		const MString& optionsString,
		FileAccessMode mode);

	MStatus         writer ( const MFileObject& file,
		const MString& optionsString,
		FileAccessMode mode );
	bool            haveReadMethod () const;
	bool            haveWriteMethod () const;
	MString         defaultExtension () const;
	MFileKind       identifyFile ( const MFileObject& fileName,
		const char* buffer,
		short size) const;
private:
	void            outputSetsAndGroups    ( MDagPath&, int, bool, int );
	MStatus         OutputPolygons( MDagPath&, MObject& );
	MStatus         exportSelected();
	MStatus         exportAll();
	void		    initializeSetsAndLookupTables( bool exportAll );
	void		    freeLookupTables();
	bool 		    lookup( MDagPath&, int, int, bool );
	void            setToLongUnitName( const MDistance::Unit&, MString& );
	void			recFindTransformDAGNodes( MString&, MIntArray& );


	// Edge lookup methods
	//
	void            buildEdgeTable( MDagPath& );
	void            addEdgeInfo( int, int, bool );
	EdgeInfoPtr     findEdgeInfo( int, int );
	void            destroyEdgeTable();
	bool            smoothingAlgorithm( int, MFnMesh& );

private:
	// counters
	int v,vt,vn;
	// offsets
	int voff,vtoff,vnoff;
	// options
	bool groups, ptgroups, materials, smoothing, normals;

	FILE *fp;

	// Keeps track of all sets.
	//
	int numSets;
	MObjectArray *sets;

	// Keeps track of all objects and components.
	// The Tables are used to mark which sets each 
	// component belongs to.
	//
	MStringArray *objectNames;

	bool **polygonTablePtr;
	bool **vertexTablePtr;
	bool * polygonTable;
	bool * vertexTable;
	bool **objectGroupsTablePtr;


	// Used to determine if the last set(s) written out are the same
	// as the current sets to be written. We don't need to write out
	// sets unless they change between components. Same goes for
	// materials.
	//
	MIntArray *lastSets;
	MIntArray *lastMaterials;

	// We have to do 2 dag iterations so keep track of the
	// objects found in the first iteration by this index.
	//
	int objectId;
	int objectCount;

	// Edge lookup table (by vertex id) and smoothing group info
	//
	EdgeInfoPtr *   edgeTable;
	int *           polySmoothingGroups;
	int             edgeTableSize;
	int             nextSmoothingGroup;
	int             currSmoothingGroup;
	bool            newSmoothingGroup;

	// List of names of the mesh shapes that we export from maya
	MStringArray	objectNodeNamesArray;

	// Used to keep track of Maya groups (transform DAG nodes) that
	// contain objects being exported
	MStringArray	transformNodeNameArray;
};

#endif