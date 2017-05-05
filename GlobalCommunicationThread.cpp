#include "stdafx.h"
#include "GlobalCommunicationThread.h"
#include <qhostaddress.h>
#include <qdatastream>
#include <qfile.h>
#include <QDomDocument>


#include <Wtsapi32.h>
#include <userenv.h>
#include <windows.h> 
#include <lm.h>


#pragma comment(lib, "wtsapi32.lib")
#pragma comment(lib, "userenv.lib")
#pragma comment(lib, "netapi32.lib")


namespace RW{
	namespace COM{
		GlobalCommunicationThread::GlobalCommunicationThread(QTcpSocket* ID, std::shared_ptr<spdlog::logger> Logger, QObject *parent) :
			QThread(parent),
			m_Logger(Logger),
			m_Socket(ID)
			{
			}


			GlobalCommunicationThread::~GlobalCommunicationThread()
			{
			}

			void GlobalCommunicationThread::run()
			{
				//m_Socket = new QTcpSocket();

				//// set the ID
				//if (!m_Socket->setSocketDescriptor(m_SocketDescriptor))
				//{
				//	// something's wrong, we just emit a signal
				//	emit error(m_Socket->error());
				//	return;
				//}

				// connect socket and signal
				// note - Qt::DirectConnection is used because it's multithreaded
				//        This makes the slot to be invoked immediately, when the signal is emitted.

				connect(m_Socket, SIGNAL(readyRead()), this, SLOT(OnExternalMessage()), Qt::DirectConnection);
				connect(m_Socket, SIGNAL(disconnected()), this, SLOT(OnDisconnect()));
				connect(m_Socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(OnSocketError(QAbstractSocket::SocketError)));

				// We'll have multiple clients, we want to know which is which
				m_Logger->debug("Client connected to GlobalServer {}",m_Socket->localAddress().toString().toStdString());

				// make this thread a loop,
				// thread will stay alive so that signal/slot to function properly
				// not dropped out in the middle when thread dies

				exec();
			}


			void GlobalCommunicationThread::OnProcessMessage(Message Msg)
			{
				QByteArray arr;
				QDataStream dataStream(&arr, QIODevice::OpenModeFlag::WriteOnly);
				dataStream << Msg;
				quint64 size = m_Socket->write(arr);
				if (size < arr.size())
					m_Logger->warn("Uncomplete message was send to {}", Msg.identifier().toStdString());

				if (!m_Socket->flush())
					m_Logger->error("Message couldn't send to {}", Msg.identifier().toStdString());
			}

			void GlobalCommunicationThread::OnExternalMessage()
			{
				if (m_Socket->bytesAvailable() <= 0)
				{
					m_Logger->warn("TcpSocket don't have any data received but was triggered.");
					return;
				}

                QByteArray command = m_Socket->readAll();
                if (QString(command).contains("UserInfo"))
                {
                    QString username;
                    if(!ReturnCurrentUser(username))
                    {
                        m_Logger->error("Couldn't read the username from the system");
                    }
                    
                    QString answer;
                    //answer += "UserInfo";
                    //answer.append(",");
                    answer += username;
                    answer.append(",");
                    answer.append(LongTerm() ? "true" : "false");
                    answer.append(",");

                    char computername[255];
                    DWORD computernane_len = 255;
                    GetComputerNameA(computername, &computernane_len);

                    answer += computername;
                    answer.append(",");
                    answer += "192.156.244.111";
                    answer.append(",");
                    answer += "192.156.244.111";
                    answer.append(",");
                    answer += "";
                    answer.append(",");
                    answer += "Streamlist{}";
                    m_Socket->write(answer.toUtf8());
                    m_Socket->flush();
                }
                else if (QString(command).contains("Enabled"))
                {
                    return;
                }
                else if (QString(command).contains("Disabled"))
                {
                    return;
                }
                else if (QString(command).contains("Reason"))
                {
                    return;
                }
                else if (QString(command).contains("WDM-Analysis"))
                {
                    return;
                }
                else if (QString(command).contains("Endurance Test"))
                {
                    return;
                }
                else if (QString(command).contains("SSTS-Analysis"))
                {
                    return;
                }
                else if (QString(command).contains("MR-Test"))
                {
                    return;
                }
                else if (QString(command).contains("Other"))
                {
                    return;
                }
                else
                {
                    try{
                        Message msg;
                        QDataStream sizeStream(command);
                        sizeStream >> msg;

                        emit NewMessage(msg);
                    }
                    catch (...)
                    {
                        m_Logger->error("Error durring message processing");
                    }
                }
			}

			
			void GlobalCommunicationThread::OnDisconnect()
			{
				QTcpSocket *socket = (QTcpSocket*)qobject_cast<QAbstractSocket*>(sender());
				if (socket) {
					m_Logger->debug("Client {} disconnected from the server.", socket->peerAddress().toString().toStdString());
					socket->deleteLater();
				}
			}

