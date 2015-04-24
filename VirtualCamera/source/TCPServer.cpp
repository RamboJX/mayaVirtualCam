// NewServer.cpp : 定义控制台应用程序的入口点。
//
#include "TCPServer.h"
#include <iostream>
#include <winsock2.h>
#pragma comment(lib, "wsock32.lib")
#include <maya/MGlobal.h>

using namespace std;


Server::Server()
{
	memset(bufRecv, 0, MAX_NUM_BUF);             //将bufRecv中的值付成0，MAX_NUM_BUF是缓冲区buf的最大尺寸
	memset(bufSend, 0, MAX_NUM_BUF);            

	sServer = INVALID_SOCKET;           //将服务器监听套接字sServer赋值为(~0)
	sClient = INVALID_SOCKET;

	Conning = FALSE;                   //将间接出示化为0

									//调用windows sockets DLL版本
	WORD	wVersionRequested;		//应用程序需要Windows sockets DLL的版本
	WSADATA	wsaData;				//用来存储被WSAStartup函数调用后返回的 Windows Sockets 数据
	int		retVal;					//调用Windows sockets API返回值		
	
	//1.初始化Windows Sockets DLL
	wVersionRequested = MAKEWORD(1,1);        //将版本赋值为1.1
	retVal = WSAStartup(wVersionRequested, &wsaData);   //WSAStartup调用初始化函数，成功的返回值为0
	if ( 0 != retVal ) 
	{
		MGlobal::displayError( MString( "WSAStartup error" ) );
		WSACleanup();
	}	
	//确保WinSock DLL支持1.1
	if ( LOBYTE( wsaData.wVersion ) != 1 ||	HIBYTE( wsaData.wVersion ) != 1)     //LOBYTE（）是取得16进制数最低（最右边）那个字节的内容,HIBYTE（）是取得16进制数最高（最左边）那个字节的内容
	{
		MGlobal::displayError( MString( "WSAStartup error" ) );
		WSACleanup();
	}
}



int	Server::CSocket()
{
	sServer= socket(AF_INET, SOCK_STREAM, 0);	   //socket（地址族，连接类型，默认值此处为6）创建成功返回值为0
	if(INVALID_SOCKET == sServer)
	{
		MGlobal::displayError( MString( "create socket error" ) );
		WSACleanup();
		return -1;
	}
	return 1;
}	

int	Server::CBind()
{
	//服务器套接字地址 
	SOCKADDR_IN addrServ;          //maybeSOCKADDR_IN是一个类，此处定义一个对象，一下的这个对象的结构体成分
	addrServ.sin_family = AF_INET;                //通信类型	
	addrServ.sin_port = htons(SERVERPORT);        //通信端口 定义为了5555  htons主机字节顺序返回为网络字节顺序
	addrServ.sin_addr.s_addr = INADDR_ANY;		  //INTERNET地址，定义任意地址或地址0.0.0.0
	//绑定
	retVal = bind(sServer, (LPSOCKADDR)&addrServ, sizeof(SOCKADDR_IN));  //bind（要绑定的套接口；结构体的成分；长度），只用于服务端
	if(SOCKET_ERROR == retVal)                                       //sizeof是一个操作符，就是返回一个对象或者类型所占的内存字节数
	{
		closesocket(sServer);						//关闭套接字
		MGlobal::displayError( MString( "bind socket error" ) );
		WSACleanup();
		return -1;
	}
	return 1;
}

int	Server::CListen()
{
	//开始监听 
	retVal = listen(sServer, 1);                    //listen（监听对象；监听个数）
	if(SOCKET_ERROR == retVal)
	{
		closesocket(sServer);						   //关闭套接字
		MGlobal::displayError( MString( "listen socket error" ) );
		WSACleanup();
		return -1;
	}
	else
	{	
		//等待客户端的连接	
		MGlobal::displayInfo( MString( "Server succeeded!" ) );
	}
	return 1;
}

