#pragma once

enum class HTTPMethod {
  mUnknown,
  mGET,
  mPOST,
  mPUT,
  mDELETE
};

enum class HTTPStatusCode {
  mOk = 200,
  mBadRequest = 400,
  mUnauthorized = 401,
  mForbidden = 403,
  mNotFound = 404,
  mInternalServerError = 500,
  mBadGateway = 502,
  mServiceUnavailable = 503,
  mGatewayTimeout = 504
};