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
string DataSource = ".\\OMSQL";
string DatabaseName = "OMSQLDB";

int main() {

	SQLHANDLE SQLEnvHandle = NULL;
	SQLHANDLE SQLConnectionHandle = NULL;
	SQLHANDLE SQLStatementHandle = NULL;
	SQLRETURN retCode = 0;

		if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &SQLEnvHandle) == SQL_ERROR) {
		}
		if (SQLSetEnvAttr(SQLEnvHandle, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0) == SQL_ERROR) {
		} 
		if (SQLAllocHandle(SQL_HANDLE_DBC, SQLEnvHandle, &SQLConnectionHandle) == SQL_ERROR) {
		}
		if (SQLSetConnectAttr(SQLConnectionHandle, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0) == SQL_ERROR) {
		}

		SQLCHAR retConString[1024];
		string ConnectionString = "DRIVER={SQL Server Native Client 11.0}; SERVER=" + DataSource + "; DATABASE=" + DatabaseName + ";Uid=OM_USER;Pwd=OMSQL@2004;";
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
			break;
		case SQL_INVALID_HANDLE:
			cout << "Error - SQL_INVALID_HANDLE" << string(2, '\n');;
			showSQLErrorMsg(SQL_HANDLE_DBC, SQLConnectionHandle);
			break;
		case SQL_ERROR:
			cout << "Error - SQL_ERROR" << string(2, '\n');
			showSQLErrorMsg(SQL_HANDLE_DBC, SQLConnectionHandle);
			break;
		}

		if (SQLAllocHandle(SQL_HANDLE_STMT, SQLConnectionHandle, &SQLStatementHandle) == SQL_ERROR) {
			cout << left << setw(10) << "Error allocating SQL Statement Handle" << endl;
		}

		char SQLQuery[] = "SELECT patient_no, first_name, last_name FROM patient ORDER BY patient_no ASC";

		// Allocate Handle for SQLStatementHandle and using char Query[]
		if (SQLExecDirect(SQLStatementHandle, (SQLCHAR*)SQLQuery, SQL_NTS) == SQL_ERROR) {
			cout << left << setw(10) << "Error allocating SQL Statement Handle" << endl;
			showSQLErrorMsg(SQL_HANDLE_STMT, SQLStatementHandle);
		}
		else 
		{	
			int patientID;
			char first_name[256];
			char last_name[256];

			while (SQLFetch(SQLStatementHandle) == SQL_SUCCESS) {
				SQLGetData(SQLStatementHandle, 1, SQL_C_DEFAULT, &patientID, sizeof(patientID) + 1, NULL);
				SQLGetData(SQLStatementHandle, 2, SQL_C_DEFAULT, &first_name, size(first_name), NULL);
				SQLGetData(SQLStatementHandle, 3, SQL_C_DEFAULT, &last_name, size(last_name), NULL);
				cout << right << setw(10) << patientID << " " << first_name << " " << last_name << endl;
			}
		}

	// Free SQL Handles previously in use
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