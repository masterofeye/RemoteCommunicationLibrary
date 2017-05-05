#pragma once
#include "stdafx.h"
#include <QObject>
#include <QTcpSocket>
#include <QThread>

#include "Message.h"

namespace RW{
	namespace COM{
		class REMOTECOMMUNICATIONLIBRARY_EXPORT GlobalCommunicationThread :
			public QThread
		{
			Q_OBJECT
		private:
			std::shared_ptr<spdlog::logger> m_Logger;
			QTcpSocket* m_Socket;
		public:
			GlobalCommunicationThread(QTcpSocket* ID, std::shared_ptr<spdlog::logger> Logger, QObject *parent = nullptr);
			~GlobalCommunicationThread();
			void run();
        private:
            bool ReturnCurrentUser(QString &Username);
            bool LongTerm();
		public slots:
			void OnDisconnect();
			void OnProcessMessage(Message Msg);
			void OnExternalMessage();
			void OnSocketError(QAbstractSocket::SocketError socketError);
		signals:
			void error(QTcpSocket::SocketError socketerror);
			void NewMessage(Message Msg);
		};
	}
}
