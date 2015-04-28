//
// Copyright (C) 2001 David Gould 
// 
#ifndef TCPSERVERTESTCMD_H
#define TCPSERVERTESTCMD_H

#include <iostream>
#include <maya/MPxCommand.h>
#include <maya/MDagPath.h>
#include <maya/MString.h>
#include <maya/MStringArray.h>
#include "cameraControlCmd.h"


#include "dlib-18.14/dlib/sockets.h"
#include "dlib-18.14/dlib/server.h"

using namespace dlib;
using namespace std;

//to define the max connection num to 1, because I just want to connect to one client
//Store the connection to class serv
#define MAX_CONNECTION_NUM 1
#define MAX_COMMAND_LINE 255		//define a command line length


class serv : public server
{
public:
	void on_connect (connection& new_connection);
	void sendCmdLineTo(connection& clientCam, string cmdLine);
	void parseCmdLine();
	//store the connection
	connection * clientCams;
private:
	char bufferCmdLine[MAX_COMMAND_LINE];	
	CameraControl cameraController;
};


class tcpServerTestCmd : public MPxCommand 
{
public:
	virtual MStatus	doIt ( const MArgList& );
	static void *creator() { return new tcpServerTestCmd; }

	static MSyntax newSyntax();

private:
	static serv mayaTcpServer;
};

#endif