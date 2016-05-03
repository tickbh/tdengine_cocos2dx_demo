#include "TDSocket.h"

TDSocket::TDSocket(SOCKET socket/* = INVALID_SOCKET */, CSocketType nType /* = SocketTypeTcp*/ ) :
m_socket(socket),
m_socketErrno(TDSocket::SocketInvalidSocket),
m_nBufferSize(0), m_nSocketDomain(AF_INET),
m_nSocketType(SocketTypeInvalid), m_nBytesReceived(-1),
m_nBytesSent(-1), m_nFlags(0),
m_bIsBlocking(true),
m_bIsConnect(false)
{
	SetConnectTimeout(1, 0);
	memset(&m_stRecvTimeout, 0, sizeof(struct timeval));
	memset(&m_stSendTimeout, 0, sizeof(struct timeval));
	memset(&m_stLinger, 0, sizeof(struct linger));

	switch (nType)
	{
		//----------------------------------------------------------------------
		// Declare socket type stream - TCP
		//----------------------------------------------------------------------
	case TDSocket::SocketTypeTcp:
	{
		m_nSocketDomain = AF_INET;
		m_nSocketType = TDSocket::SocketTypeTcp;
		break;
	}
	case TDSocket::SocketTypeTcp6:
	{
		m_nSocketDomain = AF_INET6;
		m_nSocketType = TDSocket::SocketTypeTcp6;
		break;
	}
		//----------------------------------------------------------------------
		// Declare socket type datagram - UDP
		//----------------------------------------------------------------------
	case TDSocket::SocketTypeUdp:
	{
		m_nSocketDomain = AF_INET;
		m_nSocketType = TDSocket::SocketTypeUdp;
		break;
	}
	case TDSocket::SocketTypeUdp6:
	{
		m_nSocketDomain = AF_INET6;
		m_nSocketType = TDSocket::SocketTypeUdp6;
		break;
	}
		//----------------------------------------------------------------------
		// Declare socket type raw Ethernet - Ethernet
		//----------------------------------------------------------------------
	case TDSocket::SocketTypeRaw:
	{
#ifdef _LINUX
		m_nSocketDomain = AF_PACKET;
		m_nSocketType = TDSocket::SocketTypeRaw;
#endif
#ifdef WIN32
		m_nSocketType = TDSocket::SocketTypeInvalid;
#endif
		break;
	}
	default:
		m_nSocketType = TDSocket::SocketTypeInvalid;
		break;
	}
}

TDSocket::TDSocket(TDSocket &socket)
{
	m_nBufferSize = socket.m_nBufferSize;
	m_pBuffer.resize(m_nBufferSize);
	memcpy(&m_pBuffer[0], &socket.m_pBuffer[0], m_nBufferSize);
}

TDSocket *TDSocket::operator=(TDSocket &socket)
{
	if (m_nBufferSize != socket.m_nBufferSize)
	{
		m_nBufferSize = socket.m_nBufferSize;
		m_pBuffer.resize(m_nBufferSize);
		memcpy(&m_pBuffer[0], &socket.m_pBuffer[0], m_nBufferSize);
	}

	return this;
}


//------------------------------------------------------------------------------
//
// Initialize() - Initialize socket class
//
//------------------------------------------------------------------------------
bool TDSocket::Initialize()
{
	errno = TDSocket::SocketSuccess;

#ifdef WIN32
	//-------------------------------------------------------------------------
	// Data structure containing general Windows Sockets Info                
	//-------------------------------------------------------------------------
	//memset(&m_hWSAData, 0, sizeof(m_hWSAData));
	WSAStartup(MAKEWORD(2, 0), &m_hWSAData);

#endif

	//-------------------------------------------------------------------------
	// Create the basic Socket Handle										 
	//-------------------------------------------------------------------------
	m_socket = socket(m_nSocketDomain, m_nSocketType, 0);

	TranslateSocketError();

	return (IsSocketValid());
}


//------------------------------------------------------------------------------
//
// BindInterface()
//
//------------------------------------------------------------------------------
bool TDSocket::BindInterface(u8 *pInterface)
{
	bool           bRetVal = false;
	struct in_addr stInterfaceAddr;

	if (GetMulticast() == true)
	{
		if (pInterface)
		{
			stInterfaceAddr.s_addr = inet_addr((const char *)pInterface);
			if (SETSOCKOPT(m_socket, IPPROTO_IP, IP_MULTICAST_IF, &stInterfaceAddr, sizeof(stInterfaceAddr)) == SocketSuccess)
			{
				bRetVal = true;
			}
		}
	}
	else
	{
		SetSocketError(TDSocket::SocketProtocolError);
	}

	return bRetVal;
}


//------------------------------------------------------------------------------
//
// SetMulticast()
//
//------------------------------------------------------------------------------
bool TDSocket::SetMulticast(bool bEnable, u8 multicastTTL)
{
	bool bRetVal = false;

	if (GetSocketType() == TDSocket::SocketTypeUdp)
	{
		m_bIsMulticast = bEnable;
		if (SETSOCKOPT(m_socket, IPPROTO_IP, IP_MULTICAST_TTL, (void *)&multicastTTL, sizeof(multicastTTL)) == SocketError)
		{
			TranslateSocketError();
			bRetVal = false;
		}
		else
		{
			bRetVal = true;
		}
	}
	else
	{
		m_socketErrno = TDSocket::SocketProtocolError;
	}

	return bRetVal;
}

