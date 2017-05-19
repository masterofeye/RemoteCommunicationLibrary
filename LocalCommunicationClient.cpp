#include "stdafx.h"
#include "LocalCommunicationClient.h"
#include "CommunicatonServer.h"
//TODO Könnte man eleganter gestalten
#include "Types.h"

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
            m_Logger->trace("LocalCommunicationClient::OnProcessMessage {}",(int)spdlog::sinks::FilterType::LocalCommunicationClient, (int)Msg.MessageID());

            if (size < arr.size())
                m_Logger->warn("Uncomplete message was send to {}", (int)spdlog::sinks::FilterType::LocalCommunicationClient, Msg.identifier().toStdString());

            if (!m_Client->flush())
                m_Logger->error("Message couldn't send to {}", (int)spdlog::sinks::FilterType::LocalCommunicationClient, Msg.identifier().toStdString());
            m_Logger->flush();
        }

        void LocalCommunicationClient::OnPrepareIncomingConnection()
        {
            connect(m_Client, SIGNAL(readyRead()), this, SLOT(OnExternalMessage()));
            m_Logger->debug("LocalClient is connected", (int)spdlog::sinks::FilterType::LocalCommunicationClient);
            m_Logger->flush();
        }


        void LocalCommunicationClient::OnExternalMessage()
        {
            if (m_Client->bytesAvailable() <= 0)
                return;

            Message msg;
            QDataStream sizeStream(m_Client->readAll());
            sizeStream >> msg;
            m_Logger->trace("Message was received {}", (int)spdlog::sinks::FilterType::LocalCommunicationClient, (int)msg.MessageID());
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
                m_Logger->error("A LocalCommunicationServer socket error occoured: {}", (int)spdlog::sinks::FilterType::LocalCommunicationClient, "ConnectionRefusedError");
                break;
            case QLocalSocket::PeerClosedError:
                m_Logger->error("A LocalCommunicationServer socket error occoured: {}", (int)spdlog::sinks::FilterType::LocalCommunicationClient, "PeerClosedError");
                break;
            case QLocalSocket::ServerNotFoundError:
                m_Logger->error("A LocalCommunicationServer socket error occoured: {}", (int)spdlog::sinks::FilterType::LocalCommunicationClient, "ServerNotFoundError");
                break;
            case QLocalSocket::SocketAccessError:
                m_Logger->error("A LocalCommunicationServer socket error occoured: {}", (int)spdlog::sinks::FilterType::LocalCommunicationClient, "SocketAccessError");
                break;
            case QLocalSocket::SocketResourceError:
                m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", (int)spdlog::sinks::FilterType::LocalCommunicationClient, "SocketResourceError");
                break;
            case QLocalSocket::SocketTimeoutError:
                m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", (int)spdlog::sinks::FilterType::LocalCommunicationClient, "SocketTimeoutError");
                break;
            case QLocalSocket::DatagramTooLargeError:
                m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", (int)spdlog::sinks::FilterType::LocalCommunicationClient, "DatagramTooLargeError");
                break;
            case QLocalSocket::ConnectionError:
                m_Logger->error("A LocalCommunicationServer socket error occoured: {}", (int)spdlog::sinks::FilterType::LocalCommunicationClient, "ConnectionError");
                break;
            case QLocalSocket::UnsupportedSocketOperationError:
                m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", (int)spdlog::sinks::FilterType::LocalCommunicationClient, "UnsupportedSocketOperationError");
                break;
            case QLocalSocket::UnknownSocketError:
                m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", (int)spdlog::sinks::FilterType::LocalCommunicationClient, "UnknownSocketError");
                break;
            case QLocalSocket::OperationError:
                m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", (int)spdlog::sinks::FilterType::LocalCommunicationClient, "OperationError");
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
            QString uuid = COM::Message::GenUUID(COM::TypeofServer::RemoteHiddenHelper).toString();
            msg.setIdentifier(uuid);
            msg.SetMessageID(COM::MessageDescription::EX_WELCOME);
            msg.SetIsExternal(false);

            OnProcessMessage(msg);
        }
    }
}