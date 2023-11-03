server: 
	gcc -o server_ssl_v1 server_https_v1.c -lssl -lcrypto 
client:
	gcc -o client_ssl_v1 client_ssl_v1.c -lssl -lcrypto 
servobfuscate:
	 gcc -o server_ssl_v2 server_https_v2.c crypto.c -lssl -lcrypto
clientobfuscate:
	 gcc -o client_ssl_v2 client_ssl_v2.c crypto.c -lssl -lcrypto
clean:
	rm -r server_ssl_v2 client_ssl_v2 
killer:
	sudo lsof -t -i:4433
ki:
	sudo kill -9 $1

newMakerWindows:
	 i686-w64-mingw32-gcc -o client_ssl.exe windows.c crypto.c -mwindows -L /home/dkb4rb/Downloads/openssl-3.0.12/ -I /home/dkb4rb/Downloads/openssl-3.0.12/include -lws2_32 -lssl -lcrypto

cert:
	openssl genpkey -algorithm RSA -out Private_Key.key
	openssl req -new -key Private_Key.key -out Request.csr 
	openssl x509 -req -in Request.csr -signkey Private_Key.key -out Certificate.crt
	openssl pkcs12 -export -out Certificate.pfx -inkey Private_Key.key -in Certificate.crt 		
	../../../osslsigncode/build/osslsigncode sign -pkcs12 Certificate.pfx -pass "Hacked" -in client_ssl.exe -out client_ssl_sign.exe 	