//------------------------------------------------------------------------------
//
// SetSocketDscp() 
//
//------------------------------------------------------------------------------
bool TDSocket::SetSocketDscp(i32 nDscp)
{
	bool  bRetVal = true;
	i32 nTempVal = nDscp;

	nTempVal <<= 4;
	nTempVal /= 4;

	if (IsSocketValid())
	{
		if (SETSOCKOPT(m_socket, IPPROTO_IP, IP_TOS, &nTempVal, sizeof(nTempVal)) == SocketError)
		{
			TranslateSocketError();
			bRetVal = false;
		}
	}

	return bRetVal;
}

//------------------------------------------------------------------------------
//
// GetSocketDscp() 
//
//------------------------------------------------------------------------------
i32 TDSocket::GetSocketDscp(void)
{
	i32      nTempVal = 0;
	socklen_t  nLen = 0;

	if (IsSocketValid())
	{
		if (GETSOCKOPT(m_socket, IPPROTO_IP, IP_TOS, &nTempVal, &nLen) == SocketError)
		{
			TranslateSocketError();
		}

		nTempVal *= 4;
		nTempVal >>= 4;
	}

	return nTempVal;
}

//------------------------------------------------------------------------------
//
// GetWindowSize() 
//
//------------------------------------------------------------------------------
u16 TDSocket::GetWindowSize(u32 nOptionName)
{
	u32 nTcpWinSize = 0;

	//-------------------------------------------------------------------------
	// no socket given, return system default allocate our own new socket
	//-------------------------------------------------------------------------
	if (m_socket != TDSocket::SocketError)
	{
		socklen_t nLen = sizeof(nTcpWinSize);

		//---------------------------------------------------------------------
		// query for buffer size 
		//---------------------------------------------------------------------
		GETSOCKOPT(m_socket, SOL_SOCKET, nOptionName, &nTcpWinSize, &nLen);
		TranslateSocketError();
	}
	else
	{
		SetSocketError(TDSocket::SocketInvalidSocket);
	}

	return nTcpWinSize;
}

//------------------------------------------------------------------------------
//
// SetWindowSize()
//
//------------------------------------------------------------------------------
u16 TDSocket::SetWindowSize(u32 nOptionName, u32 nWindowSize)
{
	u32 nRetVal = 0;

	//-------------------------------------------------------------------------
	// no socket given, return system default allocate our own new socket
	//-------------------------------------------------------------------------
	if (m_socket != TDSocket::SocketError)
	{
		nRetVal = SETSOCKOPT(m_socket, SOL_SOCKET, nOptionName, &nWindowSize, sizeof(nWindowSize));
		TranslateSocketError();
	}
	else
	{
		SetSocketError(TDSocket::SocketInvalidSocket);
	}

	return (u16)nWindowSize;
}


//------------------------------------------------------------------------------
//
// DisableNagleAlgorithm()
//
//------------------------------------------------------------------------------
bool TDSocket::DisableNagleAlgoritm()
{
	bool  bRetVal = false;
	i32 nTcpNoDelay = 1;

	//----------------------------------------------------------------------
	// Set TCP NoDelay flag to true
	//----------------------------------------------------------------------
	if (SETSOCKOPT(m_socket, IPPROTO_TCP, TCP_NODELAY, &nTcpNoDelay, sizeof(i32)) == 0)
	{
		bRetVal = true;
	}

	TranslateSocketError();

	return bRetVal;
}

//------------------------------------------------------------------------------
//
// EnableNagleAlgorithm()
//
//------------------------------------------------------------------------------
bool TDSocket::EnableNagleAlgoritm()
{
	bool  bRetVal = false;
	i32 nTcpNoDelay = 0;

	//----------------------------------------------------------------------
	// Set TCP NoDelay flag to false
	//----------------------------------------------------------------------
	if (SETSOCKOPT(m_socket, IPPROTO_TCP, TCP_NODELAY, &nTcpNoDelay, sizeof(i32)) == 0)
	{
		bRetVal = true;
	}

	TranslateSocketError();

	return bRetVal;
}

//------------------------------------------------------------------------------
//
// Send() - Send data on a valid socket
//
//------------------------------------------------------------------------------
i32 TDSocket::Send(const char *pBuf, size_t bytesToSend)
{
	SetSocketError(SocketSuccess);
	m_nBytesSent = 0;

	switch (m_nSocketType)
	{
	case TDSocket::SocketTypeTcp:
	{
		if (IsSocketValid())
		{
			if ((bytesToSend > 0) && (pBuf != NULL))
			{
				//---------------------------------------------------------
				// Check error condition and attempt to resend if call
				// was interrupted by a signal.
				//---------------------------------------------------------
				do
				{
					m_nBytesSent = SEND(m_socket, pBuf, bytesToSend, 0);
					TranslateSocketError();
				} while (GetSocketError() == TDSocket::SocketInterrupted);

			}
		}
		break;
	}
	case TDSocket::SocketTypeUdp:
	{
		if (IsSocketValid())
		{
			if ((bytesToSend > 0) && (pBuf != NULL))
			{
				//---------------------------------------------------------
				// Check error condition and attempt to resend if call
				// was interrupted by a signal.
				//---------------------------------------------------------
				//                    if (GetMulticast())
				//                    {
				//                        do
				//                        {
				//                            m_nBytesSent = SENDTO(m_socket, pBuf, bytesToSend, 0, (const sockaddr *)&m_stMulticastGroup, 
				//                                                  sizeof(m_stMulticastGroup));
				//                            TranslateSocketError();
				//                        } while (GetSocketError() == TDSocket::SocketInterrupted);
				//                    }
				//                    else
				{
					do
					{
						m_nBytesSent = SENDTO(m_socket, pBuf, bytesToSend, 0, (const sockaddr *)&m_stServerSockaddr, sizeof(m_stServerSockaddr));
						TranslateSocketError();
					} while (GetSocketError() == TDSocket::SocketInterrupted);
				}

			}
		}
		break;
	}
	default:
		break;
	}

	return m_nBytesSent;
}

