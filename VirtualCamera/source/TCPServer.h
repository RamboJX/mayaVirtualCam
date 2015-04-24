#include <WinSock2.h>
#pragma comment(lib, "wsock32.lib")

#define	  SERVERPORT			3000   //������TCP�˿�,�����Ӻ��Զ�����Ķ˿ڲ���ͬһ��
#define   MAX_NUM_BUF			1024   	//������(buf)���ߴ�

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
	SOCKET	  sServer;						//�����������׽���
	SOCKET	  sClient;						//���ܿͻ����׽���
	BOOL	  Conning;						//��ͻ��˵�����״̬
};
