exploit :
	i686-w64-mingw32-gcc -o malware.exe main.c -lwsock32 -lwininet
runserver :
	gcc server.c -o Server