//------------------------------------------------------------------------------
//
// Close() - Close socket and free up any memory allocated for the socket
//
//------------------------------------------------------------------------------
bool TDSocket::Close(void)
{
	bool bRetVal = false;

	//--------------------------------------------------------------------------
	// if socket handle is currently valid, close and then invalidate
	//--------------------------------------------------------------------------
	if (IsSocketValid())
	{
		if (CLOSE(m_socket) != TDSocket::SocketError)
		{
			m_socket = INVALID_SOCKET;
			bRetVal = true;
		}
	}

	m_bIsConnect = false;
	TranslateSocketError();

	return bRetVal;
}

//------------------------------------------------------------------------------
//
// Shtudown()
//
//------------------------------------------------------------------------------
bool TDSocket::Shutdown(CShutdownMode nShutdown)
{
	CSocketError nRetVal = SocketEunknown;

	nRetVal = (CSocketError)shutdown(m_socket, TDSocket::Sends);
	TranslateSocketError();

	return (nRetVal == TDSocket::SocketSuccess) ? true : false;
}

//------------------------------------------------------------------------------
//
// Flush()
//
//------------------------------------------------------------------------------
bool TDSocket::Flush()
{
	i32 nTcpNoDelay = 1;
	i32 nCurFlags = 0;
	char tmpbuf = 0;
	bool  bRetVal = false;

	//--------------------------------------------------------------------------
	// Get the current setting of the TCP_NODELAY flag.
	//--------------------------------------------------------------------------
	if (GETSOCKOPT(m_socket, IPPROTO_TCP, TCP_NODELAY, &nCurFlags, sizeof(i32)) == 0)
	{
		//----------------------------------------------------------------------
		// Set TCP NoDelay flag
		//----------------------------------------------------------------------
		if (SETSOCKOPT(m_socket, IPPROTO_TCP, TCP_NODELAY, &nTcpNoDelay, sizeof(i32)) == 0)
		{
			//------------------------------------------------------------------
			// Send empty byte stream to flush the TCP send buffer
			//------------------------------------------------------------------
			if (Send(&tmpbuf, 0) != TDSocket::SocketError)
			{
				bRetVal = true;
			}

			TranslateSocketError();
		}

		//----------------------------------------------------------------------
		// Reset the TCP_NODELAY flag to original state.
		//----------------------------------------------------------------------
		SETSOCKOPT(m_socket, IPPROTO_TCP, TCP_NODELAY, &nCurFlags, sizeof(i32));
	}

	return bRetVal;
}

//------------------------------------------------------------------------------
//
// Writev -
//
//------------------------------------------------------------------------------
i32 TDSocket::Writev(const struct iovec *pVector, size_t nCount)
{
	i32 nBytes = 0;
	i32 nBytesSent = 0;
	i32 i = 0;

	//--------------------------------------------------------------------------
	// Send each buffer as a separate send, windows does not support this
	// function call.                                                    
	//--------------------------------------------------------------------------
	for (i = 0; i < (i32)nCount; i++)
	{
		if ((nBytes = Send((char *)pVector[i].iov_base, pVector[i].iov_len)) == TDSocket::SocketError)
		{
			break;
		}

		nBytesSent += nBytes;
	}

	if (i > 0)
	{
		Flush();
	}

	return nBytesSent;
}

//------------------------------------------------------------------------------
//
// Send() - Send data on a valid socket via a vector of buffers.
//
//------------------------------------------------------------------------------
i32 TDSocket::Send(const struct iovec *sendVector, i32 nNumItems)
{
	SetSocketError(SocketSuccess);
	m_nBytesSent = 0;

	if ((m_nBytesSent = WRITEV(m_socket, sendVector, nNumItems)) == TDSocket::SocketError)
	{
		TranslateSocketError();
	}

	return m_nBytesSent;
}

//------------------------------------------------------------------------------
//
// SetReceiveTimeout()
//
//------------------------------------------------------------------------------
bool TDSocket::SetReceiveTimeout(i32 nRecvTimeoutSec, i32 nRecvTimeoutUsec)
{
	bool bRetVal = true;

	void* ptimeout = NULL;
#if WIN32
	int timeout = nRecvTimeoutSec * 1000 + nRecvTimeoutUsec / 1000;
	ptimeout = &timeout;
#else
	m_stRecvTimeout.tv_sec = nRecvTimeoutSec;
	m_stRecvTimeout.tv_usec = nRecvTimeoutUsec;
	ptimeout = &m_stRecvTimeout;
#endif
	//--------------------------------------------------------------------------
	// Sanity check to make sure the options are supported!					
	//--------------------------------------------------------------------------
	if (SETSOCKOPT(m_socket, SOL_SOCKET, SO_RCVTIMEO, ptimeout,
		sizeof(struct timeval)) == TDSocket::SocketError)
	{
		bRetVal = false;
		TranslateSocketError();
	}

	return bRetVal;
}

//------------------------------------------------------------------------------
//
// SetSendTimeout()
//
//------------------------------------------------------------------------------
bool TDSocket::SetSendTimeout(i32 nSendTimeoutSec, i32 nSendTimeoutUsec)
{
	bool bRetVal = true;

	memset(&m_stSendTimeout, 0, sizeof(struct timeval));
	m_stSendTimeout.tv_sec = nSendTimeoutSec;
	m_stSendTimeout.tv_usec = nSendTimeoutUsec;

	//--------------------------------------------------------------------------
	// Sanity check to make sure the options are supported!					
	//--------------------------------------------------------------------------
	if (SETSOCKOPT(m_socket, SOL_SOCKET, SO_SNDTIMEO, &m_stSendTimeout,
		sizeof(struct timeval)) == TDSocket::SocketError)
	{
		bRetVal = false;
		TranslateSocketError();
	}

	return bRetVal;
}

