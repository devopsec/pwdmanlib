#!/bin/bash

echo "creating root CA cert"
openssl req -config ca.conf -new -x509 -sha256 -newkey rsa:4096 -nodes -keyout rootCA.key -days 365 -out rootCA.pem
#openssl req -x509 -newkey rsa:4096 -nodes -keyout rootCA.key -days 365 -out rootCA.pem -subj "/C=US/ST=Michigan/L=DETROIT/O=DevOpSec Labs/OU=IT/CN=www.devopsec.net/emailAddress=ca@devopsec.net"
echo "root CA cert:"
openssl x509 -in rootCA.pem -text -noout

echo "creating client cert"
openssl req -nodes -x509 -newkey rsa:4096 -keyout client.key
openssl req -nodes -x509 -newkey rsa:4096 -keyout client.key -out client.csr
openssl req -nodes -x509 -newkey rsa:4096 -keyout client.key -out client.csr "/C=US/ST=Michigan/L=Detroit/O=client/OU=IT/CN=www.client.com/emailAddress=test@client.com"
openssl x509 -req -in client.csr -CA rootCA.pem -CAkey rootCA.key -set_serial 01 -out client.pem --extensions usr_cert -days 365 -subj "/C=US/ST=Michigan/L=Detroit/O=client/OU=IT/CN=www.client.com/emailAddress=test@client.com"
echo "client signed cert:"
openssl x509 -in client.pem -text -noout

echo "creating server cert"
openssl genrsa -out server.key 4096
openssl req -x509 -newkey rsa:4096 -nodes server.key -out server.csr -subj "/C=US/ST=Michigan/L=Detroit/O=server/OU=IT/CN=www.server.com/emailAddress=test@server.com"
openssl x509 -req -in server.csr -CA rootCA.pem -CAkey rootCA.key -set_serial 01 -out server.pem --extensions server_cert -days 365 -subj "/C=US/ST=Michigan/L=Detroit/O=server/OU=IT/CN=www.server.com/emailAddress=test@server.com"
echo "server signed cert:"
openssl x509 -in server.pem -text -noout

