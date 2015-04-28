/*
|| create by rambo
*/

#include "tcpServerTestCmd.h"

serv tcpServerTestCmd::mayaTcpServer;

///class serv
//this function is a new thread to listen to a client and read command or sent command to the client
//when this function end the connection will gracefully closed.
void serv::on_connect(connection& new_connection)
{
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
		//find out if is a line end
		if('\n' == buffer)
		{
			//Parse this command line
			parseCmdLine();
			//refresh the bufferCmdLine, and iDataLen.
			memset(bufferCmdLine, 0, MAX_COMMAND_LINE);
			iDataLen = 0;
		}

    }

	
}

//send a command to the connection.
void serv::sendCmdLineTo(connection& clientCam, string cmdLine)
{
	clientCam.write(cmdLine.c_str(), cmdLine.length());
}


//Parse the bufferCmdLine
void serv::parseCmdLine()
{
	MString cmdLine (bufferCmdLine);
	MStringArray cmdWords;

	cout << cmdLine << endl;
	cmdLine.split(';', cmdWords);

	if(cmdWords[0] == "camera")
	{
		//set camera keyframe;
		CameraProperties camParam;
		camParam.frame = cmdWords[1].asInt();
		camParam.tranlateX = cmdWords[2].asDouble();
		camParam.translateY = cmdWords[3].asDouble();
		camParam.translateZ = cmdWords[4].asDouble();
		camParam.rotation[0] = cmdWords[5].asDouble();
		camParam.rotation[1] = cmdWords[6].asDouble();
		camParam.rotation[2] = cmdWords[7].asDouble();
		camParam.fov = cmdWords[8].asDouble();
		cameraController.CameraMove(camParam);

	}
	else
	{
		;
	}
}


MStatus tcpServerTestCmd::doIt (const MArgList &)
{
	cout<<"i am here, to start the tcp "<<endl;

	try
	{
		tcpServerTestCmd::mayaTcpServer.set_max_connections(MAX_CONNECTION_NUM);
		tcpServerTestCmd::mayaTcpServer.set_listening_port(3000);
		cout<<"i am alreay set the listening port"<<endl;
		tcpServerTestCmd::mayaTcpServer.start_async ();
		cout << "i am start to async to the listen" <<endl;
	}
	catch(exception& e)
	{
		cout << e.what();
	}

	return MStatus::kSuccess;
}