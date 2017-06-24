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
            TypeofServer m_ServerType = TypeofServer::NON;
        public:
            LocalCommunicationClient(QString ServerName, std::shared_ptr<spdlog::logger> Logger, QObject* Parent = nullptr);
            ~LocalCommunicationClient();
            bool Listen() Q_DECL_OVERRIDE;

            /**
            @brief setzt den Type des Kommunikationservers für den Client. 
            Dies ist wichtig um bei der Welcome Nachricht die Quelle angeben zu können.
            @param TypeofServer Type des verwendeten Servers
            @return void
            */
            inline void SetTypeOfServer(TypeofServer Type){ m_ServerType = Type; }

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