//------------------------------------------------------------------------------
//
// SetOptionReuseAddr()
//																			
//------------------------------------------------------------------------------
bool TDSocket::SetOptionReuseAddr()
{
	bool  bRetVal = false;
	i32 nReuse = IPTOS_LOWDELAY;

	if (SETSOCKOPT(m_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&nReuse, sizeof(i32)) == 0)
	{
		bRetVal = true;
	}

	TranslateSocketError();

	return bRetVal;
}

//------------------------------------------------------------------------------
//
// SetOptionLinger()
//																			
//------------------------------------------------------------------------------
bool TDSocket::SetOptionLinger(bool bEnable, u16 nTime)
{
	bool bRetVal = false;

	m_stLinger.l_onoff = (bEnable == true) ? 1 : 0;
	m_stLinger.l_linger = nTime;

	if (SETSOCKOPT(m_socket, SOL_SOCKET, SO_LINGER, &m_stLinger, sizeof(m_stLinger)) == 0)
	{
		bRetVal = true;
	}

	TranslateSocketError();

	return bRetVal;
}

//------------------------------------------------------------------------------
//
// Receive() - Attempts to receive a block of data on an established		
//			   connection.	Data is received in an internal buffer managed	
//			   by the class.  This buffer is only valid until the next call	
//			   to Receive(), a call to Close(), or until the object goes out
//			   of scope.													
//																			
//------------------------------------------------------------------------------
i32 TDSocket::Receive(i32 nMaxBytes)
{
	m_nBytesReceived = 0;

	//--------------------------------------------------------------------------
	// If the socket is invalid then return false.
	//--------------------------------------------------------------------------
	if (IsSocketValid() == false)
	{
		return m_nBytesReceived;
	}

	//--------------------------------------------------------------------------
	// Free existing buffer and allocate a new buffer the size of
	// nMaxBytes.
	//--------------------------------------------------------------------------
	if (nMaxBytes != m_nBufferSize)
	{
		m_nBufferSize = nMaxBytes;
		m_pBuffer.clear();
		m_pBuffer.resize(m_nBufferSize);
	}

	SetSocketError(SocketSuccess);

	switch (m_nSocketType)
	{
		//----------------------------------------------------------------------
		// If zero bytes are received, then return.  If SocketERROR is 
		// received, free buffer and return CSocket::SocketError (-1) to caller.	
		//----------------------------------------------------------------------
	case TDSocket::SocketTypeTcp:
	{
		do
		{
			m_nBytesReceived = RECV(m_socket, &m_pBuffer[m_nBytesReceived],
				nMaxBytes - m_nBytesReceived, m_nFlags);
			TranslateSocketError();
		} while (IsSocketValid() && (GetSocketError() == TDSocket::SocketInterrupted));

		break;
	}
	case TDSocket::SocketTypeUdp:
	{
		socklen_t srcSize;

		srcSize = sizeof(struct sockaddr_in);

		if (GetMulticast() == true)
		{
			do
			{
				m_nBytesReceived = RECVFROM(m_socket, &m_pBuffer[0], nMaxBytes, 0,
					&m_stMulticastGroup, &srcSize);
				TranslateSocketError();
			} while (IsSocketValid() && GetSocketError() == TDSocket::SocketInterrupted);
		}
		else
		{
			do
			{
				m_nBytesReceived = RECVFROM(m_socket, &m_pBuffer[0], nMaxBytes, 0,
					&m_stClientSockaddr, &srcSize);
				TranslateSocketError();
			} while (GetSocketError() == TDSocket::SocketInterrupted);
		}

		break;
	}
	default:
		break;
	}

	TranslateSocketError();
	return m_nBytesReceived;
}

//------------------------------------------------------------------------------
//
// SetNonblocking()
//
//------------------------------------------------------------------------------
bool TDSocket::SetNonblocking(void)
{
	i32  nCurFlags;

#if WIN32
	nCurFlags = 1;

	if (ioctlsocket(m_socket, FIONBIO, (ULONG *)&nCurFlags) != 0)
	{
		TranslateSocketError();
		return false;
	}
#else
	if ((nCurFlags = fcntl(m_socket, F_GETFL)) < 0)
	{
		TranslateSocketError();
		return false;
	}

	nCurFlags |= O_NONBLOCK;

	if (fcntl(m_socket, F_SETFL, nCurFlags) != 0)
	{
		TranslateSocketError();
		return false;
	}
#endif

	m_bIsBlocking = false;

	return true;
}

//------------------------------------------------------------------------------
//
// SetBlocking()
//
//------------------------------------------------------------------------------
bool TDSocket::SetBlocking(void)
{
	i32 nCurFlags;

#if WIN32
	nCurFlags = 0;

	if (ioctlsocket(m_socket, FIONBIO, (ULONG *)&nCurFlags) != 0)
	{
		return false;
	}
#else
	if ((nCurFlags = fcntl(m_socket, F_GETFL)) < 0)
	{
		TranslateSocketError();
		return false;
	}

	nCurFlags &= (~O_NONBLOCK);

	if (fcntl(m_socket, F_SETFL, nCurFlags) != 0)
	{
		TranslateSocketError();
		return false;
	}
#endif
	m_bIsBlocking = true;

	return true;
}

