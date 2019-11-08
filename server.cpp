// server.cpp : Defines the entry point for the application.
//

#include <windows.h>
#include "common.h"
#include "server.h"

using namespace std;

int main()
{
	cout << "I am the server." << endl;

	const DWORD pipeMode{ PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE };
	// use PIPE_WAIT or PIPE_NOWAIT for sync/async?

	const HANDLE handle = CreateNamedPipe(pipeName, PIPE_ACCESS_DUPLEX, pipeMode, PIPE_UNLIMITED_INSTANCES, 0, 0, 0, NULL);
	return 0;
}
