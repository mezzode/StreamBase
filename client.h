// client.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include <string>

using std::string;

void send(string key, int data);
void sendHeader(HANDLE h, string key);
void sendData(HANDLE h, int data);