//------------------------------------------------------------------------------
//
// SendFile() - stands-in for system provided sendfile				
//
//------------------------------------------------------------------------------
i32 TDSocket::SendFile(i32 nOutFd, i32 nInFd, off_t *pOffset, i32 nCount)
{
	i32  nOutCount = TDSocket::SocketError;

	static char szData[SOCKET_SENDFILE_BLOCKSIZE];
	i32       nInCount = 0;

	if (LSEEK(nInFd, *pOffset, SEEK_SET) == -1)
	{
		return -1;
	}

	while (nOutCount < nCount)
	{
		nInCount = (nCount - nOutCount) < SOCKET_SENDFILE_BLOCKSIZE ? (nCount - nOutCount) : SOCKET_SENDFILE_BLOCKSIZE;

		if ((READ(nInFd, szData, nInCount)) != (i32)nInCount)
		{
			return -1;
		}

		if ((SEND(nOutFd, szData, nInCount, 0)) != (i32)nInCount)
		{
			return -1;
		}

		nOutCount += nInCount;
	}

	*pOffset += nOutCount;

	TranslateSocketError();

	return nOutCount;
}

//------------------------------------------------------------------------------
//
// TranslateSocketError() -					
//
//------------------------------------------------------------------------------
void TDSocket::TranslateSocketError(void)
{
#ifndef WIN32
	switch (errno)
	{
	case EXIT_SUCCESS:
		SetSocketError(TDSocket::SocketSuccess);
		break;
	case ENOTCONN:
		SetSocketError(TDSocket::SocketNotconnected);
		break;
	case ENOTSOCK:
	case EBADF:
	case EACCES:
	case EAFNOSUPPORT:
	case EMFILE:
	case ENFILE:
	case ENOBUFS:
	case ENOMEM:
	case EPROTONOSUPPORT:
		SetSocketError(TDSocket::SocketInvalidSocket);
		break;
	case ECONNREFUSED:
		SetSocketError(TDSocket::SocketConnectionRefused);
		break;
	case ETIMEDOUT:
		SetSocketError(TDSocket::SocketTimedout);
		break;
	case EINPROGRESS:
		SetSocketError(TDSocket::SocketEinprogress);
		break;
	case EWOULDBLOCK:
		//		case EAGAIN:
		SetSocketError(TDSocket::SocketEwouldblock);
		break;
	case EINTR:
		SetSocketError(TDSocket::SocketInterrupted);
		break;
	case ECONNABORTED:
		SetSocketError(TDSocket::SocketConnectionAborted);
		break;
	case EINVAL:
	case EPROTO:
		SetSocketError(TDSocket::SocketProtocolError);
		break;
	case EPERM:
		SetSocketError(TDSocket::SocketFirewallError);
		break;
	case EFAULT:
		SetSocketError(TDSocket::SocketInvalidSocketBuffer);
		break;
	case ECONNRESET:
		SetSocketError(TDSocket::SocketConnectionReset);
		break;
	case ENOPROTOOPT:
		SetSocketError(TDSocket::SocketConnectionReset);
		break;
	default:
		SetSocketError(TDSocket::SocketEunknown);
		break;
	}
#endif
#ifdef WIN32
	i32 nError = WSAGetLastError();
	switch (nError)
	{
	case EXIT_SUCCESS:
		SetSocketError(TDSocket::SocketSuccess);
		break;
	case WSAEBADF:
	case WSAENOTCONN:
		SetSocketError(TDSocket::SocketNotconnected);
		break;
	case WSAEINTR:
		SetSocketError(TDSocket::SocketInterrupted);
		break;
	case WSAEACCES:
	case WSAEAFNOSUPPORT:
	case WSAEINVAL:
	case WSAEMFILE:
	case WSAENOBUFS:
	case WSAEPROTONOSUPPORT:
		SetSocketError(TDSocket::SocketInvalidSocket);
		break;
	case WSAECONNREFUSED:
		SetSocketError(TDSocket::SocketConnectionRefused);
		break;
	case WSAETIMEDOUT:
		SetSocketError(TDSocket::SocketTimedout);
		break;
	case WSAEINPROGRESS:
		SetSocketError(TDSocket::SocketEinprogress);
		break;
	case WSAECONNABORTED:
		SetSocketError(TDSocket::SocketConnectionAborted);
		break;
	case WSAEWOULDBLOCK:
		SetSocketError(TDSocket::SocketEwouldblock);
		break;
	case WSAENOTSOCK:
		SetSocketError(TDSocket::SocketInvalidSocket);
		break;
	case WSAECONNRESET:
		SetSocketError(TDSocket::SocketConnectionReset);
		break;
	case WSANO_DATA:
		SetSocketError(TDSocket::SocketInvalidAddress);
		break;
	case WSAEADDRINUSE:
		SetSocketError(TDSocket::SocketAddressInUse);
		break;
	case WSAEFAULT:
		SetSocketError(TDSocket::SocketInvalidPointer);
		break;
	default:
		SetSocketError(TDSocket::SocketEunknown);
		break;
	}
#endif
}

