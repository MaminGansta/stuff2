

struct Server
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	bool runnig()
	{
		DWORD exit_code = 0;
		GetExitCodeProcess(pi.hProcess, &exit_code);
		return exit_code == STILL_ACTIVE;
	}

	bool create(wchar_t* args)
	{
		if (runnig())
			return false;

		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));
		
		if (!CreateProcess(L"..\\Debug\\battle_city_server.exe", args, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
			doutput("CreateProcess failed (%d).\n", GetLastError());

		return true;
	}

};