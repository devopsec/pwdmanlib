#!/bin/bash

mkdir certs
rm certs/*
echo "creating server cert"
openssl req -new -nodes -x509 -out certs/server.pem -keyout certs/server.key -days 365 -subj "/C=US/ST=UNKNWON/L=UNKNOWN/O=DevOpSec/OU=IT/CN=devopsec.net/emailAddress=testing@devopsec.net"
echo "creating client cert"
openssl req -new -nodes -x509 -out certs/client.pem -keyout certs/client.key -days 365 -subj "/C=US/ST=UNKNWON/L=UNKNOWN/O=DevOpSec/OU=IT/CN=devopsec.net/emailAddress=testing@devopsec.net"

