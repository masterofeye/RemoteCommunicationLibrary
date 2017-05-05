#pragma once
#include "stdafx.h"
#include <QObject>
#include "Message.h"

namespace RW{
	namespace COM{
		class REMOTECOMMUNICATIONLIBRARY_EXPORT BasicCommunicationServer :
			public QObject
		{
			Q_OBJECT
		public:
			BasicCommunicationServer(QObject* Parent = nullptr);
			~BasicCommunicationServer();

			virtual bool Listen() = 0;
		public slots:
			virtual void OnProcessMessage(COM::Message Msg) = 0;
			virtual void OnPrepareIncomingConnection() = 0;
			virtual void OnExternalMessage() = 0;
		signals:
			void NewMessage(COM::Message Msg);
		};
	}
}