//------------------------------------------------------------------------------
//
// Select()
//
//------------------------------------------------------------------------------
bool TDSocket::Select(i32 nTimeoutSec, i32 nTimeoutUSec)
{
	bool            bRetVal = false;
	struct timeval *pTimeout = NULL;
	struct timeval  timeout;
	i32           nNumDescriptors = -1;
	i32           nError = 0;

	FD_ZERO(&m_errorFds);
	FD_ZERO(&m_readFds);
	FD_ZERO(&m_writeFds);
	FD_SET(m_socket, &m_errorFds);
	FD_SET(m_socket, &m_readFds);
	FD_SET(m_socket, &m_writeFds);

	//---------------------------------------------------------------------
	// If timeout has been specified then set value, otherwise set timeout
	// to NULL which will block until a descriptor is ready for read/write
	// or an error has occurred.
	//---------------------------------------------------------------------
	if ((nTimeoutSec >= 0) || (nTimeoutUSec >= 0))
	{
		timeout.tv_sec = nTimeoutSec;
		timeout.tv_usec = nTimeoutUSec;
		pTimeout = &timeout;
	}

	nNumDescriptors = SELECT(m_socket + 1, &m_readFds, &m_writeFds, &m_errorFds, pTimeout);
	//    nNumDescriptors = SELECT(m_socket+1, &m_readFds, NULL, NULL, pTimeout);

	//----------------------------------------------------------------------
	// Handle timeout
	//----------------------------------------------------------------------
	if (nNumDescriptors == 0)
	{
		SetSocketError(TDSocket::SocketTimedout);
	}
	//----------------------------------------------------------------------
	// If a file descriptor (read/write) is set then check the
	// socket error (SO_ERROR) to see if there is a pending error.
	//----------------------------------------------------------------------
	else if ((FD_ISSET(m_socket, &m_readFds)) || (FD_ISSET(m_socket, &m_writeFds)))
	{
		i32 nLen = sizeof(nError);

		if (GETSOCKOPT(m_socket, SOL_SOCKET, SO_ERROR, &nError, &nLen) == 0)
		{
			errno = nError;

			if (nError == 0)
			{
				bRetVal = true;
			}
		}

		TranslateSocketError();
	}

	return bRetVal;
}



//connect 


//------------------------------------------------------------------------------
//
// ConnectTCP() -
//
//------------------------------------------------------------------------------
bool TDSocket::ConnectTCP(const char *pAddr, i16 nPort)
{
	bool           bRetVal = false;
	struct in_addr stIpAddress;

	//------------------------------------------------------------------
	// Preconnection setup that must be preformed					 
	//------------------------------------------------------------------
	memset(&m_stServerSockaddr, 0, sizeof(m_stServerSockaddr));
	m_stServerSockaddr.sin_family = AF_INET;

	hostent * pHE = NULL;
	if ((pHE = GETHOSTBYNAME(pAddr)) == NULL)
	{
#ifdef WIN32
		TranslateSocketError();
#else
		if (h_errno == HOST_NOT_FOUND)
		{
			SetSocketError(SocketInvalidAddress);
		}
#endif
		return bRetVal;
	}

	memcpy(&stIpAddress, pHE->h_addr_list[0], pHE->h_length);
	m_stServerSockaddr.sin_addr.s_addr = stIpAddress.s_addr;

	if ((i32)m_stServerSockaddr.sin_addr.s_addr == TDSocket::SocketError)
	{
		TranslateSocketError();
		return bRetVal;
	}

	m_stServerSockaddr.sin_port = htons(nPort);

	//------------------------------------------------------------------
	// Connect to address "xxx.xxx.xxx.xxx"	(IPv4) address only.  
	// 
	//------------------------------------------------------------------

	if (connect(m_socket, (struct sockaddr*)&m_stServerSockaddr, sizeof(m_stServerSockaddr)) ==
		TDSocket::SocketError)
	{
		//--------------------------------------------------------------
		// Get error value this might be a non-blocking socket so we 
		// must first check.
		//--------------------------------------------------------------
		TranslateSocketError();

		//--------------------------------------------------------------
		// If the socket is non-blocking and the current socket error
		// is SocketEinprogress or SocketEwouldblock then poll connection 
		// with select for designated timeout period.
		// Linux returns EINPROGRESS and Windows returns WSAEWOULDBLOCK.
		//--------------------------------------------------------------
		if ((IsNonblocking()) &&
			((GetSocketError() == TDSocket::SocketEwouldblock) ||
			(GetSocketError() == TDSocket::SocketEinprogress)))
		{
			bRetVal = Select(GetConnectTimeoutSec(), GetConnectTimeoutUSec());
		}
	}
	else
	{
		TranslateSocketError();
		bRetVal = true;
	}


	return bRetVal;
}


//------------------------------------------------------------------------------
//
// ConnectUDP() -
//
//------------------------------------------------------------------------------
bool TDSocket::ConnectUDP(const char *pAddr, i16 nPort)
{
	bool           bRetVal = false;
	struct in_addr stIpAddress;

	//------------------------------------------------------------------
	// Pre-connection setup that must be preformed					 
	//------------------------------------------------------------------
	memset(&m_stServerSockaddr, 0, sizeof(m_stServerSockaddr));
	m_stServerSockaddr.sin_family = AF_INET;

	hostent * pHE = NULL;
	if ((pHE = GETHOSTBYNAME(pAddr)) == NULL)
	{
#ifdef WIN32
		TranslateSocketError();
#else
		if (h_errno == HOST_NOT_FOUND)
		{
			SetSocketError(SocketInvalidAddress);
		}
#endif
		return bRetVal;
	}

	memcpy(&stIpAddress, pHE->h_addr_list[0], pHE->h_length);
	m_stServerSockaddr.sin_addr.s_addr = stIpAddress.s_addr;

	if ((i32)m_stServerSockaddr.sin_addr.s_addr == TDSocket::SocketError)
	{
		TranslateSocketError();
		return bRetVal;
	}

	m_stServerSockaddr.sin_port = htons(nPort);

	//------------------------------------------------------------------
	// Connect to address "xxx.xxx.xxx.xxx"	(IPv4) address only.  
	// 
	//------------------------------------------------------------------

	if (connect(m_socket, (struct sockaddr*)&m_stServerSockaddr, sizeof(m_stServerSockaddr)) != TDSocket::SocketError)
	{
		bRetVal = true;
	}

	TranslateSocketError();


	return bRetVal;
}


