#include "stdafx.h"

#include "LesserODBCConnector.h"

bool LesserODBCConnector::Prepare()
{
    rc = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);
    if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
        HandleDiagnosticRecord(env, SQL_HANDLE_ENV, rc);
        return false;
    }
    else {
        rc = SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
        if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
            HandleDiagnosticRecord(env, SQL_HANDLE_ENV, rc);
            return false;
        }
        else {
            rc = SQLAllocHandle(SQL_HANDLE_DBC, env, &connection);
            if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
                HandleDiagnosticRecord(connection, SQL_HANDLE_DBC, rc);
                return false;
            }
        }
    }
    return true;
}

bool LesserODBCConnector::Connect(string arg)
{
    unique_ptr<wchar_t> cs = move(wcharStr.Value(arg).ToWChar());
    rc = SQLDriverConnect(
        connection,
        NULL,
        cs.get(),
        SQL_NTS,
        connectionString,
        1024,
        &bufSize,
        SQL_DRIVER_NOPROMPT);

    if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO) {
        rc = SQLAllocHandle(SQL_HANDLE_STMT, connection, &statement);
        if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO) {
            return true;
        }
        else {
            HandleDiagnosticRecord(connection, SQL_HANDLE_STMT, rc);
            return false;
        }
    }
    else {
        HandleDiagnosticRecord(connection, SQL_HANDLE_DBC, rc);
        return false;
    }
}

string LesserODBCConnector::FetchScalar(string arg)
{
    unique_ptr<wchar_t> query = move(wcharStr.Value(arg).ToWChar());
    rc = SQLExecDirect(statement, query.get(), SQL_NTS);

    if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO) {
        SQLWCHAR columnName[128];
        SQLSMALLINT columnNameSize;
        SQLSMALLINT columnType;
        SQLULEN columnSize;
        SQLSMALLINT scale;
        SQLSMALLINT nullable;
        rc = SQLDescribeCol(
            statement,
            (SQLUSMALLINT)1,
            columnName,
            (SQLSMALLINT)(sizeof(columnName) / 2),
            &columnNameSize,
            &columnType,
            &columnSize,
            &scale,
            &nullable);
        if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO) {
            wchar_t * value = new wchar_t[columnSize];
            SQLLEN nullIndicator;
            rc = SQLBindCol(
                statement,
                1,
                SQL_C_TCHAR,
                (SQLPOINTER)value,
                (columnSize + 1) * sizeof(wchar_t),
                &nullIndicator);
            if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO) {
                rc = SQLFetch(statement);
                if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO) {
                    return wcharStr.Value(value).ToString();
                }
                else {
                    return string();
                }
            }
            else {
                HandleDiagnosticRecord(statement, SQL_HANDLE_STMT, rc);
                return string();
            }
        }
        else {
            return string();
        }
    }
    else {
        HandleDiagnosticRecord(statement, SQL_HANDLE_STMT, rc);
        return string();
    }
}

bool LesserODBCConnector::SQLNonQuery(string arg)
{
    unique_ptr<wchar_t> query = move(wcharStr.Value(arg).ToWChar());
    rc = SQLExecDirect(statement, query.get(), SQL_NTS);
    if (rc != SQL_SUCCESS && rc != SQL_NEED_DATA && rc != SQL_SUCCESS_WITH_INFO) {
        HandleDiagnosticRecord(statement, SQL_HANDLE_STMT, rc);
        return false;
    }
    else {
        return true;
    }
}

void LesserODBCConnector::HandleDiagnosticRecord(SQLHANDLE handle, SQLSMALLINT handleType, SQLRETURN retCode)
{
    if (retCode == SQL_INVALID_HANDLE) {
        locale::global(locale("C"));
        cout << "Invalid handle" << "\n";
        locale::global(locale(""));
        return;
    }
    SQLSMALLINT iRec = 0;
    WCHAR szSQLState[SQL_SQLSTATE_SIZE + 1];
    SQLINTEGER error;
    WCHAR szErrorMessage[1000];
    while (SQLGetDiagRec(
        handleType,
        handle,
        ++iRec,
        szSQLState,
        &error,
        szErrorMessage,
        (SQLSMALLINT)(sizeof(szErrorMessage) / sizeof(WCHAR)),
        (SQLSMALLINT *)NULL) == SQL_SUCCESS
        ) {
        WCharString msgCnv;
        locale::global(locale("C"));
        cout << msgCnv.Value("Status = ").Append(szSQLState).Append(" ").Append("\n").Append("Message = ").Append(szErrorMessage).ToString() << "\n" << "\n";
        locale::global(locale(""));
    }
}

LesserODBCConnector::LesserODBCConnector()
{
}

LesserODBCConnector::~LesserODBCConnector()
{
}