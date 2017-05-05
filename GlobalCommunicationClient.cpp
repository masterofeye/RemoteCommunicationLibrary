#include "stdafx.h"
#include "GlobalCommunicationClient.h"
#include <qdatastream.h>
#include <qhostaddress.h>

namespace RW{
    namespace COM{
        GlobalCommunicationClient::GlobalCommunicationClient(quint16 Port, std::shared_ptr<spdlog::logger> Logger, QObject* Parent) : BasicCommunicationServer(Parent),
            m_Logger(Logger),
            m_Port(Port),
            m_Client(new QTcpSocket(this))
        {
        }

        bool GlobalCommunicationClient::Listen()
        {
			connect(m_Client, &QTcpSocket::connected, this, &GlobalCommunicationClient::OnPrepareIncomingConnection);
			connect(m_Client, &QTcpSocket::disconnected, this, &GlobalCommunicationClient::OnDisconnected);

			connect(m_Client, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(OnSocketError(QAbstractSocket::SocketError)));
			m_Client->connectToHost(QHostAddress::LocalHost, m_Port);

            return true;
        }

        GlobalCommunicationClient::~GlobalCommunicationClient()
        {
        }

        void GlobalCommunicationClient::OnDisconnected()
        {
			m_Logger->debug("Client {} hat sich vom Server getrennt.", m_Client->peerAddress().toString().toStdString());
			m_Client->deleteLater();
        }

        void GlobalCommunicationClient::OnPrepareIncomingConnection()
        {

        }

        void GlobalCommunicationClient::OnProcessMessage(Message Msg)
        {

        }

        void GlobalCommunicationClient::OnExternalMessage()
        {
			if (m_Client->bytesAvailable() <= 0)
				return;

			Message msg;
			QDataStream sizeStream(m_Client->readAll());
			sizeStream >> msg;

			emit NewMessage(msg);
        }

        void GlobalCommunicationClient::OnExternalMessage(Message Msg)
        {
        }

        void GlobalCommunicationClient::OnSocketError(QAbstractSocket::SocketError socketError)
        {
			switch (socketError)
			{
			case QAbstractSocket::ConnectionRefusedError:
				m_Logger->error("A GlobalCommunicationClient socket error occoured: {}", "ConnectionRefusedError");
				break;
			case QAbstractSocket::AddressInUseError:
				m_Logger->error("A GlobalCommunicationClient socket error occoured: {}", "AddressInUseError");
				break;
			case QAbstractSocket::TemporaryError:
				m_Logger->error("A GlobalCommunicationClient socket error occoured: {}", "ServerNotFoundError");
				break;
			case QAbstractSocket::SocketAccessError:
				m_Logger->error("A GlobalCommunicationClient socket error occoured: {}", "SocketAccessError");
				break;
			case QAbstractSocket::SocketResourceError:
				m_Logger->error("A GlobalCommunicationClient socket error occoured:  {}", "SocketResourceError");
				break;
			case QAbstractSocket::SocketTimeoutError:
				m_Logger->error("A GlobalCommunicationClient socket error occoured:  {}", "SocketTimeoutError");
				break;
			case QAbstractSocket::DatagramTooLargeError:
				m_Logger->error("A GlobalCommunicationClient socket error occoured:  {}", "DatagramTooLargeError");
				break;
			case QAbstractSocket::UnsupportedSocketOperationError:
				m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", "UnsupportedSocketOperationError");
				break;
			case QAbstractSocket::UnknownSocketError:
				m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", "UnknownSocketError");
				break;
			case QAbstractSocket::OperationError:
				m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", "OperationError");
				break; 
			case QAbstractSocket::UnfinishedSocketOperationError:
				m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", "UnfinishedSocketOperationError");
				break;
			case QAbstractSocket::NetworkError:
				m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", "NetworkError");
				break;
			case QAbstractSocket::HostNotFoundError:
				m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", "HostNotFoundError");
				break;
			case QAbstractSocket::RemoteHostClosedError:
				m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", "RemoteHostClosedError");
				break;
			case QAbstractSocket::ProxyAuthenticationRequiredError:
				m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", "ProxyAuthenticationRequiredError");
				break;
			case QAbstractSocket::SslHandshakeFailedError:
				m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", "SslHandshakeFailedError");
				break;
			case QAbstractSocket::ProxyConnectionRefusedError:
				m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", "UnfinishedSocketOperationError");
				break;
			case QAbstractSocket::ProxyConnectionClosedError:
				m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", "ProxyConnectionClosedError");
				break;
			case QAbstractSocket::ProxyConnectionTimeoutError:
				m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", "ProxyConnectionTimeoutError");
				break;
			case QAbstractSocket::ProxyNotFoundError:
				m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", "ProxyNotFoundError");
				break;
			case QAbstractSocket::ProxyProtocolError:
				m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", "ProxyProtocolError");
				break;
			case QAbstractSocket::SslInternalError:
				m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", "SslInternalError");
				break;
			case QAbstractSocket::SslInvalidUserDataError:
				m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", "SslInvalidUserDataError");
				break;
			default:
				break;
			}
        }
    }
}