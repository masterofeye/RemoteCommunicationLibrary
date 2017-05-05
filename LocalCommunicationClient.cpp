#include "stdafx.h"
#include "LocalCommunicationClient.h"
#include "CommunicatonServer.h"

#include <qdatastream.h>

namespace RW{
    namespace COM{
        LocalCommunicationClient::LocalCommunicationClient(QString ServerName, std::shared_ptr<spdlog::logger> Logger, QObject* Parent) : BasicCommunicationServer(Parent),
            m_ServerName(ServerName),
            m_Logger(Logger),
            m_Client(new QLocalSocket(this))
        {
        }


        LocalCommunicationClient::~LocalCommunicationClient()
        {
        }

        bool LocalCommunicationClient::Listen()
        {
            connect(m_Client, &QLocalSocket::connected, this, &LocalCommunicationClient::OnPrepareIncomingConnection);
            connect(m_Client, &QLocalSocket::connected, this, &LocalCommunicationClient::SendWelcome);
            connect(m_Client, &QLocalSocket::disconnected, this, &LocalCommunicationClient::OnDisconnected);

            connect(m_Client, static_cast<void(QLocalSocket::*)(QLocalSocket::LocalSocketError)>(&QLocalSocket::error), this, &LocalCommunicationClient::OnSocketError);
            m_Client->connectToServer(m_ServerName);

            return true;
        }
        
        void LocalCommunicationClient::OnProcessMessage(Message Msg)
        {
            QByteArray arr;
            QDataStream dataStream(&arr, QIODevice::OpenModeFlag::WriteOnly);
            dataStream << Msg;
            quint64 size = m_Client->write(arr);
            if (size < arr.size())
                m_Logger->warn("Uncomplete message was send to {}", Msg.identifier().toStdString());

            if (!m_Client->flush())
                m_Logger->error("Message couldn't send to {}", Msg.identifier().toStdString());
        }

        void LocalCommunicationClient::OnPrepareIncomingConnection()
        {
            connect(m_Client, SIGNAL(readyRead()), this, SLOT(OnExternalMessage()));
            m_Logger->debug("LocalClient is connected");
            m_Logger->flush();
        }


        void LocalCommunicationClient::OnExternalMessage()
        {
            if (m_Client->bytesAvailable() <= 0)
                return;

            Message msg;
            QDataStream sizeStream(m_Client->readAll());
            sizeStream >> msg;

            emit NewMessage(msg);
        }

        void LocalCommunicationClient::OnExternalMessage(Message Msg)
        {
        }


        void LocalCommunicationClient::OnSocketError(QLocalSocket::LocalSocketError Error)
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

        void LocalCommunicationClient::OnDisconnected()
        {
			m_Client->deleteLater();
        }

        void LocalCommunicationClient::SendWelcome()
        {
            COM::Message msg;
            QString uuid = COM::CommunicatonServer::GenUUID(COM::CommunicatonServer::TypeofServer::RemoteHiddenHelper).toString();
            msg.setIdentifier(uuid);
            msg.SetMessageID(COM::MessageDescription::EX_WELCOME);
            msg.SetIsExternal(false);

            OnProcessMessage(msg);
        }
    }
}