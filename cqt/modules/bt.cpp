#include <QtDebug>
#include <QVariant>

#include <winsock2.h>
#include <mswsock.h>
#include <ws2bth.h>
#include <BluetoothAPIs.h>
#include <objbase.h>
#include <initguid.h>

#include "bt.h"
#include "bt_pvt.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "irprops.lib")

// 
// Host
//
class Private
{
public:
	Private()
	{
		pWSAQuerySet = NULL;
		ulPQSSize = sizeof(WSAQUERYSET);
		AllocMemory(pWSAQuerySet, ulPQSSize);
		pWSAQuerySet->dwSize = ulPQSSize;
		pWSAQuerySet->dwNameSpace = NS_BTH;
		s = INVALID_SOCKET;
		connectedSock = INVALID_SOCKET;
	}

	~Private()
	{
		if (pWSAQuerySet)
			HeapFree(GetProcessHeap(), 0, pWSAQuerySet);

		::closesocket(s);
		::closesocket(connectedSock);
	}

public:
	PWSAQUERYSET pWSAQuerySet;
	HANDLE hLookup;
	ULONG ulFlags;
	ULONG ulPQSSize;
	GUID protocol;
	SOCKET s, connectedSock;
	WSAPROTOCOL_INFO protocolInfo;
	char* inComingdata;
};

QBtLocator::QBtLocator(): proxy(NULL)
{
}

QBtLocator::~QBtLocator()
{
	delete proxy;
}

void QBtLocator::run()
{
	proxy = new LocatorProxy();
	exec();
}

void QBtLocator::startLookup()
{
	// wait until proxy is defined
	while (!proxy)
	{
		QThread::sleep(2);
	}
	if (!QObject::connect(proxy, SIGNAL(proxyFoundDevice(QString, QByteArray)), 
		this, SLOT(proxyFoundDevice(QString, QByteArray))))
	{
		qDebug() << "Could not connect to proxy for device found notification";
	}
	
	if (!QObject::connect(proxy, SIGNAL(proxyRequestComplete(int)), 
		this, SLOT(proxyRequestComplete(int))))
	{
		qDebug() << "Could not connect to proxy for no more devices notification";
	}

	if (!QMetaObject::invokeMethod(proxy, "startLookup"))
	{
		qDebug() << "Could not Invoke method startLookup on proxy";
	}
}

void QBtLocator::findNext()
{
	if (!QMetaObject::invokeMethod(proxy, "findNext"))
	{
		qDebug() << "Could not Invoke method FindNext on proxy";
	}
}

void QBtLocator::proxyFoundDevice(QString name, QByteArray btAddr)
{
	emit foundDevice(name, btAddr.toLongLong());
}

void QBtLocator::proxyRequestComplete(int error)
{
	emit requestComplete(error);
}

// Proxy
LocatorProxy::LocatorProxy()
{
	WSADATA    WSAData = {0};
    if (::WSAStartup(MAKEWORD(2, 2), &WSAData) != 0)
	{
		qDebug() << "Could not start networking";
		emit proxyRequestComplete(::GetLastError());
	}

	pvt = new Private();
	if (pvt)
	{
		pvt->ulFlags = LUP_RETURN_NAME | LUP_CONTAINERS | LUP_RETURN_ADDR |
			/*LUP_FLUSHCACHE |*/ LUP_RETURN_TYPE | LUP_RETURN_BLOB | LUP_RES_SERVICE;
	}
}

LocatorProxy::~LocatorProxy()
{
	delete pvt;
	WSACleanup();
}

void LocatorProxy::startLookup()
{
	if (pvt->pWSAQuerySet == NULL)
	{
		qDebug() << "Could not allocate memory";
		emit proxyRequestComplete(::GetLastError());
		return;
	}
	int result = WSALookupServiceBegin(pvt->pWSAQuerySet, pvt->ulFlags, &pvt->hLookup);
    if (0 == result)
	{
		findNext();
	}
	else 
	{
		emit proxyRequestComplete(::GetLastError());
		qDebug() << "Could not start host lookup";
	}
}

