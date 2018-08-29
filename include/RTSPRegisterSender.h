/*
 * RTSPRegisterSender.h
 *
 *  Created on: Aug 9, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_RTSPREGISTERSENDER_H_
#define INCLUDE_RTSPREGISTERSENDER_H_

#include "RTSPClient.h"

class RTSPRegisterOrDeregisterSender: public RTSPClient {
public:
  virtual ~RTSPRegisterOrDeregisterSender();
protected: // we're a virtual base class
  RTSPRegisterOrDeregisterSender(UsageEnvironment& env,
				 char const* remoteClientNameOrAddress, portNumBits remoteClientPortNum,
				 Authenticator* authenticator,
				 int verbosityLevel, char const* applicationName);

public: // Some compilers complain if this is "protected:"
  // A subclass of "RTSPClient::RequestRecord", specific to our "REGISTER" and "DEREGISTER" commands:
  class RequestRecord_REGISTER_or_DEREGISTER: public RTSPClient::RequestRecord {
  public:
    RequestRecord_REGISTER_or_DEREGISTER(unsigned cseq, char const* cmdName, RTSPClient::responseHandler* rtspResponseHandler, char const* rtspURLToRegisterOrDeregister, char const* proxyURLSuffix);
    virtual ~RequestRecord_REGISTER_or_DEREGISTER();

    char const* proxyURLSuffix() const { return fProxyURLSuffix; }

  protected:
    char* fRTSPURLToRegisterOrDeregister;
    char* fProxyURLSuffix;
  };

protected:
  portNumBits fRemoteClientPortNum;
};

//////////

class RTSPRegisterSender: public RTSPRegisterOrDeregisterSender {
public:
  static RTSPRegisterSender*
  createNew(UsageEnvironment& env,
	    char const* remoteClientNameOrAddress, portNumBits remoteClientPortNum, char const* rtspURLToRegister,
	    RTSPClient::responseHandler* rtspResponseHandler, Authenticator* authenticator = NULL,
	    Boolean requestStreamingViaTCP = False, char const* proxyURLSuffix = NULL, Boolean reuseConnection = False,
	    int verbosityLevel = 0, char const* applicationName = NULL);

  void grabConnection(int& sock, struct sockaddr_in& remoteAddress); // so that the socket doesn't get closed when we're deleted

protected:
  RTSPRegisterSender(UsageEnvironment& env,
		     char const* remoteClientNameOrAddress, portNumBits remoteClientPortNum, char const* rtspURLToRegister,
		     RTSPClient::responseHandler* rtspResponseHandler, Authenticator* authenticator,
		     Boolean requestStreamingViaTCP, char const* proxyURLSuffix, Boolean reuseConnection,
		     int verbosityLevel, char const* applicationName);
    // called only by "createNew()"
  virtual ~RTSPRegisterSender();

  // Redefined virtual functions:
  virtual Boolean setRequestFields(RequestRecord* request,
                                   char*& cmdURL, Boolean& cmdURLWasAllocated,
                                   char const*& protocolStr,
                                   char*& extraHeaders, Boolean& extraHeadersWereAllocated);

public: // Some compilers complain if this is "protected:"
  // A subclass of "RequestRecord_REGISTER_or_DEREGISTER", specific to our "REGISTER" command:
  class RequestRecord_REGISTER: public RTSPRegisterOrDeregisterSender::RequestRecord_REGISTER_or_DEREGISTER {
  public:
    RequestRecord_REGISTER(unsigned cseq, RTSPClient::responseHandler* rtspResponseHandler, char const* rtspURLToRegister,
			   Boolean reuseConnection, Boolean requestStreamingViaTCP, char const* proxyURLSuffix);
    virtual ~RequestRecord_REGISTER();

    char const* rtspURLToRegister() const { return fRTSPURLToRegisterOrDeregister; }
    Boolean reuseConnection() const { return fReuseConnection; }
    Boolean requestStreamingViaTCP() const { return fRequestStreamingViaTCP; }

  private:
    Boolean fReuseConnection, fRequestStreamingViaTCP;
  };
};

//////////

class RTSPDeregisterSender: public RTSPRegisterOrDeregisterSender {
public:
  static RTSPDeregisterSender*
  createNew(UsageEnvironment& env,
	    char const* remoteClientNameOrAddress, portNumBits remoteClientPortNum, char const* rtspURLToDeregister,
	    RTSPClient::responseHandler* rtspResponseHandler, Authenticator* authenticator = NULL,
	    char const* proxyURLSuffix = NULL,
	    int verbosityLevel = 0, char const* applicationName = NULL);

protected:
  RTSPDeregisterSender(UsageEnvironment& env,
		       char const* remoteClientNameOrAddress, portNumBits remoteClientPortNum, char const* rtspURLToDeregister,
		     RTSPClient::responseHandler* rtspResponseHandler, Authenticator* authenticator,
		     char const* proxyURLSuffix,
		     int verbosityLevel, char const* applicationName);
    // called only by "createNew()"
  virtual ~RTSPDeregisterSender();

  // Redefined virtual functions:
  virtual Boolean setRequestFields(RequestRecord* request,
                                   char*& cmdURL, Boolean& cmdURLWasAllocated,
                                   char const*& protocolStr,
                                   char*& extraHeaders, Boolean& extraHeadersWereAllocated);

public: // Some compilers complain if this is "protected:"
  // A subclass of "RequestRecord_REGISTER_or_DEREGISTER", specific to our "DEREGISTER" command:
  class RequestRecord_DEREGISTER: public RTSPRegisterOrDeregisterSender::RequestRecord_REGISTER_or_DEREGISTER {
  public:
    RequestRecord_DEREGISTER(unsigned cseq, RTSPClient::responseHandler* rtspResponseHandler, char const* rtspURLToDeregister, char const* proxyURLSuffix);
    virtual ~RequestRecord_DEREGISTER();

    char const* rtspURLToDeregister() const { return fRTSPURLToRegisterOrDeregister; }
  };
};

#endif /* INCLUDE_RTSPREGISTERSENDER_H_ */
