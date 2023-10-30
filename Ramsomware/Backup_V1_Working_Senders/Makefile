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
