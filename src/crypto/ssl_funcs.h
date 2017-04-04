/********************************************************************************
 *                          Copyright 2017 DevOpSec                             *
 *                                                                              *
 *   Licensed under the Apache License, Version 2.0 (the "License");            *
 *   you may not use this file except in compliance with the License.           *
 *   You may obtain a copy of the License at                                    *
 *                                                                              *
 *       http://www.apache.org/licenses/LICENSE-2.0                             *
 *                                                                              *
 *   Unless required by applicable law or agreed to in writing, software        *
 *   distributed under the License is distributed on an "AS IS" BASIS,          *
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   *
 *   See the License for the specific language governing permissions and        *
 *   limitations under the License.                                             *
 *                                                                              *
 *      Contributors: ....                                                      *
 *                                                                              *
 ********************************************************************************/

#ifndef PWDMANLIB_SSL_H
#define PWDMANLIB_SSL_H

/********************************************************************************
 *                        SSL Implementation Functions                          *
 ********************************************************************************/

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/crypto.h>
#include <openssl/pem.h>
#include <openssl/opensslv.h>
//#include <engine.h>
//#include <conf.h>
//#include <openssl/applink.c>

#include "validation/openssl_validation.h"
#include "util/constants.h"

/* Function Pre-Declarations */
static void        initSSL();
static void        destroySSL(SSL *cSSL, SSL_CTX *ctx);
static void        shutdownSSL(SSL *ssl, SSL_CTX *ctx);
SSL_CTX            *initServerCTX();
SSL                *configServerSSL(int worker_sock);
SSL_CTX            *initClientCTX();
SSL                *configClientSSL(int connect_sock);
static void        showCerts(SSL *ssl);
static void        loadCerts(SSL_CTX *ctx, const char *cert_file, const char *key_file);
static void        configCAStack(const char *cafile, const char *capath, SSL_CTX *ctx);
static void        logSSLErr(const char *msg, FILE *log);
static void        printSSLErr(const char *msg);
static int         sendall(SSL *ssl, char *strbuf, int *len);
static int         readall(SSL *ssl, char *buf);

static void initSSL() {
#if OPENSSL_VERSION_NUMBER < 0x10100000L
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    ERR_load_crypto_strings();
#else
    OPENSSL_init();
    if (OPENSSL_init_ssl(OPENSSL_INIT_LOAD_SSL_STRINGS | OPENSSL_INIT_LOAD_CRYPTO_STRINGS, NULL) == 0) {
        fprintf(stderr, "OPENSSL_init_ssl() error: could not init library\n");
        exit(-1);
    }

    if (OPENSSL_init_crypto(OPENSSL_INIT_ADD_ALL_CIPHERS | OPENSSL_INIT_ADD_ALL_DIGESTS, NULL) == 0) {
        fprintf(stderr, "OPENSSL_init_crypto() error: could not init library\n");
        exit(-1);
    }

    ERR_load_BIO_strings();
    ERR_load_PEM_strings();
#endif
}

SSL_CTX *initServerCTX() {
    const SSL_METHOD *method = TLS_server_method();
    if (!method) {
        printSSLErr("initServerCTX() error: method is NULL");
    }

    SSL_CTX *ctx = SSL_CTX_new(method);
    if (!ctx) {
        printSSLErr("initServerCTX() error: ctx is NULL");
    }

    if (!ctx) {
        perror("initServerCTX() error\n");
        ERR_print_errors_fp(stderr);
        //send err response
    }
    return ctx;
}

/** Full implementation and setup for server ssl occurs here **/
SSL *configServerSSL(int worker_sock) {
    int status, ssl_err;
    const char *cert      =    "/pwdmanlib/certs/server.pem";
    const char *key       =    "/pwdmanlib/certs/server.key";
    const char *cafile    =    "/pwdmanlib/certs2/rootCA.pem";
    const char *capath    =    NULL;
    SSL_CTX *ctx;

    /* initialize ssl libraries & contex */
    initSSL();
    ctx = initServerCTX();

    /* set SSL contex configurations and options */
    if (!ctx) { /* ensure we set the reference ptr */
        printSSLErr("initServerCTX() error: ctx is NULL");
    }

    /* Enable certificate validation */
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);
    SSL_CTX_set_verify_depth(ctx, 4);
//    SSL_CTX_set_cert_verify_callback();
//    SSL_CTX_set_client_CA_list(ssl_ctx,SSL_load_client_CA_file("client_chain.pem"));
//    SSL_CTX_set_options(ctx, SSL_OP_ALL);

    /* set certs to use for SSL */
    loadCerts(ctx, cert, key);

    /* create new SSL connection */
    SSL *cSSL = SSL_new(ctx);

    showCerts(cSSL); /* DEBUG: print um */

    /* set SSL settings on sock */
    SSL_set_fd(cSSL, worker_sock);

    ssl_err = SSL_accept(cSSL);
    if (ssl_err <= 0) {
        fprintf(stderr, ("SSL_accept() error\n"));
        shutdownSSL(cSSL, ctx);
        exit(1); //DEBUG
    }

    return cSSL;
}

