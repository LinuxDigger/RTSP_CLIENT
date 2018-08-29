/*
 * DigestAuthentication.h
 *
 *  Created on: Jun 8, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_DIGESTAUTHENTICATION_H_
#define INCLUDE_DIGESTAUTHENTICATION_H_

#include "Boolean.h"

// A class used for digest authentication.
// The "realm", and "nonce" fields are supplied by the server
// (in a "401 Unauthorized" response).
// The "username" and "password" fields are supplied by the client.
class Authenticator {
public:
  Authenticator();
  Authenticator(char const* username, char const* password, Boolean passwordIsMD5 = False);
      // If "passwordIsMD5" is True, then "password" is actually the value computed
      // by md5(<username>:<realm>:<actual-password>)
  Authenticator(const Authenticator& orig);
  Authenticator& operator=(const Authenticator& rightSide);
  Boolean operator<(const Authenticator* rightSide);
  virtual ~Authenticator();

  void reset();
  void setRealmAndNonce(char const* realm, char const* nonce);
  void setRealmAndRandomNonce(char const* realm);
      // as above, except that the nonce is created randomly.
      // (This is used by servers.)
  void setUsernameAndPassword(char const* username, char const* password, Boolean passwordIsMD5 = False);
      // If "passwordIsMD5" is True, then "password" is actually the value computed
      // by md5(<username>:<realm>:<actual-password>)

  char const* realm() const { return fRealm; }
  char const* nonce() const { return fNonce; }
  char const* username() const { return fUsername; }
  char const* password() const { return fPassword; }

  char const* computeDigestResponse(char const* cmd, char const* url) const;
      // The returned string from this function must later be freed by calling:
  void reclaimDigestResponse(char const* responseStr) const;

private:
  void resetRealmAndNonce();
  void resetUsernameAndPassword();
  void assignRealmAndNonce(char const* realm, char const* nonce);
  void assignUsernameAndPassword(char const* username, char const* password, Boolean passwordIsMD5);
  void assign(char const* realm, char const* nonce,
	      char const* username, char const* password, Boolean passwordIsMD5);

private:
  char* fRealm; char* fNonce;
  char* fUsername; char* fPassword;
  Boolean fPasswordIsMD5;
};



#endif /* INCLUDE_DIGESTAUTHENTICATION_H_ */
