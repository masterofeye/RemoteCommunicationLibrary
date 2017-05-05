#pragma once
#include "BasicCommunicationServer.h"
#include "qlocalsocket.h"

namespace RW{
    namespace COM{
        class REMOTECOMMUNICATIONLIBRARY_EXPORT LocalCommunicationClient : public BasicCommunicationServer
        {
            Q_OBJECT
        private:
            QLocalSocket* m_Client = nullptr;
            std::shared_ptr<spdlog::logger> m_Logger;
            QString m_ServerName = "";
        public:
            LocalCommunicationClient(QString ServerName, std::shared_ptr<spdlog::logger> Logger, QObject* Parent = nullptr);
            ~LocalCommunicationClient();
            bool Listen() Q_DECL_OVERRIDE;
        public slots:
            void OnPrepareIncomingConnection() Q_DECL_OVERRIDE;
            void OnProcessMessage(Message Msg) Q_DECL_OVERRIDE;
            void OnExternalMessage() Q_DECL_OVERRIDE;
            void OnExternalMessage(Message Msg);
            void OnSocketError(QLocalSocket::LocalSocketError Error);
            void OnDisconnected();
            void SendWelcome();
        signals:

        };
    }

}
