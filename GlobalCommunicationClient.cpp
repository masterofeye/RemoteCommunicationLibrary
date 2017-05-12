#include "stdafx.h"
#include "GlobalCommunicationClient.h"
#include <qdatastream.h>
#include <qhostaddress.h>
//TODO Könnte man eleganter gestalten
#include "Types.h"

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
			m_Logger->debug("Client {} hat sich vom Server getrennt.",(int)spdlog::sinks::FilterType::GlobalCommunicationClient, m_Client->peerAddress().toString().toStdString());
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
                m_Logger->error("A GlobalCommunicationClient socket error occoured: {}", (int)spdlog::sinks::FilterType::GlobalCommunicationClient, "ConnectionRefusedError");
				break;
			case QAbstractSocket::AddressInUseError:
                m_Logger->error("A GlobalCommunicationClient socket error occoured: {}", (int)spdlog::sinks::FilterType::GlobalCommunicationClient, "AddressInUseError");
				break;
			case QAbstractSocket::TemporaryError:
                m_Logger->error("A GlobalCommunicationClient socket error occoured: {}", (int)spdlog::sinks::FilterType::GlobalCommunicationClient, "ServerNotFoundError");
				break;
			case QAbstractSocket::SocketAccessError:
                m_Logger->error("A GlobalCommunicationClient socket error occoured: {}", (int)spdlog::sinks::FilterType::GlobalCommunicationClient, "SocketAccessError");
				break;
			case QAbstractSocket::SocketResourceError:
                m_Logger->error("A GlobalCommunicationClient socket error occoured:  {}", (int)spdlog::sinks::FilterType::GlobalCommunicationClient, "SocketResourceError");
				break;
			case QAbstractSocket::SocketTimeoutError:
                m_Logger->error("A GlobalCommunicationClient socket error occoured:  {}", (int)spdlog::sinks::FilterType::GlobalCommunicationClient, "SocketTimeoutError");
				break;
			case QAbstractSocket::DatagramTooLargeError:
                m_Logger->error("A GlobalCommunicationClient socket error occoured:  {}", (int)spdlog::sinks::FilterType::GlobalCommunicationClient, "DatagramTooLargeError");
				break;
			case QAbstractSocket::UnsupportedSocketOperationError:
                m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", (int)spdlog::sinks::FilterType::GlobalCommunicationClient, "UnsupportedSocketOperationError");
				break;
			case QAbstractSocket::UnknownSocketError:
                m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", (int)spdlog::sinks::FilterType::GlobalCommunicationClient, "UnknownSocketError");
				break;
			case QAbstractSocket::OperationError:
                m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", (int)spdlog::sinks::FilterType::GlobalCommunicationClient, "OperationError");
				break; 
			case QAbstractSocket::UnfinishedSocketOperationError:
                m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", (int)spdlog::sinks::FilterType::GlobalCommunicationClient, "UnfinishedSocketOperationError");
				break;
			case QAbstractSocket::NetworkError:
                m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", (int)spdlog::sinks::FilterType::GlobalCommunicationClient, "NetworkError");
				break;
			case QAbstractSocket::HostNotFoundError:
                m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", (int)spdlog::sinks::FilterType::GlobalCommunicationClient, "HostNotFoundError");
				break;
			case QAbstractSocket::RemoteHostClosedError:
                m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", (int)spdlog::sinks::FilterType::GlobalCommunicationClient, "RemoteHostClosedError");
				break;
			case QAbstractSocket::ProxyAuthenticationRequiredError:
                m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", (int)spdlog::sinks::FilterType::GlobalCommunicationClient, "ProxyAuthenticationRequiredError");
				break;
			case QAbstractSocket::SslHandshakeFailedError:
                m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", (int)spdlog::sinks::FilterType::GlobalCommunicationClient, "SslHandshakeFailedError");
				break;
			case QAbstractSocket::ProxyConnectionRefusedError:
                m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", (int)spdlog::sinks::FilterType::GlobalCommunicationClient, "UnfinishedSocketOperationError");
				break;
			case QAbstractSocket::ProxyConnectionClosedError:
                m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", (int)spdlog::sinks::FilterType::GlobalCommunicationClient, "ProxyConnectionClosedError");
				break;
			case QAbstractSocket::ProxyConnectionTimeoutError:
                m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", (int)spdlog::sinks::FilterType::GlobalCommunicationClient, "ProxyConnectionTimeoutError");
				break;
			case QAbstractSocket::ProxyNotFoundError:
                m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", (int)spdlog::sinks::FilterType::GlobalCommunicationClient, "ProxyNotFoundError");
				break;
			case QAbstractSocket::ProxyProtocolError:
                m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", (int)spdlog::sinks::FilterType::GlobalCommunicationClient, "ProxyProtocolError");
				break;
			case QAbstractSocket::SslInternalError:
                m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", (int)spdlog::sinks::FilterType::GlobalCommunicationClient, "SslInternalError");
				break;
			case QAbstractSocket::SslInvalidUserDataError:
                m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", (int)spdlog::sinks::FilterType::GlobalCommunicationClient, "SslInvalidUserDataError");
				break;
			default:
				break;
			}
        }
    }
}