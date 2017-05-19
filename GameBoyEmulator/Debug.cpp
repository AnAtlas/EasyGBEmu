#include <stdio.h>

bool Debug = false;
bool Logging = false;
bool fileOpen = false;

FILE * logFile;



void DebugLogMessage(const char* message) {
	if (!fileOpen) {
		logFile = fopen("myemulator.log", "w");
		fileOpen = true;
	}
	fputs(message, logFile);
}