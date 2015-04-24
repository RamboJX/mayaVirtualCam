// NewServer.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include "TCPServer.h"
#include <iostream>
#include <winsock2.h>
#pragma comment(lib, "wsock32.lib")
#include <maya/MGlobal.h>

using namespace std;


Server::Server()
{
	memset(bufRecv, 0, MAX_NUM_BUF);             //��bufRecv�е�ֵ����0��MAX_NUM_BUF�ǻ�����buf�����ߴ�
	memset(bufSend, 0, MAX_NUM_BUF);            

	sServer = INVALID_SOCKET;           //�������������׽���sServer��ֵΪ(~0)
	sClient = INVALID_SOCKET;

	Conning = FALSE;                   //����ӳ�ʾ��Ϊ0

									//����windows sockets DLL�汾
	WORD	wVersionRequested;		//Ӧ�ó�����ҪWindows sockets DLL�İ汾
	WSADATA	wsaData;				//�����洢��WSAStartup�������ú󷵻ص� Windows Sockets ����
	int		retVal;					//����Windows sockets API����ֵ		
	
	//1.��ʼ��Windows Sockets DLL
	wVersionRequested = MAKEWORD(1,1);        //���汾��ֵΪ1.1
	retVal = WSAStartup(wVersionRequested, &wsaData);   //WSAStartup���ó�ʼ���������ɹ��ķ���ֵΪ0
	if ( 0 != retVal ) 
	{
		MGlobal::displayError( MString( "WSAStartup error" ) );
		WSACleanup();
	}	
	//ȷ��WinSock DLL֧��1.1
	if ( LOBYTE( wsaData.wVersion ) != 1 ||	HIBYTE( wsaData.wVersion ) != 1)     //LOBYTE������ȡ��16��������ͣ����ұߣ��Ǹ��ֽڵ�����,HIBYTE������ȡ��16��������ߣ�����ߣ��Ǹ��ֽڵ�����
	{
		MGlobal::displayError( MString( "WSAStartup error" ) );
		WSACleanup();
	}
}



int	Server::CSocket()
{
	sServer= socket(AF_INET, SOCK_STREAM, 0);	   //socket����ַ�壬�������ͣ�Ĭ��ֵ�˴�Ϊ6�������ɹ�����ֵΪ0
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
	//�������׽��ֵ�ַ 
	SOCKADDR_IN addrServ;          //maybeSOCKADDR_IN��һ���࣬�˴�����һ������һ�µ��������Ľṹ��ɷ�
	addrServ.sin_family = AF_INET;                //ͨ������	
	addrServ.sin_port = htons(SERVERPORT);        //ͨ�Ŷ˿� ����Ϊ��5555  htons�����ֽ�˳�򷵻�Ϊ�����ֽ�˳��
	addrServ.sin_addr.s_addr = INADDR_ANY;		  //INTERNET��ַ�����������ַ���ַ0.0.0.0
	//��
	retVal = bind(sServer, (LPSOCKADDR)&addrServ, sizeof(SOCKADDR_IN));  //bind��Ҫ�󶨵��׽ӿڣ��ṹ��ĳɷ֣����ȣ���ֻ���ڷ����
	if(SOCKET_ERROR == retVal)                                       //sizeof��һ�������������Ƿ���һ���������������ռ���ڴ��ֽ���
	{
		closesocket(sServer);						//�ر��׽���
		MGlobal::displayError( MString( "bind socket error" ) );
		WSACleanup();
		return -1;
	}
	return 1;
}

int	Server::CListen()
{
	//��ʼ���� 
	retVal = listen(sServer, 1);                    //listen���������󣻼���������
	if(SOCKET_ERROR == retVal)
	{
		closesocket(sServer);						   //�ر��׽���
		MGlobal::displayError( MString( "listen socket error" ) );
		WSACleanup();
		return -1;
	}
	else
	{	
		//�ȴ��ͻ��˵�����	
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
		closesocket(sServer);							//�ر��׽���
		MGlobal::displayError( MString( "accept socket error" ) );
		WSACleanup();
		return -1;
	}
	else
	{
		Conning = TRUE;								//�ͻ�������ɹ�
		MGlobal::displayInfo( MString( "Client accepted" ) );
	}

	//��ʾ�ͻ��˵�IP�Ͷ˿�
	char *pClientIP = inet_ntoa(addrClient.sin_addr);     //�������ַת���ɡ�.��������ַ�����ʽ
	u_short  clientPort = ntohs(addrClient.sin_port);	 //ntohs �� 16 λ�����������ֽ�˳��ת��Ϊ�����ֽ�˳��
	
	MGlobal::displayInfo( MString( "accept a client: ip:" ) + pClientIP + MString( "port:" ) + clientPort);
	return 1;
}


int	Server::CRecv()
{
	BOOL	retVal = TRUE;			//����ֵ
	BOOL	bLineEnd = FALSE;		//�н���
	int		nReadLen = 0;			//�����ֽ���
	int		nDataLen = 0;			//���ݳ���
	memset(bufRecv, 0, MAX_NUM_BUF);  
	while (!bLineEnd && Conning)	//��ͻ������� û�л���
	{
		nReadLen = recv(sClient, bufRecv + nDataLen, 1, 0);//ÿ�ν���һ���ֽ�

		//������
		if (SOCKET_ERROR == nReadLen)
		{
			int nErrCode = WSAGetLastError();//�������
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
			retVal = FALSE;	//������ʧ��
			break;			//����ѭ��						
		}

		//�����ݽ���
		if (0 == nReadLen)//�ͻ��˹ر�
		{
			retVal = FALSE;	//������ʧ��
			break ;			//����ѭ��			
		}
		//��������
		if ('\n' == *(bufRecv + nDataLen))	//���з�
		{
			bLineEnd = TRUE;			//�������ݽ���
		}
		else
		{
			nDataLen += nReadLen;		//�������ݳ���
		}	
	}

	if (!retVal)
	{  
		MGlobal::displayInfo( MString( "recieve over!" ) );
		//cout<<"bye";
		return	-1;    //�˳�
	}	
	//��ʾ�ͻ�������
	//MGlobal::displayInfo( bufRecv );
	//cout <<endl<< bufRecv<<endl;
	return 1;
}

int	Server::CSend()
{
	//��ͻ��˷�������
	strcpy(bufSend, "Hello,Client!\n");	
	retVal = send(sClient, bufSend, strlen(bufSend), 0);//һ�η���
	if(INVALID_SOCKET==retVal)
	{
		MessageBoxA(NULL,"Failed send!", "Server Error",MB_OK);
		return -1;
	}
	return 1;
}

int	Server::CClose()
{
	//��ʾ�˳���Ϣ
	MGlobal::displayInfo( MString( "Server exiting..." ) );
	return 1;	
}

Server::~Server()
{
	closesocket(sServer);	
	closesocket(sClient);	
	WSACleanup();			
}
