
wchar_t* open_file(const wchar_t* file_name , int& len)
{
	HANDLE hFile;

	hFile = CreateFile(file_name,
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		_tprintf(TEXT("hFile is NULL\n"));
		_tprintf(TEXT("Target file is %s\n"),
			file_name);
		return  NULL;
	}

	len = GetFileSize(hFile, NULL);
	DWORD nBytesRead;
	BOOL bResult;
	wchar_t* data = new wchar_t[len];

	bResult = ReadFile(hFile, data, len * sizeof(wchar_t), &nBytesRead, NULL);

	if (!bResult)
	{
		doutput("Read failed\n");
		return NULL;
	}

	CloseHandle(hFile);

	return data;
}


void write_file(const wchar_t* file_name, wchar_t* data, int len)
{
	HANDLE hFile;

	hFile = CreateFile(file_name,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		_tprintf(TEXT("hFile is NULL\n"));
		_tprintf(TEXT("Target file is %s\n"),
			file_name);
		return;
	}

	DWORD nBytesWrited;
	BOOL bResult;
	bResult = WriteFile(hFile, data, len * sizeof(wchar_t), &nBytesWrited, NULL);

	if (!bResult)
	{
		doutput("Write failed\n");
		return;
	}

	CloseHandle(hFile);
}