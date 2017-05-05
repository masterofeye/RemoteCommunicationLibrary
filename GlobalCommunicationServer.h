#pragma once
#include "stdafx.h"
#include "BasicCommunicationServer.h"
#include <QTcpsocket>
#include <QTcpserver>

namespace RW{
	namespace COM{
		class REMOTECOMMUNICATIONLIBRARY_EXPORT GlobalCommunicationServer :
			public BasicCommunicationServer
		{
			Q_OBJECT
		private:
			quint16 m_Port;
			std::shared_ptr<spdlog::logger> m_Logger;
			QTcpServer* m_TcpServer;
			QMap<QString, QTcpSocket*>* m_SocketList;
		public:
			GlobalCommunicationServer(quint16 Port, std::shared_ptr<spdlog::logger> Logger, QObject* Parent = nullptr);
			~GlobalCommunicationServer();

			bool Listen() Q_DECL_OVERRIDE;
		public slots:
			void OnPrepareIncomingConnection() Q_DECL_OVERRIDE;
			void OnProcessMessage(Message Msg) Q_DECL_OVERRIDE;
			void OnExternalMessage() Q_DECL_OVERRIDE;
			void OnExternalMessage(Message Msg);
		};
	}
}