int	Server::CAccept()
{
	sockaddr_in addrClient;
	int addrClientlen = sizeof(addrClient);
	sClient = accept(sServer,(sockaddr FAR*)&addrClient, &addrClientlen);
	if(INVALID_SOCKET == sClient)
	{
		closesocket(sServer);							//关闭套接字
		MGlobal::displayError( MString( "accept socket error" ) );
		WSACleanup();
		return -1;
	}
	else
	{
		Conning = TRUE;								//客户端请求成功
		MGlobal::displayInfo( MString( "Client accepted" ) );
	}

	//显示客户端的IP和端口
	char *pClientIP = inet_ntoa(addrClient.sin_addr);     //将网络地址转换成“.”点隔的字符串格式
	u_short  clientPort = ntohs(addrClient.sin_port);	 //ntohs 将 16 位数量从网络字节顺序转换为主机字节顺序
	
	MGlobal::displayInfo( MString( "accept a client: ip:" ) + pClientIP + MString( "port:" ) + clientPort);
	return 1;
}


int	Server::CRecv()
{
	BOOL	retVal = TRUE;			//返回值
	BOOL	bLineEnd = FALSE;		//行结束
	int		nReadLen = 0;			//读入字节数
	int		nDataLen = 0;			//数据长度
	memset(bufRecv, 0, MAX_NUM_BUF);  
	while (!bLineEnd && Conning)	//与客户端连接 没有换行
	{
		nReadLen = recv(sClient, bufRecv + nDataLen, 1, 0);//每次接收一个字节

		//错误处理
		if (SOCKET_ERROR == nReadLen)
		{
			int nErrCode = WSAGetLastError();//错误代码
			if (WSAENOTCONN == nErrCode)
			{
				MGlobal::displayInfo( MString( "The socket is not connected!" ) );
				//MessageBoxA(NULL,"The socket is not connected!","Server Error", MB_OK);
			}
			else if(WSAESHUTDOWN == nErrCode)
			{
				MGlobal::displayInfo( MString( "The socket has been shut down!" ) );
				//MessageBoxA(NULL,"The socket has been shut down!","Server Error", MB_OK);

			}else if (WSAETIMEDOUT == nErrCode)
			{
				MGlobal::displayInfo( MString( "The connection has been dropped!" ) );
				//MessageBoxA(NULL,"The connection has been dropped!", "Server Error",MB_OK);
			}else if (WSAECONNRESET == nErrCode)
			{
				MGlobal::displayInfo( MString( "The virtual circuit was reset by the remote side!" ) );
				//MessageBoxA(NULL,"The virtual circuit was reset by the remote side!","Server Error", MB_OK);
			}else{}	
			retVal = FALSE;	//读数据失败
			break;			//跳出循环						
		}

		//无数据接受
		if (0 == nReadLen)//客户端关闭
		{
			retVal = FALSE;	//读数据失败
			break ;			//跳出循环			
		}
		//读入数据
		if ('\n' == *(bufRecv + nDataLen))	//换行符
		{
			bLineEnd = TRUE;			//接收数据结束
		}
		else
		{
			nDataLen += nReadLen;		//增加数据长度
		}	
	}

	if (!retVal)
	{  
		MGlobal::displayInfo( MString( "recieve over!" ) );
		//cout<<"bye";
		return	-1;    //退出
	}	
	//显示客户端数据
	//MGlobal::displayInfo( bufRecv );
	//cout <<endl<< bufRecv<<endl;
	return 1;
}

int	Server::CSend()
{
	//向客户端发送数据
	strcpy(bufSend, "Hello,Client!\n");	
	retVal = send(sClient, bufSend, strlen(bufSend), 0);//一次发送
	if(INVALID_SOCKET==retVal)
	{
		MessageBoxA(NULL,"Failed send!", "Server Error",MB_OK);
		return -1;
	}
	return 1;
}

int	Server::CClose()
{
	//显示退出信息
	MGlobal::displayInfo( MString( "Server exiting..." ) );
	return 1;	
}

Server::~Server()
{
	closesocket(sServer);	
	closesocket(sClient);	
	WSACleanup();			
}
