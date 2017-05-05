#include "CommunicatonServer.h"
#include "LocalCommunicationServer.h"
#include "GlobalCommunicationServer.h"
#include "LocalCommunicationClient.h"
#include "GlobalCommunicationClient.h"
#include "Message.h"

#include <QMetaobject>
#include <QMetaMethod>


namespace RW{
	namespace COM{
        CommunicatonServer::CommunicatonServer(bool IsServer, TypeofServer ServerType, QString LocalServerName, quint16 Port, std::shared_ptr<spdlog::logger> Logger, QObject* Parent) : BasicCommunicationServer(Parent),
			m_ReceiverList(new QList<QObject*>()),
			m_Logger(Logger),
            m_IsServer(IsServer),
            m_TypeofServer(ServerType)
		{
            if (IsServer)
            {
                m_LocalComObj = new LocalCommunicationServer("Server", Logger, this);
                m_GlobalComObj = new GlobalCommunicationServer(42364, Logger, this);
            }
            else
            {
                m_LocalComObj = new LocalCommunicationClient("Server", Logger, this);
                m_GlobalComObj = new GlobalCommunicationClient(42364, Logger, this);
            }

            
            bool res = connect(m_GlobalComObj, &GlobalCommunicationServer::NewMessage, this, static_cast<void(CommunicatonServer::*)(Message Msg)>(&CommunicatonServer::OnExternalMessage));
            if (!res)
                m_Logger->critical("Global communcation object can't connect to CommunicationServer OnExternalMessage");
            res = connect(m_LocalComObj, &LocalCommunicationServer::NewMessage, this, static_cast<void(CommunicatonServer::*)(Message Msg)>(&CommunicatonServer::OnExternalMessage));
            if (!res)
                m_Logger->critical("Local communcation object can't connect to CommunicationServer OnExternalMessage");
            res = connect(this, &CommunicatonServer::SendExtMessage, (GlobalCommunicationServer*)m_GlobalComObj, &GlobalCommunicationServer::OnProcessMessage);
            if (!res)
                m_Logger->critical("CommunicationServer SendExtMessage can't connect to Global OnProcessMessage");
            res = connect(this, &CommunicatonServer::SendExtMessage, (LocalCommunicationServer*)m_LocalComObj, &LocalCommunicationServer::OnProcessMessage);
            if (!res)
                m_Logger->critical("CommunicationServer SendExtMessage can't connect to Local OnProcessMessage");

            m_UUID =  GenUUID(ServerType);
		}


		CommunicatonServer::~CommunicatonServer()
		{

            //TODO der Listeninhalt wird nicht gelˆscht
            m_ReceiverList->clear();
            delete m_ReceiverList;
			m_ReceiverList = nullptr;
		}

        QUuid CommunicatonServer::GenUUID(TypeofServer ServerType)
        {
            switch (ServerType)
            {
            case RW::COM::CommunicatonServer::TypeofServer::RemoteView:
                return QUuid("{00000000-1234-1234-1234-ABCDEF123456}");
                break;
            case RW::COM::CommunicatonServer::TypeofServer::RemoteApp:
                return QUuid("{00000001-1234-1234-1234-ABCDEF123456}");
                break;
            case RW::COM::CommunicatonServer::TypeofServer::RemoteService:
                return QUuid("{00000002-1234-1234-1234-ABCDEF123456}");
                break;
            case RW::COM::CommunicatonServer::TypeofServer::RemoteHiddenHelper:
                return QUuid("{00000003-1234-1234-1234-ABCDEF123456}");
                break;
            case RW::COM::CommunicatonServer::TypeofServer::ServiceTest:
                return QUuid("{00000004-1234-1234-1234-ABCDEF123456}");
                break;
            case RW::COM::CommunicatonServer::TypeofServer::NON:
                return QUuid("{0000000A-1234-1234-1234-ABCDEF123456}");
                break;
            default:
                return QUuid().toString();
                break;
                
            }
        }

		bool CommunicatonServer::Listen()
		{
			if (!m_LocalComObj->Listen())
			{
				return false;
			}

			if (!m_GlobalComObj->Listen())
			{
				return false;
			}

			return true;
		}