SSL_CTX *initClientCTX() {
    const SSL_METHOD *method = TLS_client_method();
    if (!method) {
        printSSLErr("initClientCTX() error: method is NULL");
    }

    SSL_CTX *ctx = SSL_CTX_new(method);
    if (!ctx) {
        printSSLErr("initClientCTX() error: ctx is NULL");
    }

    return ctx;
}

/** Full implementation and setup for client ssl occurs here **/
SSL *configClientSSL(int connect_sock) {
    int status;
    SSL_CTX *ctx;
    X509 *recv_cert;
    const char *rootCA    =    "/pwdmanlib/certs2/rootCA.pem";

    /* initialize ssl libraries */
    initSSL();

    /* set SSL contex configurations and options */
    ctx = initClientCTX();

//    SSL_CTX_set_ecdh_auto(ctx, 1);
//    SSL_CTX_set_options(ctx, SSL_OP_SINGLE_DH_USE);

    /* enable certificate validation */
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);
//    SSL_CTX_set_mode(ctx, SSL_MODE_AUTO_RETRY);

    /* configure client to authenticate using CA trust stack */
    if (SSL_CTX_load_verify_locations(ctx, rootCA, NULL) != 1) {
        printSSLErr("SSL_CTX_load_verify_locations() error: Couldn't load CA Stack");
        SSL_CTX_free(ctx);
        destroySSL(NULL, NULL);
        close(connect_sock);
        exit(-1);
    }

    /* create new SSL connection */
    SSL *cSSL = SSL_new(ctx);
    if (!cSSL) {
        printSSLErr("SSL_new() error: Could not create SSL struct");
        shutdownSSL(cSSL, ctx);
        destroySSL(NULL, NULL);
        close(connect_sock);
        exit(-1);
    }

    /* attach SSL layer to sock (I/O directed through sock now) */
    status = SSL_set_fd(cSSL, connect_sock);
    if (status == 0) {
        printSSLErr("SSL_set_fd() error: Could not attach to sock");
        shutdownSSL(cSSL, ctx);
        destroySSL(NULL, NULL);
        close(connect_sock);
        exit(-1);
    }

    /* establish connection & start TLS/SSL handshake */
    status = SSL_connect(cSSL);
    long verify_result = SSL_get_verify_result(cSSL);
    if (status <= 0) { /* error occurred */
        if (verify_result != X509_V_OK) {
            switch (verify_result) {
                //TODO
            }

            fprintf(stderr, "SSL_accept() error: Certificate Chain validation failed (Fatal Error):\n");
            printSSLErr(X509_verify_cert_error_string(status));
            // retry handshake, handling error, or exit
        }
        else {
            printSSLErr("SSL_accept() error: handshake was not successful (Fatal Error)");
        }

        shutdownSSL(cSSL, ctx);
        destroySSL(NULL, NULL);
        close(connect_sock);
        exit(-1);
    }

    showCerts(cSSL); // DEBUG

    /* receive certificate (sent from server) */
    recv_cert =  SSL_get_peer_certificate(cSSL);
    if (!recv_cert) {
        printSSLErr("SSL_get_peer_certificate() error: Server did not provide a certificate");
        shutdownSSL(cSSL, ctx);
        destroySSL(NULL, NULL);
        close(connect_sock);
        exit(-1);
    }

    /* Validate server hostname */
    if (validate_hostname("devopsec.net", recv_cert) != MatchFound) {
        fprintf(stderr, "Hostname validation failed.\n");
        printSSLErr("SSL_get_peer_certificate() error: Server did not provide a certificate");
        shutdownSSL(cSSL, ctx);
        destroySSL(NULL, NULL);
        close(connect_sock);
        exit(-1);
    }

    return cSSL;
}

/** load public certificate and private key into SSL ctx provided **/
static void loadCerts(SSL_CTX *ctx, const char *cert_file, const char *key_file) {
    /* public key */
    if (SSL_CTX_use_certificate_file(ctx, cert_file , SSL_FILETYPE_PEM) != 1) {
        perror("SSL_CTX_use_certificate_file() error\n");
        ERR_print_errors_fp(stderr);
        //send err response
    }

    /* private key */
    if (SSL_CTX_use_PrivateKey_file(ctx, key_file, SSL_FILETYPE_PEM) != 1) {
        perror("SSL_CTX_use_PrivateKey_file() error\n");
        ERR_print_errors_fp(stderr);
        //send err response
    }

    /* verify private key */
    if (!SSL_CTX_check_private_key(ctx)) {
        printf("Private key does not match the public certificate\n");
        ERR_print_errors_fp(stderr);
        //send err response
    }
}

/*********************************************************************
 * Certificate matching in stack (CA Trust Store) is done based on:  *
 *    -  subject name,                                               *
 *    -  key identifier (if present), and                            *
 *    -  serial number as taken from the certificate to be verified. *
 *********************************************************************/
