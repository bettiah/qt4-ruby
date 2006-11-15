#ifndef __BT_H__
#define __BT_H__

#include <QThread>
#include <QUuid>

class LocatorProxy;
class Private;

class QBtLocator : public QThread
{
	Q_OBJECT

public:
    QBtLocator();
    ~QBtLocator();

signals:
	void foundDevice(QString name, qlonglong btAddr);
	void requestComplete(int error);

public slots:
	void startLookup();
	void findNext();

private slots:
	void proxyFoundDevice(QString name, QByteArray btAddr);
	void proxyRequestComplete(int error);
	
private:
	void run();
	
private:
	LocatorProxy* proxy;
};


class QBtRemotehost : public QObject
{
	Q_OBJECT
	Q_PROPERTY(qlonglong localAddress READ getLocalAddress WRITE setLocalAddress)
	Q_PROPERTY(qlonglong remoteAddress READ getRemoteAddress WRITE setRemoteAddress)
	Q_PROPERTY(QString localName READ getLocalName WRITE setLocalName)
	Q_PROPERTY(QString remoteName READ getRemoteName WRITE setRemoteName)
	Q_PROPERTY(QString lastError READ getLastError)
	Q_PROPERTY(States state READ getState WRITE setState)
	Q_FLAGS(States)

public:
	QBtRemotehost();
	~QBtRemotehost();
	
	qlonglong getLocalAddress() { return localAddress; }
	qlonglong getRemoteAddress() { return remoteAddress; }
	QString getLocalName() { return localName; }
	QString getRemoteName() { return remoteName; }
	QString getLastError();
	void setLocalAddress(qlonglong aString) { localAddress = aString; }
	void setRemoteAddress(qlonglong aString) { remoteAddress = aString; }
	void setLocalName(QString aString) { localName = aString; }
	void setRemoteName(QString aString) { remoteName = aString; }

	enum State {Connected = 0x01, Remembered = 0x02, Authenticated = 0x04};
	Q_DECLARE_FLAGS(States, State)
	States getState() { return state; }
	void setState(States aState) { state |= aState; }

public slots:
	void getServices();
	void findNext();
	void authenticate();

signals:
	void foundService(QString name, QString comment, ulong port, QByteArray blob);
	void requestComplete(int error);
	
private:
	qlonglong localAddress;
	qlonglong remoteAddress;
	QString localName;
	QString remoteName;
	States state;

private:
	Private* pvt;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QBtRemotehost::States)


class QBtService : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString serviceName READ getServiceName WRITE setServiceName)
	Q_PROPERTY(int serviceId READ getServiceId WRITE setServiceId)
	
public:
	QBtService();
	~QBtService();

	QString getServiceName() { return serviceName; }
	void setServiceName(QString aString) { serviceName = aString; }
	
	int getServiceId() { return serviceId.data1; }
	void setServiceId(int id);
	
	QString getLastError();

public slots:
	void advertise();
	// The following 2 methods co-operate to connect to a remote service, 
	// or receive data from the service

	// If this is called first, we start a listening socket for this service, 
	// and wait for packet to arrive over the passiveconnect socket
	// Otherwise we receive over the activeconnect socket
	void startReceive();
	// If this is called first, we connect to the other side and try to send a packet
	// Otherwise we send over the already accepted socket
	void sendPacket(QByteArray data);

signals:
	void receivedPacket(QByteArray packet);
	void requestComplete(int error);

private:
	QString serviceName;
	QUuid serviceId;

	Private* pvt;
};

#endif //__BT_H__
