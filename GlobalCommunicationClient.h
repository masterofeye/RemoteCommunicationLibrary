#pragma once
#include "BasicCommunicationServer.h"
#include "qtcpsocket.h"

namespace RW{
    namespace COM{
        class REMOTECOMMUNICATIONLIBRARY_EXPORT GlobalCommunicationClient : public BasicCommunicationServer
        {
            Q_OBJECT
        private:
            QTcpSocket* m_Client = nullptr;
            std::shared_ptr<spdlog::logger> m_Logger;
            quint16 m_Port = 0;
        public:
            GlobalCommunicationClient(quint16 Port, std::shared_ptr<spdlog::logger> Logger, QObject* Parent = nullptr);
            ~GlobalCommunicationClient();

            bool Listen();
            public slots:
            void OnPrepareIncomingConnection() Q_DECL_OVERRIDE;
            void OnProcessMessage(Message Msg) Q_DECL_OVERRIDE;
            void OnExternalMessage() Q_DECL_OVERRIDE;
            void OnExternalMessage(Message Msg);
            void OnSocketError(QAbstractSocket::SocketError socketError);
            void OnDisconnected();
        };
    }
}

