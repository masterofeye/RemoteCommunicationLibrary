#include "GlobalCommunicationServer.h"
#include "GlobalCommunicationThread.h"
//TODO Könnte man eleganter gestalten
#include "Types.h"

#include <qdatastream>
namespace RW{
	namespace COM{
		GlobalCommunicationServer::GlobalCommunicationServer(quint16 Port, std::shared_ptr<spdlog::logger> Logger, QObject* Parent) : BasicCommunicationServer(Parent),
			m_Port(Port),
			m_Logger(Logger),
			m_TcpServer(new QTcpServer(this))
		{
			connect(m_TcpServer, &QTcpServer::newConnection, this, &GlobalCommunicationServer::OnPrepareIncomingConnection);
		}


		GlobalCommunicationServer::~GlobalCommunicationServer()
		{
		}

		bool GlobalCommunicationServer::Listen()
		{
			if (!m_TcpServer->listen(QHostAddress::Any, m_Port))
			{
                m_Logger->error("TcpServer couldn't listen on port {}. ErrorString: {}, ErrorID: {}", (int)spdlog::sinks::FilterType::GlobalCommunicationServer, m_Port, m_TcpServer->errorString().toStdString(), (qint8)m_TcpServer->serverError());
				return false;
			}
			else
			{
				m_Logger->debug("TcpServer listening on port {}", m_Port);
				return true;
			}
		}

		void GlobalCommunicationServer::OnPrepareIncomingConnection()
		{
			QTcpSocket *socket = m_TcpServer->nextPendingConnection();
			// We have a new connection
            m_Logger->debug("A new client {} has connected ", (int)spdlog::sinks::FilterType::GlobalCommunicationServer, socket->peerAddress().toString().toStdString());
			
            // Every new connection will be run in a newly created thread
            GlobalCommunicationThread *thread = new GlobalCommunicationThread(socket, m_Logger, this);
            socket->moveToThread(thread);
            // connect signal/slot
            // once a thread is not needed, it will be beleted later
            connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
            connect(this, &GlobalCommunicationServer::NewMessage, thread, &GlobalCommunicationThread::OnProcessMessage, Qt::QueuedConnection);
            connect(thread, SIGNAL(NewMessage(Message Msg)), this, SLOT(OnExternalMessage(Message Msg)), Qt::QueuedConnection);

            thread->start();
            
		}

		void GlobalCommunicationServer::OnProcessMessage(Message Msg)
		{
		}

		void GlobalCommunicationServer::OnExternalMessage()
		{
		
		}

		void GlobalCommunicationServer::OnExternalMessage(Message Msg)
		{
			emit NewMessage(Msg);
		}
	}
}