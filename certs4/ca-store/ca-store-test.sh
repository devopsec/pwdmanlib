#!/bin/bash -

# Create certificates

certs_dir=/pwdmanlib/certs4/certs
mkdir -p $certs_dir
cd $certs_dir
cd ..

for i in 1 2; do
  pk="cert${i}.key"
  cert="cert${i}.pem"
  req=cert${i}.req

  openssl genrsa -out $pk 2048
  openssl req -new -key $pk -out $req
  openssl x509 -req -days 365 -in $req -signkey $pk -out $cert
done

# Move the second certificate from this directory
	
mv cert2* $certs_dir

# Create trusted certificates bundle

bundle_file=CAbundle.pem

rm -f $bundle_file

for i in *.pem; do
	openssl x509 -in $i -text >> $bundle_file
done

c_rehash .

ls -l

# server. Trust certificates from CAbundle.pem.

openssl s_server -key cert1.key -cert cert1.pem -Verify 0 \
  -accept 9998 -CAfile CAbundle.pem \
  -crlf

# client with untrusted certificate

openssl s_client -key $certs_dir/cert2.key -cert $certs_dir/cert2.pem \
  -connect localhost:9998


# Client output

: <<COMMENTBLOCK
verify depth is 0
CONNECTED(00000003)
depth=0 C = AU, ST = StateA, L = CityA, O = CompanyA, CN = localhost, emailAddress = a@gmail.com
verify error:num=18:self signed certificate
verify return:1
depth=0 C = AU, ST = StateA, L = CityA, O = CompanyA, CN = localhost, emailAddress = a@gmail.com
verify return:1
---
Certificate chain
 0 s:/C=AU/ST=StateA/L=CityA/O=CompanyA/CN=localhost/emailAddress=a@gmail.com
   i:/C=AU/ST=StateA/L=CityA/O=CompanyA/CN=localhost/emailAddress=a@gmail.com
---
Server certificate
-----BEGIN CERTIFICATE-----
MIIDXjCCAkYCCQDVdvVFVIVeBTANBgkqhkiG9w0BAQsFADBxMQswCQYDVQQGEwJB
VTEPMA0GA1UECAwGU3RhdGVBMQ4wDAYDVQQHDAVDaXR5QTERMA8GA1UECgwIQ29t
cGFueUExEjAQBgNVBAMMCWxvY2FsaG9zdDEaMBgGCSqGSIb3DQEJARYLYUBnbWFp
bC5jb20wHhcNMTYwNDI0MDcwMDQ5WhcNMTcwNDI0MDcwMDQ5WjBxMQswCQYDVQQG
EwJBVTEPMA0GA1UECAwGU3RhdGVBMQ4wDAYDVQQHDAVDaXR5QTERMA8GA1UECgwI
Q29tcGFueUExEjAQBgNVBAMMCWxvY2FsaG9zdDEaMBgGCSqGSIb3DQEJARYLYUBn
bWFpbC5jb20wggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDW1tOqU+Tg
5k+Q9EEKTmNzj4zmrkNbFR4hOxY1cJmSSi5iWrCJa+mS07JkAsptWgwqUfIhQgAX
N8UN6Kzx+MnRzglomHvx9wxJESHfizzSGlRv3JYuPxXYZFdqGoObIPVmmFlHFHk7
B/UaVM/cg8CAHMpqTveduX6kq3z1vzs1TP/tYzVwhgkh68867v/MNLdQ8RxGKQfG
iAmlndUoz2cIepd3j4cooacQqw7hXBuV83pHrbRdz5GAQroF+XCXoyI/2Y6F+8eQ
wlvSYf7IO+ran29ow4PaJcQNAKORK5Rjxc/ONLER68W6ekOd2kME2C7IzBWmZ/bH
ixpZp/FcIfHtAgMBAAEwDQYJKoZIhvcNAQELBQADggEBABvW05MFvl/aYYhSIm60
g2Y8xhR4z4o8iS9OUa98wVh2KWm2By/rANKqiwactQ6UBxJX8CJC9K0kgU1dsOUa
maXAAulAv11bk8MCZ7jJdH70J2o2hzC8QzmyZfUR4YFZzIQsh1vHtoDKuuoQIdT/
a5DOa63Ht3AtaupRdULwlJcXwTAe5+I65HyDNZLSNckB4v6JcN7uIe34/WSvRL5c
K8fl9sLwPlmvmzk9VbTzmwSBRdAyCrRRpYXPg7b1IA9LAkPO9oQErB/PqU65b1Yr
tgPUgUFjqilp1VBySM2YQ9NedRQrLp4Vj7TwazoZ/vog7FMfnfss2i6AEQ9Rxn0G
b3M=
-----END CERTIFICATE-----
subject=/C=AU/ST=StateA/L=CityA/O=CompanyA/CN=localhost/emailAddress=a@gmail.com
issuer=/C=AU/ST=StateA/L=CityA/O=CompanyA/CN=localhost/emailAddress=a@gmail.com
---
Acceptable client certificate CA names
/C=AU/ST=StateA/L=CityA/O=CompanyA/CN=localhost/emailAddress=a@gmail.com
Client Certificate Types: RSA sign, DSA sign, ECDSA sign
Requested Signature Algorithms: RSA+SHA512:DSA+SHA512:ECDSA+SHA512:RSA+SHA384:DSA+SHA384:ECDSA+SHA384:RSA+SHA256:DSA+SHA256:ECDSA+SHA256:RSA+SHA224:DSA+SHA224:ECDSA+SHA224:RSA+SHA1:DSA+SHA1:ECDSA+SHA1
Shared Requested Signature Algorithms: RSA+SHA512:DSA+SHA512:ECDSA+SHA512:RSA+SHA384:DSA+SHA384:ECDSA+SHA384:RSA+SHA256:DSA+SHA256:ECDSA+SHA256:RSA+SHA224:DSA+SHA224:ECDSA+SHA224:RSA+SHA1:DSA+SHA1:ECDSA+SHA1
Peer signing digest: SHA512
Server Temp Key: ECDH, P-256, 256 bits
---
SSL handshake has read 2573 bytes and written 1592 bytes
---
New, TLSv1/SSLv3, Cipher is ECDHE-RSA-AES256-GCM-SHA384
Server public key is 2048 bit
Secure Renegotiation IS supported
Compression: zlib compression
Expansion: zlib compression
No ALPN negotiated
SSL-Session:
    Protocol  : TLSv1.2
    Cipher    : ECDHE-RSA-AES256-GCM-SHA384
    Session-ID: 29FED26A404779D594FCD9517AAC1A0B49A1BDA04E093D5409E5775C888568FA
    Session-ID-ctx: 
    Master-Key: 9F230C7E33FD9BCA950FFF51F93701FCE3406C1ADB67E540B02521D554FE51C070FD8B9772B5696132F01DACEFBA0C95
    Key-Arg   : None
    PSK identity: None
    PSK identity hint: None
    SRP username: None
    TLS session ticket lifetime hint: 300 (seconds)
    TLS session ticket:
    ...
    Compression: 1 (zlib compression)
    Start Time: 1461482590
    Timeout   : 300 (sec)
    Verify return code: 18 (self signed certificate)
