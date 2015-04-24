#include <WinSock2.h>
#pragma comment(lib, "wsock32.lib")

#define	  SERVERPORT			3000   //服务器TCP端口,与连接后自动分配的端口不是同一个
#define   MAX_NUM_BUF			1024   	//缓冲区(buf)最大尺寸

class Server
{
public:
	Server();
	~Server();

public:
	int CSocket();
	int CBind();
	int CListen();
	int CAccept();
	int CRecv();
	int CSend();
	int CClose();


public:
	int       retVal ;
	char	  bufRecv[MAX_NUM_BUF];			//read buffer
	char	  bufSend[MAX_NUM_BUF];			//send buffer
	SOCKET	  sServer;						//服务器监听套接字
	SOCKET	  sClient;						//接受客户端套接字
	BOOL	  Conning;						//与客户端的连接状态
};
