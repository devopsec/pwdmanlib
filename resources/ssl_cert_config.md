# SSL Certificate Configuration

## server certs
create a root Certificate Authority
this will also create a server key and server cert
```
openssl req -nodes -newkey rsa:4096 -keyout ca.key -out ca.crt -x509 -days 365
```

## Client certs
create the client Key
```
openssl req -nodes -newkey rsa:4096 -keyout client.key
```

create client (will be CA verified) CSR 
```
openssl req -nodes -newkey rsa:4096 -keyout client.key -out client.csr
```

create client (Self Signed) cert
```
openssl x509 -nodes -req -days 365 -in client.csr -CA /pwdmanlib/src/server/ca.crt -CAkey /pwdmanlib/src/server/ca.key -set_serial 01 -out client.crt
```

convert client key to PKCS (for browser support)
```
openssl pkcs12 -export -clcerts -in client.crt -inkey client.key -out client.p12
```

convert client key to (combined) PEM
```
openssl pkcs12 -in client.p12 -out client.pem -clcerts
```

#### Some more variations and Examples

```
openssl req -nodes -newkey rsa:2048 -keyout test.key
openssl req -nodes -newkey rsa:2048 -keyout test.key -out test.crt -x509 -days 365
openssl req -config com.conf -new -x509 -sha256 -newkey rsa:4096 -nodes -keyout com.key.pem -days 365 -out com.cert.pem
openssl req -config com.conf -new -sha256 -newkey rsa:2048 -nodes -keyout com.req.key.pem -days 365 -out com.req.pem
openssl x509 -in com.cert.pem -text -noout
openssl req -in com.req.pem -text -noout
```