			void GlobalCommunicationThread::OnSocketError(QAbstractSocket::SocketError socketError)
			{
				switch (socketError)
				{
				case QAbstractSocket::ConnectionRefusedError:
					m_Logger->error("A GlobalCommunicationClient socket error occoured: {}", "ConnectionRefusedError");
					break;
				case QAbstractSocket::AddressInUseError:
					m_Logger->error("A GlobalCommunicationClient socket error occoured: {}", "AddressInUseError");
					break;
				case QAbstractSocket::TemporaryError:
					m_Logger->error("A GlobalCommunicationClient socket error occoured: {}", "ServerNotFoundError");
					break;
				case QAbstractSocket::SocketAccessError:
					m_Logger->error("A GlobalCommunicationClient socket error occoured: {}", "SocketAccessError");
					break;
				case QAbstractSocket::SocketResourceError:
					m_Logger->error("A GlobalCommunicationClient socket error occoured:  {}", "SocketResourceError");
					break;
				case QAbstractSocket::SocketTimeoutError:
					m_Logger->error("A GlobalCommunicationClient socket error occoured:  {}", "SocketTimeoutError");
					break;
				case QAbstractSocket::DatagramTooLargeError:
					m_Logger->error("A GlobalCommunicationClient socket error occoured:  {}", "DatagramTooLargeError");
					break;
				case QAbstractSocket::UnsupportedSocketOperationError:
					m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", "UnsupportedSocketOperationError");
					break;
				case QAbstractSocket::UnknownSocketError:
					m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", "UnknownSocketError");
					break;
				case QAbstractSocket::OperationError:
					m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", "OperationError");
					break;
				case QAbstractSocket::UnfinishedSocketOperationError:
					m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", "UnfinishedSocketOperationError");
					break;
				case QAbstractSocket::NetworkError:
					m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", "NetworkError");
					break;
				case QAbstractSocket::HostNotFoundError:
					m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", "HostNotFoundError");
					break;
				case QAbstractSocket::RemoteHostClosedError:
					m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", "RemoteHostClosedError");
					break;
				case QAbstractSocket::ProxyAuthenticationRequiredError:
					m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", "ProxyAuthenticationRequiredError");
					break;
				case QAbstractSocket::SslHandshakeFailedError:
					m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", "SslHandshakeFailedError");
					break;
				case QAbstractSocket::ProxyConnectionRefusedError:
					m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", "UnfinishedSocketOperationError");
					break;
				case QAbstractSocket::ProxyConnectionClosedError:
					m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", "ProxyConnectionClosedError");
					break;
				case QAbstractSocket::ProxyConnectionTimeoutError:
					m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", "ProxyConnectionTimeoutError");
					break;
				case QAbstractSocket::ProxyNotFoundError:
					m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", "ProxyNotFoundError");
					break;
				case QAbstractSocket::ProxyProtocolError:
					m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", "ProxyProtocolError");
					break;
				case QAbstractSocket::SslInternalError:
					m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", "SslInternalError");
					break;
				case QAbstractSocket::SslInvalidUserDataError:
					m_Logger->error("A LocalCommunicationServer socket error occoured:  {}", "SslInvalidUserDataError");
					break;
				default:
					break;
				}
			}


            bool GlobalCommunicationThread::LongTerm()
            {
                QFile inFile("C:\\Program Files (x86)\\Schleissheimer\\RemoteSoftware\\RemoteAPP2\\config.xml");
                if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text))
                {
                    qDebug("Failed to open file for reading.");
                    return 0;
                }

                QDomDocument document;
                if (!document.setContent(&inFile))
                {
                    qDebug("Failed to parse the file into a DOM tree.");
                    inFile.close();
                    return 0;
                }

                inFile.close();

                QDomElement documentElement = document.documentElement();
                QDomNodeList elements = documentElement.elementsByTagName("LongTerm");
                if (elements.size() == 0)
                {
                    return false;
                }
                else if (elements.size() == 1)
                {
                    QDomElement bar = elements.at(0).toElement();
                    QString value = bar.text();
                    if (value.contains("Disabled"))
                    {
                        return false;
                    }
                    else
                    {
                        return true;
                    }
                }
                return false;
            }



            bool GlobalCommunicationThread::ReturnCurrentUser(QString &Username)
            {
                LPWKSTA_USER_INFO_0 pBuf = NULL;
                LPWKSTA_USER_INFO_0 pTmpBuf;
                DWORD dwLevel = 0;
                DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
                DWORD dwEntriesRead = 0;
                DWORD dwTotalEntries = 0;
                DWORD dwResumeHandle = 0;
                DWORD i;
                DWORD dwTotalCount = 0;
                NET_API_STATUS nStatus;
                LPWSTR pszServerName = NULL;
                do // begin do
                {
                    nStatus = NetWkstaUserEnum(pszServerName,
                        dwLevel,
                        (LPBYTE*)&pBuf,
                        dwPrefMaxLen,
                        &dwEntriesRead,
                        &dwTotalEntries,
                        &dwResumeHandle);
                    if ((nStatus == NERR_Success) || (nStatus == ERROR_MORE_DATA))
                    {
                        if ((pTmpBuf = pBuf) != NULL)
                        {
                            //
                            // Loop through the entries.
                            //
                            for (i = 0; (i < dwEntriesRead); i++)
                            {
                                assert(pTmpBuf != NULL);

                                if (pTmpBuf == NULL)
                                {
                                    //
                                    // Only members of the Administrators local group
                                    //  can successfully execute NetWkstaUserEnum
                                    //  locally and on a remote server.
                                    //

                                    m_Logger->error("An access violation has occurred");
                                    break;
                                }
                                //
                                // Print the user logged on to the workstation. 
                                //

                                m_Logger->trace(QString::fromStdWString(pTmpBuf->wkui0_username).toStdString());
                                QString user = QString::fromStdWString(pTmpBuf->wkui0_username);
                                if (!user.contains("$"))
                                    Username = user;

                                pTmpBuf++;
                                dwTotalCount++;
                            }
                        }
                    }
                    else
                        m_Logger->error("A system error has occurred : {0:d}", nStatus);
                    //
                    // Free the allocated memory.
                    //
                    if (pBuf != NULL)
                    {
                        NetApiBufferFree(pBuf);
                        pBuf = NULL;
                    }
                } while (nStatus == ERROR_MORE_DATA); // end do
                //
                // Check again for allocated memory.
                //
                if (pBuf != NULL)
                    NetApiBufferFree(pBuf);
                m_Logger->trace("Total of {0:d} entries enumerated", dwTotalCount);
                return true;
            }
	}
}