static void configCAStack(const char *cafile, const char *capath, SSL_CTX *ctx) {
    int status;

    status = SSL_CTX_load_verify_locations(ctx, cafile, capath);
    if (status != 1) {
        printSSLErr("SSL_CTX_load_verify_locations() error: unable to set CA location");
    }

    struct stack_st_X509_NAME *cert_names = SSL_load_client_CA_file(cafile);
    if (cert_names != NULL) {
        SSL_CTX_set_client_CA_list(ctx, cert_names);
    }
    else {
        printSSLErr("configCAStack() error: cert_names is NULL");
    }
}

/** Print received SSL cert to stdout **/
static void showCerts(SSL *ssl) {
    char *line;

    X509 *cert = SSL_get_peer_certificate(ssl);	/* Get certificates (if available) */
    if (cert != NULL) {
        printf("Server certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name (cert), NULL, 0);
        printf("Subject: %s\n", line);
        free(line);
        line = X509_NAME_oneline(X509_get_issuer_name (cert), NULL, 0);
        printf("Issuer: %s\n", line);
        free(line);
        X509_free(cert);
    }
    else {
        printf("No certificates were found\n");
    }
}

/** Print the oneline notation of a Subject DN and Issuer DN to stream **
 ** pass in a stream fp to write to file or pass in stdout for tty out **/
static void printSubjOneline(X509 *cert, FILE *fp) {
    char *ptr;
    int   n;

    X509_NAME *subj = X509_get_subject_name(cert);
    BIO *bmem = BIO_new(BIO_s_mem());
    X509_NAME_print_ex(bmem, subj, 0, XN_FLAG_ONELINE);

    n = (int) BIO_get_mem_data(bmem, &ptr);
    if (n <= 0) ptr = "?", n = 1;
    fwrite(ptr, 1, n, fp);

    if (bmem) { BIO_free_all(bmem); }
    if (subj) { free(subj); }
}

/** Log SSL error details to file **/
static void logSSLErr(const char *msg, FILE *log) {
    fprintf(log, msg);
    fprintf(log, "Error: %s\n", ERR_reason_error_string(ERR_get_error()));
    fprintf(log, "%s\n", ERR_error_string(ERR_get_error(), NULL));
    ERR_print_errors_fp(log);
}

/** Print SSL error details to stdout **/
static void printSSLErr(const char *msg) {
    fprintf(stderr, msg);
    fprintf(stderr, "Error: %s\n", ERR_reason_error_string(ERR_get_error()));
    fprintf(stderr, "%s\n", ERR_error_string(ERR_get_error(), NULL));
    ERR_print_errors_fp(stderr);
}

static void destroySSL(SSL *cSSL, SSL_CTX *ctx) {
    ERR_free_strings();
    EVP_cleanup();
    CRYPTO_cleanup_all_ex_data();
    if (cSSL) { SSL_free(cSSL); }
    if (ctx) { SSL_CTX_free(ctx); }
}

static void shutdownSSL(SSL *cSSL, SSL_CTX *ctx) {
    int rv, err;
    ERR_clear_error();
    rv = SSL_shutdown(cSSL);

    switch (rv) {
        case 1:
            break;
        case 0:
            rv = SSL_shutdown(cSSL);
            if (rv == -1) {
                printSSLErr("shutdownSSL(): bidirectional shutdown error");
            } break;
        case -1:
            err = SSL_get_error(cSSL, rv);
            if (err == SSL_ERROR_SSL) {
                printSSLErr("printSSLErr(): ssl error");
            }
            fprintf(stderr, "SSL State Description: %s\n", SSL_state_string(cSSL));
            break;
        default:
            break;
    }

    FIPS_mode_set(0);
    #if OPENSSL_VERSION_NUMBER < 0x10100000L
        CONF_modules_unload(1);
        ENGINE_cleanup();
        ERR_remove_state();
    #endif
}

/*==== Reading and Writing to & from sock using SSL encryption ====*/
/** ensure kernel doesn't hold onto our data, write until finished */
static int sendall(SSL *ssl, char *strbuf, int *len) {
    int total = 0;            // how many bytes we've sent
    int bytesleft = *len;     // how many we have left to send
    int n = -1;               // bytes sent on current write

    while(total < *len) {
        n = SSL_write(ssl, strbuf + total, bytesleft);	/* send reply */
        if (n < 0) { break; }
        total += n;
        bytesleft -= n;
    }

    *len = total;            // return number actually sent here
    return n < 0 ? n : 0;    // return err on failure, 0 on success
}

/** read data into buf until all received */
static int readall(SSL *ssl, char *buf) {
    int total = 0;           // how many bytes we've read
    int n;

    do {
        n = SSL_read(ssl, buf + total, SOCK_BUFF_DEF_READ);	/* read req */
        total += n;
        /* error checking, buffer overflow checking */
        if (n < 0 || total > SOCK_BUFF_MAX_READ) {
            fprintf(stderr, ("readall() error\n"));
            break;
        }
    } while (n != 0);
    /* return err on failure, 0 on success */
    return n < 0 ? n : 0;
}

#endif //PWDMANLIB_SSL_H
