// client.cpp : Defines the entry point for the application.
//

#include <windows.h>
#include "common.h"
#include "client.h"

using namespace std;

int main()
{
	cout << "I am the client." << endl;

	// either open pipe once and reuse handle by doing CreateFile then TransactNamedPipe
	// or open and close pipe for each message by using CallNamedPipe

	// for now using CallNamedPipe for simplicity

	// set buffer size depending on size of data we're sending?

	const BOOL success = CallNamedPipe(pipeName, /* TODO */ );

	return 0;
}
