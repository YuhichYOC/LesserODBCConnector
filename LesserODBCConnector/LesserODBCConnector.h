#pragma once

#include "stdafx.h"

#include "WCharString.h"

class LesserODBCConnector
{
private:

    SQLHANDLE env;

    SQLHANDLE connection;

    SQLWCHAR connectionString[1024];

    SQLSMALLINT bufSize;

    SQLHANDLE statement;

    SQLRETURN rc;

    WCharString wcharStr;

public:

    bool Prepare();

    bool Connect(string arg);

    string FetchScalar(string arg);

    bool SQLNonQuery(string arg);

    void HandleDiagnosticRecord(SQLHANDLE handle, SQLSMALLINT handleType, SQLRETURN retCode);

    LesserODBCConnector();

    ~LesserODBCConnector();
};
