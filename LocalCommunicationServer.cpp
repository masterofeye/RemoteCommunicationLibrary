#include "LocalCommunicationServer.h"
#include <QLocalSocket>
#include <QDataStream>
#include <qmetaobject.h>

namespace RW{
	namespace COM{
		LocalCommunicationServer::LocalCommunicationServer(QString ServerName, std::shared_ptr<spdlog::logger> Logger, QObject* Parent) : BasicCommunicationServer(Parent),
			m_ServerName(ServerName),
			m_Logger(Logger),
			m_LocalComObj(new QLocalServer(this))
		{
			m_SocketList = new QMap<QString, QLocalSocket*>();

			//Jeder hat Zugriff auf den Server.
			m_LocalComObj->setSocketOptions(QLocalServer::WorldAccessOption);
			m_LocalComObj->setMaxPendingConnections(2);

			connect(m_LocalComObj, &QLocalServer::newConnection, this, &LocalCommunicationServer::OnPrepareIncomingConnection);
		}


		LocalCommunicationServer::~LocalCommunicationServer()
		{
			m_LocalComObj->close();
			qDeleteAll(*m_SocketList);
			delete m_SocketList;
		}


		bool LocalCommunicationServer::Listen()
		{
			if (m_LocalComObj->listen(m_ServerName))
			{
				m_Logger->debug("LocalCommunicationServer is started on ChannelName {}", m_ServerName.toStdString());
				return true;
			}
			else
			{
				m_Logger->error("Can't start local server on ChannelName {}", m_ServerName.toStdString());
				return false;
			}
		}

		void LocalCommunicationServer::OnPrepareIncomingConnection()
		{

            QLocalSocket* socket = m_LocalComObj->nextPendingConnection();

			if (socket->isValid())
			{
				m_Logger->debug("A new client is connected to the LocalCommunicationServer {}", socket->serverName().toStdString());

				connect(socket, SIGNAL(disconnected()), this, SLOT(OnSocketDisconnected()));
				connect(socket, SIGNAL(readyRead()), this, SLOT(OnExternalMessage()));
				connect(socket, SIGNAL(error(QLocalSocket::LocalSocketError)), this, SLOT(OnSocketError(QLocalSocket::LocalSocketError)));
			}
		}

		void LocalCommunicationServer::OnDisconnect()
		{
			QLocalSocket* LocalSocket = (QLocalSocket *)sender();

			for (auto i = m_SocketList->begin(); i != m_SocketList->end(); ++i)
			{
				if (LocalSocket == i.value())
				{
					m_Logger->debug("Client disonnected from LocalCommunicationServer: {}", i.key().toStdString());
					QLocalSocket* socket = i.value();
					socket->deleteLater();
					m_SocketList->erase(i);
				}
			}
		}

		void LocalCommunicationServer::OnSocketError(QLocalSocket::LocalSocketError Error)
		{
			switch (Error)
			{
			case QLocalSocket::ConnectionRefusedError:
				m_Logger->error("A LocalCommunicationServer socket error occoured: {}", "ConnectionRefusedError");
				break;
			case QLocalSocket::PeerClosedError:
				m_Logger->error("A LocalCommunicationServer socket error occoured: {}", "PeerClosedError");
				break;
			case QLocalSocket::ServerNotFoundError:
				m_Logger->error("A LocalCommunicationServer socket error occoured: {}", "ServerNotFoundError");
				break;
			case QLocalSocket::SocketAccessError:
				m_Logger->error("A LocalCommunicationServer socket error occoured: {}", "SocketAccessError");
				break;
			case QLocalSocket::SocketResourceError:
				m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", "SocketResourceError");
				break;
			case QLocalSocket::SocketTimeoutError:
				m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", "SocketTimeoutError");
				break;
			case QLocalSocket::DatagramTooLargeError:
				m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", "DatagramTooLargeError");
				break;
			case QLocalSocket::ConnectionError:
				m_Logger->error("A LocalCommunicationServer socket error occoured: {}", "ConnectionError");
				break;
			case QLocalSocket::UnsupportedSocketOperationError:
				m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", "UnsupportedSocketOperationError");
				break;
			case QLocalSocket::UnknownSocketError:
				m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", "UnknownSocketError");
				break;
			case QLocalSocket::OperationError:
				m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", "OperationError");
				break;
			default:
				break;
			}

		}

		void LocalCommunicationServer::OnProcessMessage(Message Msg)
		{
            if (!m_SocketList->contains(Msg.identifier()))
            {
                m_Logger->error("Couldn't find Client with ID {}", Msg.identifier().toStdString());
                return;
            }

			QLocalSocket* localSocket = m_SocketList->find(Msg.identifier()).value();
			QByteArray arr;
			QDataStream dataStream(&arr, QIODevice::OpenModeFlag::WriteOnly);
			dataStream << Msg;
			quint64 size = localSocket->write(arr);
			if (size < arr.size())
				m_Logger->warn("Uncomplete message was send to {}", Msg.identifier().toStdString());

			if (!localSocket->flush())
				m_Logger->error("Message couldn't send to {}", Msg.identifier().toStdString());
		}

		void LocalCommunicationServer::OnExternalMessage()
		{
			QLocalSocket* localSocket = (QLocalSocket *)sender();
			if (localSocket->bytesAvailable() <= 0)
				return;

			Message msg;
			QDataStream sizeStream(localSocket->readAll());
			sizeStream >> msg;

			if (m_SocketList == nullptr)
			{
				m_Logger->error("Socketlist is null!");
				return;
			}

			if (!m_SocketList->contains(msg.identifier()))
				m_SocketList->insert(msg.identifier(), localSocket);

			emit NewMessage(msg);
		}


	}
}