void LocatorProxy::findNext()
{
	int result = 0;
	while(result == 0)
	{
		if ( NO_ERROR == WSALookupServiceNext(pvt->hLookup, pvt->ulFlags, &pvt->ulPQSSize, pvt->pWSAQuerySet) )
		{
			QString name = QString::fromUtf16((const ushort*)pvt->pWSAQuerySet->lpszServiceInstanceName);
			PCSADDR_INFO addrInfo = (PCSADDR_INFO)pvt->pWSAQuerySet->lpcsaBuffer;
			PSOCKADDR_BTH address = (PSOCKADDR_BTH)addrInfo->RemoteAddr.lpSockaddr;
	
			// This is a qlonglong
			emit proxyFoundDevice(name, QByteArray::number(address->btAddr));
			break;
		}

		if ( WSA_E_NO_MORE == ( result = WSAGetLastError() ) )
		{
			emit proxyRequestComplete(0);;
			WSALookupServiceEnd(pvt->hLookup);
		}
		else if ( WSAEFAULT == result )
		{
			AllocMemory(pvt->pWSAQuerySet, pvt->ulPQSSize);
			result = 0; // to Loop
		}
	}
}

// remote host
//
//
QBtRemotehost::QBtRemotehost()
{
	pvt = new Private;
	if (pvt)
	{
		pvt->protocol = L2CAP_PROTOCOL_UUID;
		pvt->pWSAQuerySet->lpServiceClassId = &pvt->protocol;
		pvt->ulFlags = /*LUP_FLUSHCACHE |*/ LUP_RETURN_NAME | LUP_RETURN_TYPE | 
			LUP_RETURN_ADDR | LUP_RETURN_BLOB | LUP_RETURN_COMMENT;
	}
}

QBtRemotehost::~QBtRemotehost()
{
	delete pvt;
}

void QBtRemotehost::getServices()
{
	if (pvt->pWSAQuerySet == NULL)
	{
		qDebug() << "Could not allocate memory" ;
		emit requestComplete(::GetLastError());
		return;
	}

	int sz = sizeof(pvt->protocolInfo);
	if (0 != ::bt_open_socket(&pvt->s, &pvt->protocolInfo, &sz))
	{
		qDebug() << "Could not start BT socket";
		emit requestComplete(::GetLastError());
		return;
	}

	QString address = ::AddressAsString(remoteAddress, &pvt->s, &pvt->protocolInfo, &sz);
	pvt->pWSAQuerySet->lpszContext = (LPWSTR)(address.utf16());
	int result = WSALookupServiceBegin(pvt->pWSAQuerySet, pvt->ulFlags, &pvt->hLookup);
    if (0 == result)
	{
		// In win32, remembered means that the defice appears in the bluetooth panel
		// It will appear in the database even if it shows up as not remembered
		// Authenticated indicates whether or not the passkey was exchanged.
		// Also, these states only indicate the status of the local device
		if (pvt->pWSAQuerySet->dwOutputFlags & BTHNS_RESULT_DEVICE_CONNECTED) 
			setState(QBtRemotehost::Connected);
		if (pvt->pWSAQuerySet->dwOutputFlags & BTHNS_RESULT_DEVICE_REMEMBERED)
			setState(QBtRemotehost::Remembered);
		if (pvt->pWSAQuerySet->dwOutputFlags & BTHNS_RESULT_DEVICE_AUTHENTICATED)
			setState(QBtRemotehost::Authenticated);
				
		findNext();
	}
	else 
	{
		qDebug() << "Could not start service lookup";
		emit requestComplete(::GetLastError());
	}
}

