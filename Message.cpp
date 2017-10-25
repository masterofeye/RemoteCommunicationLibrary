#include "stdafx.h"
#include "Message.h"
#include <QDatastream>
#include "RemoteDataConnectLibrary.h"

namespace RW{
	namespace COM{
		//Message::Message() : QObject(nullptr)
		//{
		//
		//}


		Message::Message(QString Identifier,
			COM::MessageDescription MessageID,
			ExecutionVariant ExcVariant,
			QList<QVariant> ParameterListe,
            bool IsExternal,
            bool IsProcessed,
			QObject* Parent) : QObject(Parent),
			m_Identifier(Identifier),
			m_MessageID(MessageID),
			m_ExecutionVariant(ExcVariant),
			m_ParameterListe(ParameterListe),
            m_IsExternal(IsExternal),
            m_IsProcessed(IsProcessed)
		{
		}

		Message::Message(QString Identifier, 
			COM::MessageDescription MessageID,
			ExecutionVariant ExcVariant,
			QList<QVariant> ParameterListe,
			QVariant Result,
			bool Success,
            bool IsExternal,
            bool IsProcessed,
			QObject* Parent) : QObject(Parent),
			m_Identifier(Identifier),
			m_MessageID(MessageID),
			m_ExecutionVariant(ExcVariant),
			m_ParameterListe(ParameterListe),
			m_Result(Result),
			m_Success(Success),
            m_IsExternal(IsExternal),
            m_IsProcessed(IsProcessed)
		{
		}


		Message::~Message()
		{
		}

		Message::Message(const Message &obj)
		{
			m_ExecutionVariant = obj.m_ExecutionVariant;
			m_ParameterListe = obj.m_ParameterListe;
			m_MessageID = obj.m_MessageID;
            m_Identifier = obj.m_Identifier;
            m_Success = obj.m_Success;
            m_Result = obj.m_Result;
            m_IsExternal = obj.m_IsExternal;
            m_IsProcessed = obj.m_IsProcessed;
		}

		Message& Message::operator = (const Message& other)
		{
			Message tmp(other);         // re-use copy-constructor
			*this = std::move(tmp); // re-use move-assignment
			return *this;
		}

		Message::Message(Message&& other):
		m_MessageID(other.m_MessageID),
		m_ExecutionVariant(other.m_ExecutionVariant),
		m_ParameterListe(other.m_ParameterListe),
        m_Identifier(other.m_Identifier),
        m_Success(other.m_Success),
        m_Result(other.m_Result),
        m_IsExternal(other.m_IsExternal),
        m_IsProcessed(other.m_IsProcessed)
		{
		}

		Message& Message::operator = (Message&& other)
		{
			return *this;
		}

		QDataStream &operator <<(QDataStream &out, const RW::COM::MessageDescription &dataStruct)
		{
			out.startTransaction();
            out << (quint16) dataStruct;
			out.commitTransaction();
			return out;
		}

		QDataStream &operator >>(QDataStream &in, RW::COM::MessageDescription &dataStruct)
		{
            quint16 e;
			in >> e;
            dataStruct = (RW::COM::MessageDescription) e;
			return in;
		}


		QDataStream &operator <<(QDataStream &out, const RW::COM::Message::ExecutionVariant &dataStruct)
		{
			out.startTransaction();
            out << (quint16)dataStruct;
			out.commitTransaction();
			return out;
		}

		QDataStream &operator >>(QDataStream &in, RW::COM::Message::ExecutionVariant &dataStruct)
		{
            quint16 e;
            in >> e;
            dataStruct = (RW::COM::Message::ExecutionVariant) e;
			return in;
		}

		QDataStream &operator <<(QDataStream &out, const RW::COM::Message &dataStruct)
		{
			out.startTransaction();
			out << dataStruct.m_Identifier;
			out << dataStruct.m_MessageID;
			out << dataStruct.m_ExecutionVariant;
			out << dataStruct.m_ParameterListe;
			out << dataStruct.m_Result;
			out << dataStruct.m_Success;
            out << dataStruct.m_IsExternal;
            out << dataStruct.m_IsProcessed;
			out.commitTransaction();
			return out;
		}



		QDataStream &operator >>(QDataStream &in, RW::COM::Message &dataStruct)
		{
			RW::COM::Message::ExecutionVariant var;
			RW::COM::MessageDescription id;
			QList<QVariant> parameterList;
			QVariant result;
			bool success;
            bool isExternal;
            bool isProcessed;
			QString Identifier;

			in >> Identifier;
			in >> id;
			in >> var;
			in >> parameterList;
			in >> result;
			in >> success;
            in >> isExternal;
            in >> isProcessed;

            dataStruct.m_Identifier = Identifier;
			dataStruct.m_ParameterListe = parameterList;
			dataStruct.m_MessageID = id;
			dataStruct.m_Result = result;
			dataStruct.m_Success = success;
			dataStruct.m_ExecutionVariant = var;
            dataStruct.m_IsExternal = isExternal;
            dataStruct.m_IsProcessed = isProcessed;

			return in;
		}

        QUuid Message::GenUUID(TypeofServer ServerType)
        {
            switch (ServerType)
            {
            case RW::COM::TypeofServer::RemoteView:
                return QUuid("{00000000-1234-1234-1234-ABCDEF123456}");
                break;
            case RW::COM::TypeofServer::RemoteApp:
                return QUuid("{00000001-1234-1234-1234-ABCDEF123456}");
                break;
            case RW::COM::TypeofServer::RemoteService:
                return QUuid("{00000002-1234-1234-1234-ABCDEF123456}");
                break;
            case RW::COM::TypeofServer::RemoteHiddenHelper:
                return QUuid("{00000003-1234-1234-1234-ABCDEF123456}");
                break;
            case RW::COM::TypeofServer::ServiceTest:
                return QUuid("{00000004-1234-1234-1234-ABCDEF123456}");
                break;
            case RW::COM::TypeofServer::NON:
                return QUuid("{0000000A-1234-1234-1234-ABCDEF123456}");
                break;
            default:
                return QUuid().toString();
                break;

            }
        }
	}
}