//------------------------------------------------------------------------------
//
// ConnectRAW() -
//
//------------------------------------------------------------------------------
bool TDSocket::ConnectRAW(const char *pAddr, i16 nPort)
{
	bool           bRetVal = false;
	struct in_addr stIpAddress;
	//------------------------------------------------------------------
	// Pre-connection setup that must be preformed					 
	//------------------------------------------------------------------
	memset(&m_stServerSockaddr, 0, sizeof(m_stServerSockaddr));
	m_stServerSockaddr.sin_family = AF_INET;

	hostent * pHE = NULL;
	if ((pHE = GETHOSTBYNAME(pAddr)) == NULL)
	{
#ifdef WIN32
		TranslateSocketError();
#else
		if (h_errno == HOST_NOT_FOUND)
		{
			SetSocketError(SocketInvalidAddress);
		}
#endif
		return bRetVal;
	}

	memcpy(&stIpAddress, pHE->h_addr_list[0], pHE->h_length);
	m_stServerSockaddr.sin_addr.s_addr = stIpAddress.s_addr;

	if ((i32)m_stServerSockaddr.sin_addr.s_addr == TDSocket::SocketError)
	{
		TranslateSocketError();
		return bRetVal;
	}

	m_stServerSockaddr.sin_port = htons(nPort);

	//------------------------------------------------------------------
	// Connect to address "xxx.xxx.xxx.xxx"	(IPv4) address only.  
	// 
	//------------------------------------------------------------------

	if (connect(m_socket, (struct sockaddr*)&m_stServerSockaddr, sizeof(m_stServerSockaddr)) != TDSocket::SocketError)
	{
		bRetVal = true;
	}

	TranslateSocketError();


	return bRetVal;
}



//------------------------------------------------------------------------------
//
// Open() - Create a connection to a specified address on a specified port
//
//------------------------------------------------------------------------------
bool TDSocket::Open(const char *pAddr, i16 nPort)
{
	bool bRetVal = false;

	if (IsSocketValid() == false)
	{
		SetSocketError(TDSocket::SocketInvalidSocket);
		return bRetVal;
	}

	if (pAddr == NULL)
	{
		SetSocketError(TDSocket::SocketInvalidAddress);
		return bRetVal;
	}

	if (nPort == 0)
	{
		SetSocketError(TDSocket::SocketInvalidPort);
		return bRetVal;
	}

	switch (m_nSocketType)
	{
	case TDSocket::SocketTypeTcp:
	{
		bRetVal = ConnectTCP(pAddr, nPort);
		break;
	}
	case TDSocket::SocketTypeUdp:
	{
		bRetVal = ConnectUDP(pAddr, nPort);
		break;
	}
	case TDSocket::SocketTypeRaw:
		break;
	default:
		break;
	}

	//--------------------------------------------------------------------------
	// If successful then create a local copy of the address and port							
	//--------------------------------------------------------------------------
	if (bRetVal)
	{
		socklen_t nSockLen = sizeof(struct sockaddr);

		memset(&m_stServerSockaddr, 0, nSockLen);
		getpeername(m_socket, (struct sockaddr *)&m_stServerSockaddr, &nSockLen);

		nSockLen = sizeof(struct sockaddr);
		memset(&m_stClientSockaddr, 0, nSockLen);
		getsockname(m_socket, (struct sockaddr *)&m_stClientSockaddr, &nSockLen);

		SetSocketError(SocketSuccess);

		m_bIsConnect = true;
	}

	return bRetVal;
}



