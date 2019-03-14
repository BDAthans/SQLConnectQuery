#include <Windows.h>
#include <iostream>
#include <string>
#include <conio.h>
#include <cstring>
#include <iomanip>

#include <odbcinst.h>
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <sqlucode.h>
#include <msdasql.h>
#include <msdadc.h>

using namespace std;

void pause();
void showSQLErrorMsg(unsigned int handleType, const SQLHANDLE& handle);

// Test strings to pass into connection string, change if instance and database are different
string dbString = ".\\OMSQL";
string dbName = "OMSQLDB";

int main() {

	SQLHANDLE SQLEnvHandle = NULL;
	SQLHANDLE SQLConnectionHandle = NULL;
	SQLHANDLE SQLStatementHandle = NULL;
	SQLRETURN retCode = 0;

	do {
		if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &SQLEnvHandle) == SQL_ERROR) {
			break;
		}
		if (SQLSetEnvAttr(SQLEnvHandle, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0) == SQL_ERROR) {
			break;
		} 
		if (SQLAllocHandle(SQL_HANDLE_DBC, SQLEnvHandle, &SQLConnectionHandle) == SQL_ERROR) {
			break;
		}
		if (SQLSetConnectAttr(SQLConnectionHandle, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0) == SQL_ERROR) {
			break;
		}

		// ConnectionString from VS: (SQLCHAR*)"Data Source = .\OMSQL; Initial Catalog = OMSQLDB; Persist Security Info = True; User ID = sa; Password = ********;"
		// ConnectionString from Tutorial: (SQLCHAR*)"DRIVER={SQL Server}; SERVER=127.0.0.1\\OMSQL, 1433; DATABASE=OMSQLDB; UID=OM_USER; PWD=OMSQL@2004;"
		// From ConnectionString.com: (SQLCHAR*)"Driver={SQL Server Native Client 11.0};Server=myServerAddress; Database=myDataBase;Uid=myUsername;Pwd=myPassword;"
		// Previously tried ConnectionString: "DRIVER={SQL Server}; SERVER = .\\OMSQL, 1433; Initial Catalog = OMSQLDB; Persist Security Info = True; User ID = OM_USER; Password = OMSQL@2004;"

		SQLCHAR retConString[1024];
		string ConnectionString = "DRIVER={SQL Server Native Client 11.0}; SERVER=" + dbString + "; DATABASE=" + dbName + ";Uid=OM_USER;Pwd=OMSQL@2004;";
		switch (SQLDriverConnect(SQLConnectionHandle, NULL, (SQLCHAR*)ConnectionString.c_str(), SQL_NTS, retConString, 1024, NULL, SQL_DRIVER_NOPROMPT)) {
		case SQL_SUCCESS:
			cout << "Success - SQL_SUCESS" << string(2, '\n');;
			break;
		case SQL_SUCCESS_WITH_INFO:
			cout << "Success - SQL_SUCCESS_WITH_INFO" << string(2, '\n');;
			break;
		case SQL_NO_DATA_FOUND:
			cout << "Error - SQL_NO_DATA_FOUND" << string(2, '\n');;
			showSQLErrorMsg(SQL_HANDLE_DBC, SQLConnectionHandle);
			retCode = -1;
			break;
		case SQL_INVALID_HANDLE:
			cout << "Error - SQL_INVALID_HANDLE" << string(2, '\n');;
			showSQLErrorMsg(SQL_HANDLE_DBC, SQLConnectionHandle);
			retCode = -1;
			break;
		case SQL_ERROR:
			cout << "Error - SQL_ERROR" << string(2, '\n');
			showSQLErrorMsg(SQL_HANDLE_DBC, SQLConnectionHandle);
			retCode = -1;
			break;
		}

		if (retCode == -1)
			break;

		if (SQLAllocHandle(SQL_HANDLE_STMT, SQLConnectionHandle, &SQLStatementHandle) == SQL_ERROR) {
			cout << left << setw(10) << "Error allocating SQL Statement Handle" << endl;
			break;
		}

		// Execute Query to get information or do procedure
		char SQLQuery[] = "SELECT patient_no, first_name, last_name FROM patient ORDER BY patient_no ASC";

		// Allocating Handle for SQLStatementHandle and using char Query[]
		if (SQLExecDirect(SQLStatementHandle, (SQLCHAR*)SQLQuery, SQL_NTS) == SQL_ERROR) {
			cout << left << setw(10) << "Error allocating SQL Statement Handle" << endl;
			showSQLErrorMsg(SQL_HANDLE_STMT, SQLStatementHandle);
			break;
		}
		else 
		{	
			int patientID;
			char first_name[256];
			char last_name[256];

			while (SQLFetch(SQLStatementHandle) == SQL_SUCCESS) {
				SQLGetData(SQLStatementHandle, 1, SQL_C_DEFAULT, &patientID, sizeof(patientID)+1, NULL);
				SQLGetData(SQLStatementHandle, 2, SQL_C_DEFAULT, &first_name, size(first_name), NULL);
				SQLGetData(SQLStatementHandle, 3, SQL_C_DEFAULT, &last_name, size(last_name), NULL);
				cout << right << setw(10) << patientID << " " << first_name << " " << last_name << endl;
			}
		}

	} while (FALSE);


	// Free SQL Handles that were previously in use
	SQLFreeHandle(SQL_HANDLE_STMT, SQLStatementHandle);
	SQLDisconnect(SQLConnectionHandle);
	SQLFreeHandle(SQL_HANDLE_DBC, SQLConnectionHandle);
	SQLFreeHandle(SQL_HANDLE_ENV, SQLEnvHandle);

	pause();
	return 0;
}