void QBtRemotehost::findNext()
{
	int result = 0;
	while(0 == result)
	{
		if ( NO_ERROR == WSALookupServiceNext(pvt->hLookup, pvt->ulFlags, &pvt->ulPQSSize, pvt->pWSAQuerySet) )
		{
			QString name = QString::fromUtf16((const ushort*)pvt->pWSAQuerySet->lpszServiceInstanceName);
			QString comment = QString::fromUtf16((const ushort*)pvt->pWSAQuerySet->lpszComment);
			ulong btPort = 0;
			PCSADDR_INFO addrInfo = (PCSADDR_INFO)pvt->pWSAQuerySet->lpcsaBuffer;
			if (addrInfo) 
			{
				PSOCKADDR_BTH address = (PSOCKADDR_BTH)addrInfo->RemoteAddr.lpSockaddr;
				btPort = address->port;
			}
			const BLOB *pBlob = (BLOB*)pvt->pWSAQuerySet->lpBlob;
			QByteArray blob;
			if (pBlob)
            {
//				getAttributes(pBlob);
				blob = QByteArray::fromRawData((const char*)pBlob->pBlobData, pBlob->cbSize);
			}
			emit foundService(name, comment, btPort, blob);
			break;
		}
		if ( WSA_E_NO_MORE == ( result = WSAGetLastError() ) )
		{
			WSALookupServiceEnd(pvt->hLookup);
			emit requestComplete(0);
		}
		if ( WSAEFAULT == result )
		{
			//qDebug() << "=CRITICAL= | WSALookupServiceNext() failed with error code" << result;
			AllocMemory(pvt->pWSAQuerySet, pvt->ulPQSSize);
			result = 0; // to Loop
		}
	}
}

void QBtRemotehost::authenticate()
{
	BLUETOOTH_DEVICE_INFO btdi = { sizeof(btdi) };
	btdi.Address.ullLong = remoteAddress;
	HANDLE hRadio = NULL;

	DWORD result = BluetoothGetDeviceInfo(hRadio, &btdi);
	if (result == 0)
	{
		QString passwd = "1234";
		const ushort* pszPasskey = passwd.utf16();
		result  = BluetoothAuthenticateDevice(NULL, NULL, &btdi, (PWCHAR)pszPasskey, passwd.length());
		result;
	}
}

QString QBtRemotehost::getLastError()
{
	return QString::fromUtf16(GetLastErrorMessage(::GetLastError()));
}

//
// Service
//
#define L2CAP_MAXPACKET 65535

QBtService::QBtService()
{
	pvt = new Private();
	pvt->inComingdata = (char*)::HeapAlloc(::GetProcessHeap(), HEAP_ZERO_MEMORY, L2CAP_MAXPACKET);
	ZeroMemory(pvt->inComingdata, L2CAP_MAXPACKET);
}

QBtService::~QBtService()
{
	if (0 != WSASetService(pvt->pWSAQuerySet, RNRSERVICE_DELETE, 0))
	{
		qDebug() << "Could not close service";
	}
	::HeapFree(GetProcessHeap(), 0, pvt->inComingdata);
	delete pvt;
}

void QBtService::setServiceId(int id) 
{
	serviceId=ServiceDiscoveryServerServiceClassID_UUID; serviceId.data1 = id; 
}

QString QBtService::getLastError()
{
	return QString::fromUtf16(GetLastErrorMessage(::GetLastError()));
}

