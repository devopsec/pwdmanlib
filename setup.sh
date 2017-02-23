#!/bin/bash
# Author: devopsec and mackhendricks
# Summary: Deployment script for pwdlibman & biicode setup.
# Note: Run script as root user to avoid conflicts in setup.

main() {
    PROJECT_DIR="/pwdmanlib" # change to where you want installed

    ####    Install Dependencies    ####
    DEBIAN_FRONTEND=noninteractive apt-get install -y build-essential cmake libmemcached-dev zlib1g-dev python2.7 setuptools python-dev

    if install_biicode; then
        echo "biicode installed successfully"
    else
        install_biicode_client_dependencies
        install_biicode_common_dependencies
        install_biicode_server_dependencies
        if [ $? -eq 0 ]; then        
            echo "biicode installed successfully"
        else
            echo "error occured while installing biicode dependencies"
            exit -1
        fi
    fi
    
    ### Install Boost dependencies ###
    cd ${PROJECT_DIR}/libraries/deps
    wget http://sourceforge.net/projects/boost/files/boost/1.60.0/boost_1_60_0.tar.bz2
    tar --bzip2 -xf boost_1_60_0.tar.bz2
    rm -r boost_1_60_0.tar.bz2
    
    
    ### Install Database dependencies ###
    DEBIAN_FRONTEND=noninteractive apt-get install -y postgresql libpq-dev
    update-rc.d -f postgresql remove # remove auto startup links
    sudo -u postgres psql postgres # add postgres user to system
    postgres=secure_pass
    # startup the server #
    service postgresql start
    service postgresql status
    sudo -u postgres createuser tmp # create db user "tmp"
    sudo -u postgres psql postgres # logon to server
    ALTER USER tmp WITH password 'pass';
    \q # logoff database
    sudo -u postgres createdb testdb --owner tmp # create a db
    
    pg_config --includedir # get loc of headers
    pg_config --libdir # get loc of libs
    pg_config --includedir-server # get loc of server header files
    

    ####    Configure biicode with Jetbrains Clion    ####
    bii setup:cpp    
    cd / # change to <path to dir containing project>
    bii init $PROJECT_DIR -l=clion
    cd $PROJECT_DIR
    bii init <new library> -l=clion
    cd <new library>
    bii configure

    ####    Resolving Dependencies using biicode    ####
    bii find
    bii configure
}

install_biicode() {
    cd / 
    mkdir biicode
    cd biicode
    git clone https://github.com/biicode/client.git
    git clone https://github.com/biicode/common.git
    git clone https://github.com/biicode/bii-server.git

    pip install -r client/requirements.txt
    pip install -r common/requirements.txt
    pip install -r bii-server/requirements.txt

    touch __init__.py
    echo "\n# pwdmanlib environment variables #\n" >> ~/.bashrc
    echo "export PYTHONPATH=\"${PYTHONPATH}:/biicode\"" >> ~/.bashrc
    echo "export PATH=$:/biicode" >> ~/.bashrc
    source ~/.bashrc
    return 0
}

####    Functions for installing broken biicode dependencies    ####
install_biicode_server_dependencies() {
    yes | pip install wsgiref passlib bottle bottle-memcache python-memcached newrelic gevent greenlet gunicorn rq Delorean PyJWT mandrill mixpanel-py mixpanel-py-async google-measurement-protocol pycrypto stripe colorama pylibmc
}

install_biicode_client_dependencies() {
    yes | pip install requests colorama jinja2 pyserial
}

install_biicode_common_dependencies() {
    yes | pip install pytz python-graph-core pyyaml ply pymongo
}

main "$@" # call main with cmd-line args