void showSQLErrorMsg(unsigned int handleType, const SQLHANDLE& handle) {
	//Function shows error message and state derived from preliminary function return message
	SQLCHAR SQLState[1024];
	SQLCHAR message[1024];

	if (SQL_SUCCESS == SQLGetDiagRec(handleType, handle, 1, SQLState, NULL, message, 1024, NULL)) {
		cout << left << setw(10) << "ODBC Driver Message: " << message << string(2, '\n');;
		cout << left << setw(10) << "ODBC SQL State: " << SQLState << string(2, '\n');;
	}
}

void pause() {
	char a;
	cout << string(2, '\n') << "Press any Key to Continue... ";
	a = _getch();
}

/*int main()
{
#define SQL_QUERY_SIZE 1000

	SQLHANDLE sqlConnectionH;
	SQLHANDLE sqlStatementH;
	SQLHANDLE sqlEnvH;
	SQLWCHAR ConnectionReturn[SQL_QUERY_SIZE];

	sqlConnectionH = NULL;
	sqlStatementH = NULL;

	if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &sqlEnvH) == SQL_ERROR){
		fwprintf(stderr, L"Unable to allocate an environment handle\n");
		exit(-1);
	}
	if (SQLSetEnvAttr(sqlEnvH, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0) == SQL_ERROR) {
		fwprintf(stderr, L"Unable to allocate an environment handle\n");
		exit(-2);
	}
	if (SQLAllocHandle(SQL_HANDLE_DBC, sqlEnvH, &sqlConnectionH)) {
		fwprintf(stderr, L"Unable to allocate an environment handle\n");
		exit(-3);
	}



	//string connectionString1 = "DRIVER={SQL Server};SERVER=" + dbString + ", 1433;DATABASE=" + dbName +  ";UID=OM_USER;PWD=OMSQL@2004";
	string connectionString = "Data Source=.\\OMSQL;Initial Catalog=OMSQLDB;Persist Security Info=True;User ID=sa;PWD=OMateSQL@2007";
	//Data Connection string from SSMS:
	//                         Data Source=.\OMSQL;Initial Catalog=OMSQLDB;Persist Security Info=True;User ID=sa;Password=OMateSQL@2007

	//Tried below connection strings or formats:

	//L"DRIVER={SQL Server};SERVER=localhost, 1433;DATABASE=master;UID=username;PWD=password;",
	//L"DRIVER={SQL Server};SERVER=ServerAddress, 1433;DATABASE=DataBaseName;UID=DataBaseUserName;PWD=PassWord;",
	//L"DRIVER={SQL Server};SERVER=.\OMSQL, 1433;DATABASE=OMSQLDB;UID=OM_USER;PWD=OMSQL@2004;"

	//DEBUG INFO
	cout << left << setw(10) << "Connection String: " << connectionString.c_str() << string(2, '\n');
	cout << left << setw(10) << "Attempting to connect to database server..." << string(1, '\n');

	switch (SQLDriverConnectW(sqlConnectionH, NULL, (SQLWCHAR*)connectionString.c_str(), SQL_NTS, ConnectionReturn, 1024, NULL, SQL_DRIVER_NOPROMPT)) {
	case SQL_SUCCESS:
		cout << "Successfully connected to SQL Server - 0" << endl;
		break;
	case SQL_SUCCESS_WITH_INFO:
		cout << "Successfully connected to SQL Server - 1" << endl;
		break;
	case SQL_INVALID_HANDLE:
		cout << "Could not connect to SQL Server - Er2" << endl;
		break;
	case SQL_ERROR:
		cout << "Could not connect to SQL Server - Er3" << endl;
		break;
	}


	pause();
	return 0;
}


/*
Using the ODBC APIs SQLAllocHandle, SQLSetConnectAttr, and SQLDriverConnect, you should be able to initialize and establish a connection to your database.

SQL Server 2005 Connection String:
Driver={SQL Native Client};Server=myServerAddress;Database=myDataBase;
Uid=myUsername;Pwd=myPassword;







SQLAllocHandle Function - https://docs.microsoft.com/en-us/sql/odbc/reference/syntax/sqlallochandle-function?view=sql-server-2017

Summary
SQLAllocHandle allocates an environment, connection, statement, or descriptor handle.

SYNTAX
SQLRETURN SQLAllocHandle(
	SQLSMALLINT   HandleType,
	SQLHANDLE     InputHandle,
	SQLHANDLE *   OutputHandlePtr);


Arguments

HandleType
[Input] The type of handle to be allocated by SQLAllocHandle. Must be one of the following values:

	SQL_HANDLE_DBC

	SQL_HANDLE_DBC_INFO_TOKEN

	SQL_HANDLE_DESC

	SQL_HANDLE_ENV

	SQL_HANDLE_STMT

SQL_HANDLE_DBC_INFO_TOKEN handle is used only by the Driver Manager and driver. Applications should not use this handle type.
For more information about SQL_HANDLE_DBC_INFO_TOKEN, see Developing Connection-Pool Awareness in an ODBC Driver.

InputHandle
[Input] The input handle in whose context the new handle is to be allocated.
If HandleType is SQL_HANDLE_ENV, this is SQL_NULL_HANDLE. If HandleType is SQL_HANDLE_DBC, this must be an environment handle, and if it is SQL_HANDLE_STMT or SQL_HANDLE_DESC, it must be a connection handle.

OutputHandlePtr
[Output] Pointer to a buffer in which to return the handle to the newly allocated data structure.
Returns

SQL_SUCCESS, SQL_SUCCESS_WITH_INFO, SQL_INVALID_HANDLE, or SQL_ERROR.

When allocating a handle other than an environment handle, if SQLAllocHandle returns SQL_ERROR, it sets OutputHandlePtr to SQL_NULL_HDBC, SQL_NULL_HSTMT, or SQL_NULL_HDESC,
depending on the value of HandleType, unless the output argument is a null pointer.
The application can then obtain additional information from the diagnostic data structure associated with the handle in the InputHandle argument.







SQLSetConnectAttr Function - https://docs.microsoft.com/en-us/sql/odbc/reference/syntax/sqlsetconnectattr-function?view=sql-server-2017

Summary
SQLSetConnectAttr sets attributes that govern aspects of connections.

SYNTAX
SQLRETURN SQLSetConnectAttr(
	 SQLHDBC       ConnectionHandle,
	 SQLINTEGER    Attribute,
	 SQLPOINTER    ValuePtr,
	 SQLINTEGER    StringLength);


Arguments

ConnectionHandle
[Input] Connection handle.

Attribute
[Input] Attribute to set, listed in "Comments."

ValuePtr
[Input] Pointer to the value to be associated with Attribute. Depending on the value of Attribute, ValuePtr will be an unsigned integer value or will point to a null-terminated character string. Note that the integral type of the Attribute argument may not be fixed length, see the Comments section for detail.

StringLength
[Input] If Attribute is an ODBC-defined attribute and ValuePtr points to a character string or a binary buffer, this argument should be the length of *ValuePtr. For character string data, this argument should contain the number of bytes in the string.

If Attribute is an ODBC-defined attribute and ValuePtr is an integer, StringLength is ignored.

If Attribute is a driver-defined attribute, the application indicates the nature of the attribute to the Driver Manager by setting the StringLength argument. StringLength can have the following values:

	If ValuePtr is a pointer to a character string, then StringLength is the length of the string or SQL_NTS.

	If ValuePtr is a pointer to a binary buffer, then the application places the result of the SQL_LEN_BINARY_ATTR(length) macro in StringLength. This places a negative value in StringLength.

	If ValuePtr is a pointer to a value other than a character string or a binary string, then StringLength should have the value SQL_IS_POINTER.

	If ValuePtr contains a fixed-length value, then StringLength is either SQL_IS_INTEGER or SQL_IS_UINTEGER, as appropriate.

Returns

SQL_SUCCESS, SQL_SUCCESS_WITH_INFO, SQL_ERROR, SQL_INVALID_HANDLE, or SQL_STILL_EXECUTING.








SQLDriverConnect Function - https://docs.microsoft.com/en-us/sql/odbc/reference/syntax/sqldriverconnect-function?view=sql-server-2017

Summary
SQLDriverConnect is an alternative to SQLConnect.
It supports data sources that require more connection information than the three arguments in SQLConnect,
dialog boxes to prompt the user for all connection information, and data sources that are not defined in the system information.

SQLDriverConnect provides the following connection attributes:

	Establish a connection using a connection string that contains the data source name, one or more user IDs, one or more passwords, and other information required by the data source.

	Establish a connection using a partial connection string or no additional information; in this case, the Driver Manager and the driver can each prompt the user for connection information.

	Establish a connection to a data source that is not defined in the system information. If the application supplies a partial connection string, the driver can prompt the user for connection information.

	Establish a connection to a data source using a connection string constructed from the information in a .dsn file.

After a connection is established, SQLDriverConnect returns the completed connection string. The application can use this string for subsequent connection requests.

SYNTAX
SQLRETURN SQLDriverConnect(
	 SQLHDBC         ConnectionHandle,
	 SQLHWND         WindowHandle,
	 SQLCHAR *       InConnectionString,
	 SQLSMALLINT     StringLength1,
	 SQLCHAR *       OutConnectionString,
	 SQLSMALLINT     BufferLength,
	 SQLSMALLINT *   StringLength2Ptr,
	 SQLUSMALLINT    DriverCompletion);








SQLExecute Function - https://docs.microsoft.com/en-us/sql/odbc/reference/syntax/sqlexecute-function?view=sql-server-2017

Summary
SQLExecute executes a prepared statement, using the current values of the parameter marker variables if any parameter markers exist in the statement.
Syntax


SQLRETURN SQLExecute(
	 SQLHSTMT     StatementHandle);

Arguments

StatementHandle
[Input] Statement handle.

Returns
SQL_SUCCESS, SQL_SUCCESS_WITH_INFO, SQL_NEED_DATA, SQL_STILL_EXECUTING, SQL_ERROR, SQL_NO_DATA, SQL_INVALID_HANDLE, or SQL_PARAM_DATA_AVAILABLE.








SQLSetEnvAttr Function - https://docs.microsoft.com/en-us/sql/odbc/reference/syntax/sqlsetenvattr-function?view=sql-server-2017

Summary
SQLSetEnvAttr sets attributes that govern aspects of environments.
Syntax


SQLRETURN SQLSetEnvAttr(
	 SQLHENV      EnvironmentHandle,
	 SQLINTEGER   Attribute,
	 SQLPOINTER   ValuePtr,
	 SQLINTEGER   StringLength);

Arguments

EnvironmentHandle
[Input] Environment handle.

Attribute
[Input] Attribute to set, listed in "Comments."

ValuePtr
[Input] Pointer to the value to be associated with Attribute. Depending on the value of Attribute, ValuePtr will be a 32-bit integer value or point to a null-terminated character string.

StringLength
[Input] If ValuePtr points to a character string or a binary buffer, this argument should be the length of *ValuePtr. For character string data, this argument should contain the number of bytes in the string.

If ValuePtr is an integer, StringLength is ignored.
Returns

SQL_SUCCESS, SQL_SUCCESS_WITH_INFO, SQL_ERROR, or SQL_INVALID_HANDLE.
*/