// MSQLBackup.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MString.h"
#include "MPath.h"
#include "MShellExecuteWait.h"

const char zip[] = "C:\\Program Files\\7-Zip\\7z.exe";

void Compress(char * filein, char * fileout)
{
	printf("Compressing %s => %s\n", filein, fileout);

	//const char zip[] = "C:\\meinedat\\projects\\MParamCapture\\release\\MParamCapture.exe";
	
	//spawnlp(_P_WAIT, zip, "a", "-tgzip", fileout, filein, 0);

	char params[1024];
	sprintf(params, "a -tgzip %s %s", fileout, filein);
	MShellExecuteWait(0, "open", zip, params, NULL, SW_HIDE);
	
	printf("done\n");
}

void BackupServer(char * server, char * login, char * password, char * backuppath, bool compress)
{
	char backuppathabsolute[1024 * 64];
	MPathMakeAbsolute(backuppath, backuppathabsolute);

	MSQLSQLServer msql;
	char connectionstring[1024];
	sprintf(connectionstring, "DRIVER={SQL Server Native Client 11.0};SERVER=%s;DATABASE=master;UID=%s;PWD=%s;Trusted_Connection=yes;", server, login, password);
	msql.Init(connectionstring);

/*
	// all done in sql on the server
	
	char query[1024 * 64];
	sprintf(query, "\
		DECLARE @name VARCHAR(50) -- database name \
		DECLARE @path VARCHAR(256) --path for backup files \
		DECLARE @fileName VARCHAR(256) --filename for backup \
\
		SET @path = '%s' \
\
		DECLARE db_cursor CURSOR FOR \
		SELECT name \
		FROM master.dbo.sysdatabases \
		WHERE name NOT IN('master', 'model', 'msdb', 'tempdb') and not name like 'ReportServer%' \
\
		OPEN db_cursor \
		FETCH NEXT FROM db_cursor INTO @name \
\
		WHILE @@FETCH_STATUS = 0 \
		BEGIN \
		SET @fileName = @path + @name + '.BAK' \
		BACKUP DATABASE @name TO DISK = @fileName \
\
		FETCH NEXT FROM db_cursor INTO @name \
		END \
\
		CLOSE db_cursor \
		DEALLOCATE db_cursor \
		", backuppath);
*/


	//if (auto rst = msql.Query("SELECT name FROM MASTER.dbo.sysdatabases WHERE name NOT IN('master', 'model', 'msdb', 'tempdb') and not name like 'ReportServer%%'"))
	if (auto rst = msql.Query("SELECT name FROM sys.databases WHERE name NOT IN('master', 'model', 'msdb', 'tempdb') and not name like 'ReportServer%%'"))
	{
		MArrayE < MString > names;

		while (rst->Fetch())
		{
			char name[1024];
			*rst << name;

			names.AddBack(name);
		}
		msql.QueryClose(rst);

		unsigned int i;
		for (i = 0; i < names.GetN(); i++)
		{
			const MString & name = names[i];
			printf("Backup Database %s\n", name.c_str());

			char filename[1024];
			sprintf(filename, "%s%s.BAK", backuppathabsolute, name.c_str());

			_unlink(filename);

			char query[1024 * 64];
			//sprintf(query, "BACKUP DATABASE [%s] TO DISK = N'%s' WITH INIT , NOUNLOAD , NAME = N'%DATABASENAME% backup', NOSKIP , STATS = 10, NOFORMAT",
			//	name.c_str(), filename, name.c_str());
			sprintf(query, "BACKUP DATABASE [%s] TO DISK = '%s'",
				name.c_str(), filename);
			auto rst1 = msql.Query(query);

			// BACKUP DATABASE seems to be asynchron. So let's wait for the file to appear
			int count = 3;
			while (count != 0)
			{
				if (MFileExists(filename))
				{
					count--;
				}
				else
				{
					count = 3;
				}
				Sleep(100);
			}

			msql.QueryClose(rst1);

			if (compress)
			{
				char filenamezip[1024];
				sprintf(filenamezip, "%s.gz", filename);
				_unlink(filenamezip);
				Compress(filename, filenamezip);
				_unlink(filename);
			}
		}

		msql.Destroy();
	}
}

inline void PrintHelp()
{
	printf("usage: MSQLBackup [server] [login] [password] [backuppath] [compress]\n");
	printf("\tserver       for example (local)\\SQLEXPRESS\n");
	printf("\tlogin        for example sa\n");
	printf("\tpassword     login's password\n");
	printf("\tbackuppath   Path where all the databases are getting backuped to. The single backups will get named like the\n");
	printf("\t             database names. It needs a trailing \\. For example e:\\backup\\\n");
	printf("\tcompress     If 0, the output doesn't get compressed. If 1, 7z will be used on the output. You have to have 7z\n");
	printf("\t             installed in %s .\n", zip);
	printf("Also note this tool cannot backup to network shares. It uses the BACKUP DATABASE command of SQL Server and\n");
	printf("apparently, it does not backup to network shares.\n");
}

int main(int argc, char ** argv)
{
	printf("MSQLBackup    v0 (c) 2018 M. Rochel\n");

	if (argc != 6)
	{
		PrintHelp();
	}
	else
	{
		char * server = argv[1];
		char * login = argv[2];
		char * password = argv[3];
		char * backuppath = argv[4];
		char * compress = argv[5];

		BackupServer(server, login, password, backuppath, compress[0] == '1');
	}
	
    return 0;
}

