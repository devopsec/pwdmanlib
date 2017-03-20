# SSL Certificate Cmds

### openssl
This is the basic command line tool for creating and managing OpenSSL certificates, keys, and other files.
### req
This subcommand specifies that we want to use X.509 certificate signing request (CSR) management. 
The "X.509" is a public key infrastructure standard that SSL and TLS adheres to for its key and certificate management. 
We want to create a new X.509 cert, so we are using this subcommand.
### -x509
This further modifies the previous subcommand by telling the utility that we want to make a self-signed certificate instead of generating a certificate signing request, as would normally happen.
### -nodes
This tells OpenSSL to skip the option to secure our certificate with a passphrase. 
We need client to be able to read the file, without user intervention, when the server starts up. 
A passphrase would prevent this from #happening because we would have to enter it after every restart.
### -days 365
This option sets the length of time that the certificate will be considered valid. 
We set it for one year here.
### -newkey rsa:2048
This specifies that we want to generate a new certificate and a new key at the same time. 
We did not create the key that is required to sign the certificate in a previous step, so we need to create it along with the certificate. 
### rsa:2048
This portion tells it to make an RSA key that is 2048 bits long.
### -keyout
This line tells OpenSSL where to place the generated private key file that we are creating.
### -out
This tells OpenSSL where to place the certificate that we are creating.


## Creating Custom Self-Signed Certs
#### Create a root private key:
```
openssl genrsa -out rootCA.key 4096
```
#### Change permissions of this private key
```
chmod 400 rootCA.key
```
#### Create self-signed root certificate
```
openssl req -x509 -new -nodes -key rootCA.key -days 2048 -out rootCA.pem
```
####  Create private key for final certificate
```
openssl genrsa -out client.key 4096
```
####  Create certificate sign request
```
openssl req -new -key client.key -out client.csr
```
####  create server certificate
```
openssl x509 -req -in client.csr -CA rootCA.pem -CAkey rootCA.key -CAcreateserial -out client.crt -days 365
```


## Walkthrough
Examples of common operations.

#### Create a self signed certificate (notice the addition of -x509 option):
```
openssl req -config /home/anon00/com.conf -new -x509 -sha256 -newkey rsa:4096 -nodes \
    -keyout /etc/ssl/private/com.key.pem -days 365 -out /etc/ssl/private/com.cert.pem
```
#### Create a signing request (notice the lack of -x509 option):
```
openssl req -config /home/anon00/com.conf -new -sha256 -newkey rsa:2048 -nodes \
    -keyout /etc/ssl/private/com.req.key.pem -days 365 -out /etc/ssl/private/com.req.pem
```
#### Print a self signed certificate:
```
openssl x509 -in /etc/ssl/private/com.cert.pem -text -noout
```
#### Print a signing request:
```
openssl req -in /etc/ssl/private/com.req.pem -text -noout
```