void QBtService::startReceive()
{
	if (pvt->s  == INVALID_SOCKET)
	{
		advertise();
	}
	// now depending on the situation, either connect or accept.
	if (pvt->connectedSock == INVALID_SOCKET)
	{		
		if (0 != listen(pvt->s, 10))
		{
			qDebug() << "Could not BT listen on socket";
			emit requestComplete(::GetLastError());
			return;
		}

		// Create a handle for the completion port
		HANDLE hCompPort = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, (u_long)0, 0 );
		// Associate the listening socket with the completion port
		CreateIoCompletionPort((HANDLE)pvt->s, hCompPort, (u_long)0, 0);
		// Load the AcceptEx function into memory using WSAIoctl.
		LPFN_ACCEPTEX lpfnAcceptEx = NULL;
		DWORD dwBytes;
		GUID GuidAcceptEx = WSAID_ACCEPTEX;
		WSAIoctl(pvt->s, 
			SIO_GET_EXTENSION_FUNCTION_POINTER, 
			&GuidAcceptEx, 
			sizeof(GuidAcceptEx),
			&lpfnAcceptEx, 
			sizeof(lpfnAcceptEx), 
			&dwBytes, 
			NULL, 
			NULL);

		SOCKET s1 = ::socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
		WSAOVERLAPPED olOverlap;
		// Empty our overlapped structure and accept connections.
		memset(&olOverlap, 0, sizeof(olOverlap));

		lpfnAcceptEx(pvt->s, 
			s1,
			pvt->inComingdata, 
			sizeof(pvt->inComingdata) - ((sizeof(sockaddr_in) + 16) * 2),
			sizeof(sockaddr_in) + 16, 
			sizeof(sockaddr_in) + 16, 
			&dwBytes, 
			&olOverlap);

		//----------------------------------------
		// Associate the accept socket with the completion port
		CreateIoCompletionPort((HANDLE)s1, hCompPort, (u_long)0, 0);

		if (s1 != INVALID_SOCKET)
		{
			pvt->connectedSock = s1;
		}
	}

	int r = recv(pvt->connectedSock, pvt->inComingdata, sizeof(pvt->inComingdata), 0);
	QByteArray packet = QByteArray::fromRawData((const char*)pvt->inComingdata, r);
	emit receivedPacket(packet);
}

void QBtService::sendPacket(QByteArray data)
{
	if (pvt->s  == INVALID_SOCKET)
	{
		advertise();
	}
	// now depending on the situation, either connect or accept.
	if (pvt->connectedSock == INVALID_SOCKET)
	{
		// do a socket open before this 
//		connect(pvt->connectedSock, 0, 0);
	}
	if (pvt->connectedSock != INVALID_SOCKET)
	{
		send(pvt->connectedSock, data.data(), data.length(), 0);
	}
	else 
	{
		emit requestComplete(-1);
	}
}

void QBtService::advertise()
{
	int sz = sizeof(pvt->protocolInfo);
	if (0 != ::bt_open_socket(&pvt->s, &pvt->protocolInfo, &sz))
	{
		qDebug() << "Could not start BT socket";
		emit requestComplete(::GetLastError());
		return;
	}

	SOCKADDR_BTH address;
	address.addressFamily = AF_BTH;
	address.btAddr = 0;
	address.serviceClassId = GUID_NULL;
	address.port = BT_PORT_ANY;

	sockaddr *pAddr = (sockaddr*)&address;
	if (0 != ::bind(pvt->s, pAddr, sizeof(SOCKADDR_BTH)))
	{
		qDebug() << "Could not bind BT socket";
		emit requestComplete(::GetLastError());
		return;
	}

	int size = sizeof(SOCKADDR_BTH);
	if (0 != ::getsockname(pvt->s, pAddr, &size))
	{
		qDebug() << "Could not get BT socket's name";
		emit requestComplete(::GetLastError());
		return;
	}

	CSADDR_INFO csAddr;
	memset(&csAddr, 0, sizeof(csAddr));
	csAddr.LocalAddr.iSockaddrLength = sizeof(SOCKADDR_BTH);
	csAddr.LocalAddr.lpSockaddr = pAddr;
	csAddr.iSocketType = SOCK_STREAM;
	csAddr.iProtocol = BTHPROTO_RFCOMM;

	pvt->pWSAQuerySet->dwNumberOfCsAddrs = 1;
	pvt->pWSAQuerySet->dwNameSpace = NS_BTH;
	pvt->pWSAQuerySet->lpcsaBuffer = &csAddr;

	// These 3 need to be updated for every service
	pvt->pWSAQuerySet->lpszServiceInstanceName = serviceName.length() ? serviceName.utf16() : __TEXT("My Service");
	pvt->pWSAQuerySet->lpszComment =  serviceName.length() ? serviceName.utf16() : __TEXT("My Service");
	pvt->protocol = serviceId.isNull() ? OBEXObjectPushServiceClass_UUID : serviceId.operator GUID();
	pvt->pWSAQuerySet->lpServiceClassId = &pvt->protocol;
	
	if (0 != WSASetService(pvt->pWSAQuerySet, RNRSERVICE_REGISTER, 0))
	{
		qDebug() << "Could not BT listen on socket";
		emit requestComplete(::GetLastError());
		return;
	}
}