		void CommunicatonServer::Register(QObject* Client)
		{
			if (Client == nullptr)
			{
				m_Logger->error("Can't register Client because he is null for Object {}", Client->objectName().toStdString());
			}
			else
			{
				//Wir brauchen das MetaObjekt des Receivers um eine Liste der Methoden zu bekommen
				const QMetaObject* ClientMetaObject = Client->metaObject();
				if (ClientMetaObject == nullptr)
				{
					m_Logger->error("Meta object is null for Object {}", Client->objectName().toStdString());
					return;
				}

				//Pr¸fen ob im Receiver Objekt die OnMessage Methode existiert
				int clientMethodIndex = ClientMetaObject->indexOfSlot(QMetaObject::normalizedSignature("OnProcessMessage(COM::Message)"));
                
				//TODO MagicNumber
				if (clientMethodIndex == -1)
				{
					m_Logger->warn("There is no function called OnProcessMessage for Object {}", Client->metaObject()->className());
					return;
				}

				QMetaMethod clientOnProcessMessageMethod = ClientMetaObject->method(clientMethodIndex);

				const QMetaObject* ServerMetaObject = this->metaObject();
				if (ServerMetaObject == nullptr)
				{
					m_Logger->error("Meta object is null for Object {}", this->metaObject()->className());
					return;
				}

				int ServerSignalIndex = ServerMetaObject->indexOfSignal(QMetaObject::normalizedSignature("NewMessage(COM::Message)"));
				//TODO MagicNumber
				if (ServerSignalIndex == -1)
				{
					m_Logger->warn("No Signal found with name NewCommand for Object {}", this->metaObject()->className());
					return;
				}

				QMetaMethod serverSignal = ServerMetaObject->method(ServerSignalIndex);

				QMetaObject::Connection connectionServerClient = QObject::connect(this, serverSignal, Client, clientOnProcessMessageMethod);
				if (((bool)connectionServerClient) == false)
				{
					m_Logger->error("Connection couldn't established for Object:{}", Client->metaObject()->className());
				}
				else
				{
					m_Logger->trace("Receiver was successfully connected to the signal. {}", Client->metaObject()->className());
				}

				int serverMethodIndex = ServerMetaObject->indexOfSlot(QMetaObject::normalizedSignature("OnProcessMessage(COM::Message)"));
				if (serverMethodIndex == -1)
				{
					m_Logger->warn("There is no function called OnProcessMessage for Object {}", this->metaObject()->className());
					return;
				}

				QMetaMethod serverOnProcessMessageMethod = ServerMetaObject->method(serverMethodIndex);

				int clientSignalIndex = ClientMetaObject->indexOfSignal(QMetaObject::normalizedSignature("NewMessage(COM::Message)"));
				//TODO MagicNumber
				if (clientSignalIndex == -1)
				{
					m_Logger->warn("No Signal found with name NewCommand for Object {}", this->metaObject()->className());
					return;
				}

				QMetaMethod clientSignal = ClientMetaObject->method(clientSignalIndex);

				QMetaObject::Connection connectionClientServer = QObject::connect(Client, clientSignal, this, serverOnProcessMessageMethod);
				if (((bool)connectionClientServer) == false)
				{
					m_Logger->error("Connection couldn't established for Object:{}", this->metaObject()->className());
				}
				else
				{
                    m_Logger->trace("Receiver was successfully connected to the signal. {}", this->metaObject()->className());
				}

				m_ReceiverList->append(Client);
			}
		}


		void CommunicatonServer::Unregister(QObject* Receiver)
		{
			QObject::disconnect(Receiver);
			m_Logger->debug("The client {} is disconnected now", Receiver->objectName().toStdString());
		}
		void CommunicatonServer::UnregisterAll()
		{
			for each (auto var in *m_ReceiverList)
			{
				QObject::disconnect(var);
			}
			m_Logger->debug("All receivers are disconnected now");
		}

		void CommunicatonServer::OnProcessMessage(Message Msg)
		{
			QObject* obj = QObject::sender();

			//Auschlieﬂen das wir Nachrichten empfangen die von CommunicatonServer::OnProcessMessage gesendet wurden.
			if (qobject_cast<GlobalCommunicationServer*>(obj) != nullptr)
				return;
			if (qobject_cast<LocalCommunicationServer*>(obj) != nullptr)
				return;
            if (qobject_cast<LocalCommunicationClient*>(obj) != nullptr)
                return;
            if (qobject_cast<GlobalCommunicationClient*>(obj) != nullptr)
                return;

			if (Msg.IsExternal())
			{
                //Der Indentifier wird nur gesetzt wenn es sich um einen Clienten handelt. Sonst kann aktuell der LocalCommunicationServer bzw GlobalCommunicationServer nicht senden, 
                //da wir nicht zwischen Empf‰nger und Sender ID unterscheiden.
                if (!m_IsServer)
				    Msg.setIdentifier(m_UUID.toString());
				emit SendExtMessage(Msg);
			}
			emit NewMessage(Msg);
		}

		void CommunicatonServer::OnPrepareIncomingConnection()
		{}

		void CommunicatonServer::OnExternalMessage()
		{

		}

		void CommunicatonServer::OnExternalMessage(Message Msg)
		{
			QObject* obj = QObject::sender();

			//if (qobject_cast<GlobalCommunicationServer*>(obj) == nullptr || qobject_cast<LocalCommunicationServer*>(obj) == nullptr)
			//	return;

			emit NewMessage(Msg);
		}
	}
}