#pragma once
#include "stdafx.h"
#include <QLocalServer>
#include <QLocalSocket>

#include "BasicCommunicationServer.h"

namespace RW{
	namespace COM{
		class REMOTECOMMUNICATIONLIBRARY_EXPORT LocalCommunicationServer :
			public BasicCommunicationServer
		{
			Q_OBJECT
		private:
			QString m_ServerName;
			std::shared_ptr<spdlog::logger> m_Logger;
			QLocalServer* m_LocalComObj;
			QMap<QString, QLocalSocket*>* m_SocketList;
		public:
			LocalCommunicationServer(QString ServerName, std::shared_ptr<spdlog::logger> Logger, QObject* Parent = nullptr);
			~LocalCommunicationServer();
			bool Listen() Q_DECL_OVERRIDE;
		public slots:
			void OnPrepareIncomingConnection() Q_DECL_OVERRIDE;

			/******************************************************************************************************************
			@autor Ivo Kunadt
			@brief Wird aufgerufen sobald der LocalCommunicationServer gestoppt werden soll.
			@return void
			********************************************************************************************************************/
			virtual void OnDisconnect();

			/******************************************************************************************************************
			@autor Ivo Kunadt
			@brief Diese Funktion wird aktuell nicht benötigt da sie durch die in Parameter abweichende OnSocketError
			ersetzt wird.
			@param Error Gibt die Fehlercode an, warum vorhergehende Funktion eine Fehler hervorgerufen hat.
			@return void
			********************************************************************************************************************/
			virtual void OnSocketError(QLocalSocket::LocalSocketError);

			void LocalCommunicationServer::OnExternalMessage()  Q_DECL_OVERRIDE;

			void OnProcessMessage(Message Msg) Q_DECL_OVERRIDE;
		};
	}
}