// Utils

WCHAR *GetLastErrorMessage(DWORD last_error)
{
   static WCHAR errmsg[512];

   if (!::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 
      0,
      last_error,
      0,
      errmsg, 
      511,
      NULL))
   {
      /* if we fail, call ourself to find out why and return that error */
	   return (::GetLastErrorMessage(GetLastError()));  
   }
  
   return errmsg;
}

void AllocMemory(PWSAQUERYSET& addr, int size)
{
	::HeapFree(GetProcessHeap(), 0, addr);
    addr = NULL;

	addr = (PWSAQUERYSET) ::HeapAlloc(::GetProcessHeap(), HEAP_ZERO_MEMORY, size);
	ZeroMemory(addr, size);
}

int bt_open_socket(SOCKET *s, WSAPROTOCOL_INFO* protocolInfo, int* protocolInfoSize)
{
	*s = ::socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
	if (*s == INVALID_SOCKET)
	{
		printf("Failed to get bluetooth socket! %s\n", GetLastErrorMessage(GetLastError()));
		return 1;
	}
	if (0 != getsockopt(*s, SOL_SOCKET, SO_PROTOCOL_INFO, (char*)protocolInfo, protocolInfoSize))
	{
		printf("Failed to get bluetooth opt! %s\n", GetLastErrorMessage(GetLastError()));
		return 1;
	}
	return 0;
}
QString AddressAsString(qlonglong remoteAddress, SOCKET *s, WSAPROTOCOL_INFO* protocolInfo, int* protocolInfoSize)
{
	QString stringAddress;
	while (1)
	{
		WCHAR addressAsString[100];
		DWORD addressSize = sizeof(addressAsString);
			
		SOCKADDR_BTH address;
		address.addressFamily = AF_BTH;
		address.btAddr = remoteAddress;
		address.serviceClassId = GUID_NULL;
		address.port = BT_PORT_ANY;

		sockaddr *pAddr = (sockaddr*)&address;
		
		if (0 != WSAAddressToString(pAddr, protocolInfo->iMaxSockAddr, protocolInfo, addressAsString, &addressSize))
		{
			qDebug() << "Failed to get bluetooth string address! " << GetLastErrorMessage(::GetLastError());
			break;
		}
		else
		{
			stringAddress = QString::fromUtf16((const ushort*)addressAsString, addressSize);
			break;
		}
	}
	return stringAddress;
}


BOOL __stdcall sdp_callback( ULONG uAttribId, LPBYTE pValueStream, ULONG cbStreamSize, LPVOID pvParam)
{
	SDP_ELEMENT_DATA element;

	if (ERROR_SUCCESS != BluetoothSdpGetElementData(pValueStream,  cbStreamSize, &element))
	{
		qDebug() << GetLastErrorMessage(GetLastError());
	}
	element;
	return true;
}

void getAttributes(const BLOB *pBlob)
{
	if (!BluetoothSdpEnumAttributes(pBlob->pBlobData, pBlob->cbSize, sdp_callback, 0))
				{
					qDebug() << "BluetoothSdpEnumAttributes" << GetLastErrorMessage(GetLastError());
				}
}

