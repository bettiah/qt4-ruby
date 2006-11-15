#include <QObject>
#include <winsock2.h>

class Private;
class LocatorProxy : public QObject
{
	Q_OBJECT

friend class QBtLocator;
public:
	LocatorProxy();
	~LocatorProxy();

public slots:
	void startLookup();
	void findNext();

signals:
	void proxyFoundDevice(QString name, QByteArray btAddr);
	void proxyRequestComplete(int error);

private:
	Private* pvt;
};

WCHAR *GetLastErrorMessage(DWORD last_error);
void AllocMemory(PWSAQUERYSET& addr, int size);
QString AddressAsString(qlonglong remoteAddress, SOCKET *s, WSAPROTOCOL_INFO* protocolInfo, int* protocolInfoSize);
int bt_open_socket(SOCKET *s, WSAPROTOCOL_INFO* protocolInfo, int* protocolInfoSize);
void getAttributes(const BLOB *pBlob);