bool TDSocket::BindMulticast(const char *pInterface, const char *pGroup, i16 nPort)
{
	bool		   bRetVal = false;
#ifdef WIN32
	ULONG          inAddr;
#else
	i32          nReuse;
	in_addr_t      inAddr;

	nReuse = IPTOS_LOWDELAY;
#endif

	//--------------------------------------------------------------------------
	// Set the following socket option SO_REUSEADDR.  This will allow the file
	// descriptor to be reused immediately after the socket is closed instead
	// of setting in a TIMED_WAIT state.
	//--------------------------------------------------------------------------
	memset(&m_stMulticastGroup, 0, sizeof(m_stMulticastGroup));
	m_stMulticastGroup.sin_family = AF_INET;
	m_stMulticastGroup.sin_port = htons(nPort);

	//--------------------------------------------------------------------------
	// If no IP Address (interface ethn) is supplied, or the loop back is 
	// specified then bind to any interface, else bind to specified interface.
	//--------------------------------------------------------------------------
	if ((pInterface == NULL) || (!strlen((const char *)pInterface)))
	{
		m_stMulticastGroup.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	else
	{
		if ((inAddr = inet_addr((const char *)pInterface)) != INADDR_NONE)
		{
			m_stMulticastGroup.sin_addr.s_addr = inAddr;
		}
	}

	//--------------------------------------------------------------------------
	// Bind to the specified port 
	//--------------------------------------------------------------------------
	if (::bind(m_socket, (struct sockaddr *)&m_stMulticastGroup, sizeof(m_stMulticastGroup)) == 0)
	{
		//----------------------------------------------------------------------
		// Join the multicast group
		//----------------------------------------------------------------------
		m_stMulticastRequest.imr_multiaddr.s_addr = inet_addr((const char *)pGroup);
		m_stMulticastRequest.imr_interface.s_addr = m_stMulticastGroup.sin_addr.s_addr;

		if (SETSOCKOPT(m_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP,
			(void *)&m_stMulticastRequest,
			sizeof(m_stMulticastRequest)) == TDSocket::SocketSuccess)
		{
			bRetVal = true;
		}

	}


	//--------------------------------------------------------------------------
	// If there was a socket error then close the socket to clean out the 
	// connection in the backlog.
	//--------------------------------------------------------------------------
	TranslateSocketError();

	if (bRetVal == false)
	{
		Close();
	}

	return bRetVal;
}



//------------------------------------------------------------------------------
//
// Listen() - 
//
//------------------------------------------------------------------------------
bool TDSocket::Listen(const char *pAddr, i16 nPort, i32 nConnectionBacklog)
{
	bool		   bRetVal = false;
#ifdef WIN32
	ULONG          inAddr;
#else
	i32          nReuse;
	in_addr_t      inAddr;

	nReuse = IPTOS_LOWDELAY;
#endif

	//--------------------------------------------------------------------------
	// Set the following socket option SO_REUSEADDR.  This will allow the file
	// descriptor to be reused immediately after the socket is closed instead
	// of setting in a TIMED_WAIT state.
	//--------------------------------------------------------------------------
#ifndef WIN32
	SETSOCKOPT(m_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&nReuse, sizeof(i32));
	SETSOCKOPT(m_socket, IPPROTO_TCP, IP_TOS, &nReuse, sizeof(i32));
#endif

	memset(&m_stServerSockaddr, 0, sizeof(m_stServerSockaddr));
	m_stServerSockaddr.sin_family = AF_INET;
	m_stServerSockaddr.sin_port = htons(nPort);

	//--------------------------------------------------------------------------
	// If no IP Address (interface ethn) is supplied, or the loop back is 
	// specified then bind to any interface, else bind to specified interface.
	//--------------------------------------------------------------------------
	if ((pAddr == NULL) || (!strlen((const char *)pAddr)))
	{
		m_stServerSockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	else
	{
		if ((inAddr = inet_addr((const char *)pAddr)) != INADDR_NONE)
		{
			m_stServerSockaddr.sin_addr.s_addr = inAddr;
		}
	}

	//--------------------------------------------------------------------------
	// Bind to the specified port 
	//--------------------------------------------------------------------------
	if (::bind(m_socket, (struct sockaddr *)&m_stServerSockaddr, sizeof(m_stServerSockaddr)) != TDSocket::SocketError)
	{
		if (m_nSocketType == TDSocket::SocketTypeTcp)
		{
			if (listen(m_socket, nConnectionBacklog) != TDSocket::SocketError)
			{
				bRetVal = true;
			}
		}
		else
		{
			bRetVal = true;
		}
	}

	//--------------------------------------------------------------------------
	// If there was a socket error then close the socket to clean out the 
	// connection in the backlog.
	//--------------------------------------------------------------------------
	TranslateSocketError();

	if (bRetVal == false)
	{
		Close();
	}

	return bRetVal;
}


//------------------------------------------------------------------------------
//
// Accept() - 
//
//------------------------------------------------------------------------------
TDSocket *TDSocket::Accept()
{
	u32         nSockLen;
	TDSocket *pClientSocket = NULL;
	SOCKET         socket = TDSocket::SocketError;

	if (m_nSocketType != TDSocket::SocketTypeTcp)
	{
		SetSocketError(TDSocket::SocketProtocolError);
		return pClientSocket;
	}

	pClientSocket = new TDSocket();

	//--------------------------------------------------------------------------
	// Wait for incoming connection.
	//--------------------------------------------------------------------------
	if (pClientSocket != NULL)
	{
		CSocketError socketErrno = SocketSuccess;

		nSockLen = sizeof(m_stClientSockaddr);

		do
		{
			errno = 0;
			socket = accept(m_socket, (struct sockaddr *)&m_stClientSockaddr, (socklen_t *)&nSockLen);

			if (socket != -1)
			{
				pClientSocket->SetSocketHandle(socket);
				pClientSocket->TranslateSocketError();
				socketErrno = pClientSocket->GetSocketError();
				socklen_t nSockLen = sizeof(struct sockaddr);

				//-------------------------------------------------------------
				// Store client and server IP and port information for this
				// connection.
				//-------------------------------------------------------------
				getpeername(m_socket, (struct sockaddr *)&pClientSocket->m_stClientSockaddr, &nSockLen);
				memcpy((void *)&pClientSocket->m_stClientSockaddr, (void *)&m_stClientSockaddr, nSockLen);

				memset(&pClientSocket->m_stServerSockaddr, 0, nSockLen);
				getsockname(m_socket, (struct sockaddr *)&pClientSocket->m_stServerSockaddr, &nSockLen);
			}
			else
			{
				TranslateSocketError();
				socketErrno = GetSocketError();
			}

		} while (socketErrno == TDSocket::SocketInterrupted);

		if (socketErrno != TDSocket::SocketSuccess)
		{
			delete pClientSocket;
			pClientSocket = NULL;
		}
	}

	return pClientSocket;
}

int TDSocket::Accept(SOCKET socket, struct sockaddr* cli_addr, socklen_t sin_size)
{
	return accept(socket, (struct sockaddr *)&cli_addr, (socklen_t *) &sin_size);
}

int TDSocket::SetSendBuffSize(int size)
{
	setsockopt(m_socket, SOL_SOCKET, SO_SNDBUF, (const char*)&size, sizeof(int));
	return 0;
}

int TDSocket::SetRecvBuffSize(int size)
{
	setsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, (const char*)&size, sizeof(int));
	return 0;
}

std::string TDSocket::getClientIp()
{
	return inet_ntoa(m_stClientSockaddr.sin_addr);
}

u16 TDSocket::getClientPort()
{
	return m_stClientSockaddr.sin_port;
}