---
test
COMMENTBLOCK

# Server output

: <<COMMENTBLOCK
verify depth is 0, must return a certificate
Using default temp DH parameters
ACCEPT
depth=0 C = AU, ST = StateB, L = CityB, O = CompanyB, CN = localhost, emailAddress = b@gmail.com
verify error:num=18:self signed certificate
verify return:1
depth=0 C = AU, ST = StateB, L = CityB, O = CompanyB, CN = localhost, emailAddress = b@gmail.com
verify return:1
-----BEGIN SSL SESSION PARAMETERS-----
MIIDxQIBAQICAwMEAsAwBAAEMJ8jDH4z/ZvKlQ//Ufk3AfzjQGwa22flQLAlIdVU
/lHAcP2Ll3K1aWEy8B2s77oMlaEGAgRXHHReogQCAgEso4IDYjCCA14wggJGAgkA
jPMT5ptF/agwDQYJKoZIhvcNAQELBQAwcTELMAkGA1UEBhMCQVUxDzANBgNVBAgM
BlN0YXRlQjEOMAwGA1UEBwwFQ2l0eUIxETAPBgNVBAoMCENvbXBhbnlCMRIwEAYD
VQQDDAlsb2NhbGhvc3QxGjAYBgkqhkiG9w0BCQEWC2JAZ21haWwuY29tMB4XDTE2
MDQyNDA3MDEwNVoXDTE3MDQyNDA3MDEwNVowcTELMAkGA1UEBhMCQVUxDzANBgNV
BAgMBlN0YXRlQjEOMAwGA1UEBwwFQ2l0eUIxETAPBgNVBAoMCENvbXBhbnlCMRIw
EAYDVQQDDAlsb2NhbGhvc3QxGjAYBgkqhkiG9w0BCQEWC2JAZ21haWwuY29tMIIB
IjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAvu025CS+6DcLxM2afQUXzR6D
O2/Dwf3h8X6+hQ22PoAUeEOoUpXHHbdQH7izbazIgMErkPvFFk8ML3gwnGubouqe
P/WpqwrNM36JJrr+3UWvUaWlxjvxtSMauv7XpkX+6K3mwXB7zd5OiDmPDHhAVqRv
i/z5EeKdOO6HQ0p8FIglVqPs5QSjsO/P1YZmb4X9ZmpdbNmXorrCjpA6p+uJPDoT
97pxn13yuNnowAn5tll5+J3hqGyk/Vu8uYf9JWx1XxHK4V2cEOWlBtzXD0FfyI7L
nA8kNiIfEoysbrWEToeyoRAS92yCgeo7PgE4RPucp0y8iivK9yYa2K1UNUUXXwID
AQABMA0GCSqGSIb3DQEBCwUAA4IBAQA6OvmzbEmQzV3NZ3Qc/VHQp6fKUqV9Z1X0
0VBXjZ2EMPXz847i4R/51v45HpOSm2IwlsMGhiyO3Zx3VURkJGUSAx54Ogvje18H
oIzjeuXpBJ0XtAlz+EHNDMy5+n3u6AkNUC4Ifd42dZePXsAix5pPG2B2LWaMke4S
7ytYwEuHIfCKJbfUfx3GHx+mXca44Hg+bu7bRzr1rVmm3yD6iOFpn/zuGoiPGi4k
M4WOpQ7Q8T7BUAZa01FQ8E+YtWHVgq1AWOzZYVJ1xP2SA7ztuyAhFdFwM8MY0In1
dzXB5/z2L6j3W4lDr6EtVi3MHu9J0oAf4WOH0SvBgw7K0YXabTcHpAYEBAEAAACl
AwIBEqsDBAEB
-----END SSL SESSION PARAMETERS-----
Client certificate
-----BEGIN CERTIFICATE-----
MIIDXjCCAkYCCQCM8xPmm0X9qDANBgkqhkiG9w0BAQsFADBxMQswCQYDVQQGEwJB
VTEPMA0GA1UECAwGU3RhdGVCMQ4wDAYDVQQHDAVDaXR5QjERMA8GA1UECgwIQ29t
cGFueUIxEjAQBgNVBAMMCWxvY2FsaG9zdDEaMBgGCSqGSIb3DQEJARYLYkBnbWFp
bC5jb20wHhcNMTYwNDI0MDcwMTA1WhcNMTcwNDI0MDcwMTA1WjBxMQswCQYDVQQG
EwJBVTEPMA0GA1UECAwGU3RhdGVCMQ4wDAYDVQQHDAVDaXR5QjERMA8GA1UECgwI
Q29tcGFueUIxEjAQBgNVBAMMCWxvY2FsaG9zdDEaMBgGCSqGSIb3DQEJARYLYkBn
bWFpbC5jb20wggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQC+7TbkJL7o
NwvEzZp9BRfNHoM7b8PB/eHxfr6FDbY+gBR4Q6hSlccdt1AfuLNtrMiAwSuQ+8UW
TwwveDCca5ui6p4/9amrCs0zfokmuv7dRa9RpaXGO/G1Ixq6/temRf7orebBcHvN
3k6IOY8MeEBWpG+L/PkR4p047odDSnwUiCVWo+zlBKOw78/VhmZvhf1mal1s2Zei
usKOkDqn64k8OhP3unGfXfK42ejACfm2WXn4neGobKT9W7y5h/0lbHVfEcrhXZwQ
5aUG3NcPQV/IjsucDyQ2Ih8SjKxutYROh7KhEBL3bIKB6js+AThE+5ynTLyKK8r3
JhrYrVQ1RRdfAgMBAAEwDQYJKoZIhvcNAQELBQADggEBADo6+bNsSZDNXc1ndBz9
UdCnp8pSpX1nVfTRUFeNnYQw9fPzjuLhH/nW/jkek5KbYjCWwwaGLI7dnHdVRGQk
ZRIDHng6C+N7XwegjON65ekEnRe0CXP4Qc0MzLn6fe7oCQ1QLgh93jZ1l49ewCLH
mk8bYHYtZoyR7hLvK1jAS4ch8Iolt9R/HcYfH6ZdxrjgeD5u7ttHOvWtWabfIPqI
4Wmf/O4aiI8aLiQzhY6lDtDxPsFQBlrTUVDwT5i1YdWCrUBY7NlhUnXE/ZIDvO27
ICEV0XAzwxjQifV3NcHn/PYvqPdbiUOvoS1WLcwe70nSgB/hY4fRK8GDDsrRhdpt
Nwc=
-----END CERTIFICATE-----
subject=/C=AU/ST=StateB/L=CityB/O=CompanyB/CN=localhost/emailAddress=b@gmail.com
issuer=/C=AU/ST=StateB/L=CityB/O=CompanyB/CN=localhost/emailAddress=b@gmail.com
Shared ciphers:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-RSA-AES256-SHA384:ECDHE-ECDSA-AES256-SHA384:ECDHE-RSA-AES256-SHA:ECDHE-ECDSA-AES256-SHA:DH-DSS-AES256-GCM-SHA384:DHE-DSS-AES256-GCM-SHA384:DH-RSA-AES256-GCM-SHA384:DHE-RSA-AES256-GCM-SHA384:DHE-RSA-AES256-SHA256:DHE-DSS-AES256-SHA256:DH-RSA-AES256-SHA256:DH-DSS-AES256-SHA256:DHE-RSA-AES256-SHA:DHE-DSS-AES256-SHA:DH-RSA-AES256-SHA:DH-DSS-AES256-SHA:DHE-RSA-CAMELLIA256-SHA:DHE-DSS-CAMELLIA256-SHA:DH-RSA-CAMELLIA256-SHA:DH-DSS-CAMELLIA256-SHA:ECDH-RSA-AES256-GCM-SHA384:ECDH-ECDSA-AES256-GCM-SHA384:ECDH-RSA-AES256-SHA384:ECDH-ECDSA-AES256-SHA384:ECDH-RSA-AES256-SHA:ECDH-ECDSA-AES256-SHA:AES256-GCM-SHA384:AES256-SHA256:AES256-SHA:CAMELLIA256-SHA:ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES128-SHA256:ECDHE-ECDSA-AES128-SHA256:ECDHE-RSA-AES128-SHA:ECDHE-ECDSA-AES128-SHA:DH-DSS-AES128-GCM-SHA256:DHE-DSS-AES128-GCM-SHA256:DH-RSA-AES128-GCM-SHA256:DHE-RSA-AES128-GCM-SHA256:DHE-RSA-AES128-SHA256:DHE-DSS-AES128-SHA256:DH-RSA-AES128-SHA256:DH-DSS-AES128-SHA256:DHE-RSA-AES128-SHA:DHE-DSS-AES128-SHA:DH-RSA-AES128-SHA:DH-DSS-AES128-SHA:DHE-RSA-SEED-SHA:DHE-DSS-SEED-SHA:DH-RSA-SEED-SHA:DH-DSS-SEED-SHA:DHE-RSA-CAMELLIA128-SHA:DHE-DSS-CAMELLIA128-SHA:DH-RSA-CAMELLIA128-SHA:DH-DSS-CAMELLIA128-SHA:ECDH-RSA-AES128-GCM-SHA256:ECDH-ECDSA-AES128-GCM-SHA256:ECDH-RSA-AES128-SHA256:ECDH-ECDSA-AES128-SHA256:ECDH-RSA-AES128-SHA:ECDH-ECDSA-AES128-SHA:AES128-GCM-SHA256:AES128-SHA256:AES128-SHA:SEED-SHA:CAMELLIA128-SHA:IDEA-CBC-SHA:ECDHE-RSA-RC4-SHA:ECDHE-ECDSA-RC4-SHA:ECDH-RSA-RC4-SHA:ECDH-ECDSA-RC4-SHA:RC4-SHA:RC4-MD5:ECDHE-RSA-DES-CBC3-SHA:ECDHE-ECDSA-DES-CBC3-SHA:EDH-RSA-DES-CBC3-SHA:EDH-DSS-DES-CBC3-SHA:DH-RSA-DES-CBC3-SHA:DH-DSS-DES-CBC3-SHA:ECDH-RSA-DES-CBC3-SHA:ECDH-ECDSA-DES-CBC3-SHA:DES-CBC3-SHA
Signature Algorithms: RSA+SHA512:DSA+SHA512:ECDSA+SHA512:RSA+SHA384:DSA+SHA384:ECDSA+SHA384:RSA+SHA256:DSA+SHA256:ECDSA+SHA256:RSA+SHA224:DSA+SHA224:ECDSA+SHA224:RSA+SHA1:DSA+SHA1:ECDSA+SHA1
Shared Signature Algorithms: RSA+SHA512:DSA+SHA512:ECDSA+SHA512:RSA+SHA384:DSA+SHA384:ECDSA+SHA384:RSA+SHA256:DSA+SHA256:ECDSA+SHA256:RSA+SHA224:DSA+SHA224:ECDSA+SHA224:RSA+SHA1:DSA+SHA1:ECDSA+SHA1
Peer signing digest: SHA512
Supported Elliptic Curve Point Formats: uncompressed:ansiX962_compressed_prime:ansiX962_compressed_char2
Supported Elliptic Curves: P-256:P-521:brainpoolP512r1:brainpoolP384r1:P-384:brainpoolP256r1:secp256k1:B-571:K-571:K-409:B-409:K-283:B-283
Shared Elliptic curves: P-256:P-521:brainpoolP512r1:brainpoolP384r1:P-384:brainpoolP256r1:secp256k1:B-571:K-571:K-409:B-409:K-283:B-283
CIPHER is ECDHE-RSA-AES256-GCM-SHA384
Secure Renegotiation IS supported
test
COMMENTBLOCK
