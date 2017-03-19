#!/bin/bash
# Author: devopsec
# Summary: Deployment script for pwdlibman
# Note: All install cmds must be run as root user or with sudo cmd.
# Note: Function return ints: 0 == success, 1 == incomplete, -1 == error

main() {
    PROJECT_DIR="/pwdmanlib" # change to where you want installed
    DEPS_DIR="${PROJECT_DIR}/libraries/deps"
    LIB_DIR="${PROJECT_DIR}/libraries/lib"
    
    if uname -a | grep -i "linux"; then
        OS="linux"
        
        TEMP=$(cat /etc/lsb-release 2>&1)
        if echo "$TEMP" | grep -iq 'DISTRIB_ID'; then
            OS_DISTRO=$(echo "$TEMP" | grep -i 'DISTRIB_ID' | sed 's/DISTRIB_ID=//g')
            OS_DISTRO=${OS_DISTRO,,} #to lower
            OS_DISTRO_CODENAME=$(echo "$TEMP" | grep -i 'DISTRIB_CODENAME' | sed 's/DISTRIB_CODENAME=//g')
            OS_DISTRO_VER=$(echo "$TEMP" | grep -i 'DISTRIB_RELEASE' | sed 's/DISTRIB_RELEASE=//g')
            echo "Distribution detected: ${OS_DISTRO} ${OS_DISTRO_VER} - \"${OS_DISTRO_CODENAME}\""
        fi

        TEMP=$(cat /etc/upstream-release/lsb-release 2>&1)
        if echo "$TEMP" | grep -iq 'DISTRIB_ID'; then
            UPSTREAM_DISTRO=$(echo "$TEMP" | grep -i 'DISTRIB_ID' | sed 's/DISTRIB_ID=//g')
            UPSTREAM_DISTRO=${UPSTREAM_DISTRO,,} #to lower
            UPSTREAM_DISTRO_CODENAME=$(echo "$TEMP" | grep -i 'DISTRIB_CODENAME' | sed 's/DISTRIB_CODENAME=//g')
            UPSTREAM_DISTRO_VER=$(echo "$TEMP" | grep -i 'DISTRIB_RELEASE' | sed 's/DISTRIB_RELEASE=//g')
            echo "Upstream distribution detected: ${UPSTREAM_DISTRO} ${UPSTREAM_DISTRO_VER} - \"${UPSTREAM_DISTRO_CODENAME}\""
        else
            UPSTREAM_DISTRO=${OS_DISTRO}
            UPSTREAM_DISTRO_CODENAME=${OS_DISTRO_CODENAME}
            UPSTREAM_DISTRO_VER=${OS_DISTRO_VER}
        fi
        

    elif uname -a | grep -i "mac"; then
        OS="mac"
        OSX_VERS=$(sw_vers -productVersion | awk -F "." '{print $2}')
    
    else 
        echo "Operating System not supported at this time.."
        exit -1
    fi
    echo "Operating System detected: ${OS}\n"
}

# TODO: add requirements for mac and windows #
install_required_deps() {    
    if [ "$OS" == "mac" ]; then
        if ! whichapp 'homebrew' &> /dev/null; then
		    echo "installing homebrew.."
		    /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
	    if ! whichapp 'git' &> /dev/null; then
		    echo "installing git.."
		    brew install git
		    git --version
        if ! whichapp 'wget' &> /dev/null; then
            brew install wget

    elif [ "$OS" == "linux" ]; then
        DEBIAN_FRONTEND=noninteractive
        apt-get update -q &&
        apt-get install -q -y build-essential make cmake libmemcached-dev \
            zlib1g-dev python2.7 python-dev git perl dpkg libncurses5-dev
    else
        echo "OS enivron variable not set\nEnsure your os is supported before retrying"
        exit -1
    fi
    
    if [[ ! install_libffi ]]; then
        echo "error occurred installing libffi"
        exit -1
    fi
    
    return 0

    ####    biicode is going to be deprecated   ####
#        if install_biicode; then
#            echo "biicode installed successfully"
#        else
#            install_biicode_client_dependencies
#            install_biicode_common_dependencies
#            install_biicode_server_dependencies
#            if [ $? -eq 0 ]; then        
#                echo "biicode installed successfully"
#            else
#                echo "error occured while installing biicode dependencies"
#                exit -1
#            fi
#        fi
}

install_boost() {
    if [ "${OS}" == "linux" ]; then
        DEBIAN_FRONTEND=noninteractive
        apt-get install -q -y bzip2 
    
    elif [ "${OS}" == "mac" ]; then
        if [[ ! is_xcode_clitools_installed ]]; then
            echo "xcode cli tools not installed\ninstall them prior to installing boost"
            return 1
        fi
    else
        echo "OS enivron variable not set\nEnsure your os is supported before retrying"
        exit -1
    fi
    
    cd /usr/local
    #                    this may take couple minutes to download                     #
    wget https://sourceforge.net/projects/boost/files/boost/1.63.0/boost_1_63_0.tar.bz2
    tar --bzip2 -xf boost_1_63_0.tar.bz2
    rm -r boost_1_63_0.tar.bz2
    cd boost_1_63_0
    ./bootstrap.sh --prefix=/usr/local && ./b2 install

    # add them to path #
    echo "" >> ~/.profile &&
    echo 'export PATH=$PATH:/usr/local/include:/usr/local/lib' >> ~/.profile
    source ~/.profile
    
    return 0
}

install_mysql() {
    DEBIAN_FRONTEND=noninteractive
    apt-get install -q -y libiodbc2 libiodbc2-dev libaio1
    # check if mysql is installed first #
    TEMP=$(dpkg -l "mysql-client" 2>&1 && | grep -i "no packages found")
    TEMP2=$(dpkg -l "mysql-server" 2>&1 && | grep -i "no packages found")
	if [[ -n "$TEMP" ]] && [[ -n "$TEMP2" ]]; then
		echo "installing mysql.."
        # Install MySQL Server in a Non-Interactive mode. Default root password will be "root"
        echo "mysql-server mysql-server/root_password password root" | debconf-set-selections
        echo "mysql-server mysql-server/root_password_again password root" | debconf-set-selections
        apt-get install -q -y -o Dpkg::Options::="--force-confnew" \
        -o Dpkg::Options::="--force-confdef" mysql-client mysql-server
        
        sed -i 's/127\.0\.0\.1/0\.0\.0\.0/g' /etc/mysql/my.cnf
        mysql -e "USE mysql; DELETE FROM mysql.db WHERE Db='test' OR Db='test\\_%'; DROP DATABASE test; DELETE FROM user WHERE User='root' AND Host NOT IN ('localhost', '127.0.0.1', '::1');"
        mysql -uroot -p -e 'USE mysql; UPDATE `user` SET `Host`="%" WHERE `User`="root" AND `Host`="localhost"; SET `Password`=PASSWORD("root") WHERE `User` = "root"; DELETE FROM `user` WHERE `Host` != "%" AND `User`="root"; FLUSH PRIVILEGES;'
        service mysql restart
    else
        echo "mysql is already installed..\n"
        return 1
    fi
    
    return 0
}

install_postgresql() {
    DEBIAN_FRONTEND=noninteractive
    apt-get install -q -y cmake postgresql \
        libpq-dev libpqxx-dev postgresql-server-dev-all
    update-rc.d -f postgresql remove # remove auto startup links
    
    # startup the server #
    service postgresql start
    service postgresql status
    sudo -u postgres psql postgres # logon to server
    \password postgres #set a root pw for postgres
    \q # logoff database
    
    sudo -u postgres createuser tmp # create db user "tmp"
    sudo -u postgres psql postgres # logon to server
    ALTER USER tmp WITH password 'pass';
    \q # logoff database
    sudo -u postgres createdb testdb --owner tmp # create a db
    
    pg_config --includedir # get loc of headers
    pg_config --libdir # get loc of libs
    pg_config --includedir-server # get loc of server header files
}

install_cJSON() {
    cd /opt
    git clone --recursive https://github.com/DaveGamble/cJSON.git
    cd cJSON
    mkdir build && cd build
    cmake .. -DENABLE_CJSON_TEST=On -DENABLE_CJSON_UTILS=On -DENABLE_SANITIZERS=On
    make && make install
}

install_yajl() {
cd /opt
  git clone --recursive https://github.com/lloyd/yajl.git
  cd yajl/
  ./configure && make install
}

####    Functions for installing Dependencies    ####
install_libffi() {
    cd /opt
    wget ftp://sourceware.org/pub/libffi/libffi-3.2.1.tar.gz
    tar -xvf libffi-3.2.1.tar.gz
    rm -r libffi-3.2.1.tar.gz
    cd libffi-3.2.1
    
    sed -e '/^includesdir/ s/$(libdir).*$/$(includedir)/' \
        -i include/Makefile.in &&

    sed -e '/^includedir/ s/=.*$/=@includedir@/' \
        -e 's/^Cflags: -I${includedir}/Cflags:/' \
        -i libffi.pc.in &&

    ./configure --prefix=/usr --disable-static && make && make install
    return 0
}

# for ssl support, OpenSSL must be installed prior to building #
install_libevent() {
    cd /opt
    git clone --recursive https://github.com/libevent/libevent.git
    cd libevent
    ./autogen.sh
    ./configure  --disable-malloc-replacement && make && make install
}

install_python3() {     # python 3.6 #
    cd /opt
    wget https://www.python.org/ftp/python/3.6.0/Python-3.6.0.tar.xz
    tar -xJf Python-3.6.0.tar.xz
    rm -r Python-3.6.0.tar.xz
    wget https://docs.python.org/3/archives/python-3.6.0-docs-html.tar.bz2
    tar --bzip2 -xf python-3.6.0-docs-html.tar.bz2
    
    cd Python-3.6.0
    
    CXX="/usr/bin/g++" \
    ./configure --prefix=/usr --enable-shared --with-system-expat --with-system-ffi --with-ensurepip=yes && make
    
    make install && chmod -v 755 /usr/lib/libpython3.6m.so && chmod -v 755 /usr/lib/libpython3.so
    
    cd python-3.6.0-docs-html
    sudo install -v -dm755 /usr/share/doc/python-3.6.0/html &&
    tar --strip-components=1 --no-same-owner --no-same-permissions \
        -C /usr/share/doc/python-3.6.0/html \
        -xvf ../python-3.6.0-docs-html.tar.bz2
    rm -r /opt/python-3.6.0-docs-html.tar.bz2
    
    ln -svfn python-3.6.0 /usr/share/doc/python-3
    export PYTHONDOCS=/usr/share/doc/python-3/html
}

install_openssl() {
    cd /opt
    wget https://www.openssl.org/source/openssl-1.1.0e.tar.gz
    tar -xvf openssl-1.1.0e.tar.gz
    rm -r openssl-1.1.0e.tar.gz
    cd openssl-1.1.0e
    
    ./config --prefix=/usr/local/bin --openssldir=/usr/local/bin shared  && make && make install
    make MANDIR=/usr/local/share/man MANSUFFIX=ssl install &&
    install -dv -m755 /usr/local/share/doc/openssl-1.1.0e  &&
    cp -vfr doc/*     /usr/local/share/doc/openssl-1.1.0e
}

install_libsodium() {
    cd /opt
    git clone https://github.com/jedisct1/libsodium.git --branch stable
    cd libsodium
    ./configure && make && make check && make install
}

#TODO: installation of crypto++ from source

# Note: you do not need to run install_openssl cmd if these packages install w/o error
install_poco() {
    cd /opt
    git clone --recursive https://github.com/pocoproject/poco.git
    DEBIAN_FRONTEND=noninteractive
    apt-get install -q -y openssl libssl-dev
    cd poco
    
    #TODO: installation of poco from source
}

# TODO: can't compile, getting build errors #
install_kore_io() {
    cd /opt
    git clone https://github.com/jorisvink/kore.git
    wget https://github.com/jorisvink/kore/releases/download/2.0.0-release/kore-2.0.0-release.tar.gz
    tar -xvf kore-2.0.0-release.tar.gz
    rm -r kore-2.0.0-release.tar.gz
    cd kore

    TASKS=1; PGSQL=1; DEBUG=1; NOTLS=1; NOHTTP=1; JSONRPC=1; NOOPT=1 #kore build settings
    if [ "$OS" == "linux" ]; then
        make && make install
    elif [ "$OS" == "mac" ]; then
        brew install kore
    else 
        echo "Operating System not supported at this time.."
        exit -1
    fi
}

install_ca_certs() {
    wget http://anduin.linuxfromscratch.org/BLFS/other/make-ca.sh-20170119
    wget http://anduin.linuxfromscratch.org/BLFS/other/certdata.txt #should be updated monthly
    sudo install -vm755 make-ca.sh-20170119 /usr/sbin/make-ca.sh
    /usr/sbin/make-ca.sh
    rm make-ca.sh-20170119 certdata.txt
    
     #        creating a openssl trusted cert from PEM encoded file            #
#    openssl x509 -in MyRootCA.pem -text -fingerprint -setalias "My Root CA 1" \
#        -addtrust serverAuth -addtrust emailProtection -addreject codeSigning \
#        > MyRootCA-trusted.pem
}

install_xcode_clitools() {
    if [ "$OSX_VERS" -ge 9 ]; then # on 10.9+, we can leverage SUS to get the latest CLI tools
        # create the placeholder file that's checked by CLI updates in Apple's SUS catalog
        touch /tmp/.com.apple.dt.CommandLineTools.installondemand.in-progress
        # find the CLI Tools update
        PROD=$(softwareupdate -l | grep "\*.*Command Line" | head -n 1 |
            awk -F"*" '{print $2}' | sed -e 's/^ *//' | tr -d '\n')
        softwareupdate -i "$PROD" -v # install it
        return 0

    else # on 10.7/10.8, we instead download from public download URLs
        [ "$OSX_VERS" -eq 7 ] && DMGURL=http://devimages.apple.com.edgekey.net/downloads/xcode/command_line_tools_for_xcode_os_x_lion_april_2013.dmg
        [ "$OSX_VERS" -eq 8 ] && DMGURL=http://devimages.apple.com.edgekey.net/downloads/xcode/command_line_tools_for_osx_mountain_lion_april_2014.dmg

        TOOLS=clitools.dmg
        curl "$DMGURL" -o "$TOOLS"
        TMPMOUNT=`/usr/bin/mktemp -d /tmp/clitools.XXXX`
        hdiutil attach "$TOOLS" -mountpoint "$TMPMOUNT"
        installer -pkg "$(find $TMPMOUNT -name '*.mpkg')" -target /
        hdiutil detach "$TMPMOUNT"
        rm -rf "$TMPMOUNT"
        rm "$TOOLS"
        return 0
    fi
    
    echo "xcode cli tools install failed"
    return 1
}

is_xcode_clitools_installed() {
    if  pkgutil --pkg-info com.apple.pkg.CLTools_Executables >/dev/null 2>&1; then
        printf '%s\n' "Checking for xcode cli tools"
        count=0
        pkgutil --files com.apple.pkg.CLTools_Executables |
        while IFS= read file; do
            test -e  "/${file}"   &&
            printf '%s\n' "/${file}…OK" || { printf '%s\n' "/${file}…MISSING"; ((count++)); }
        done
        
        if (( count > 0 )); then
            printf '%s\n' "Command Line Tools are not installed properly"
                # Provide instructions to remove and the CommandLineTools directory
                # and the package receipt then install instructions
            return 1
        else
            printf '%s\n' "Command Line Tools are installed"
            return 0
        fi
    else   
        printf '%s\n' "Command Line Tools are not installed"
           # Provide instructions to install the Command Line Tools
        return 1
    fi
}

accept_mac_eula() {
    /usr/bin/expect <<- EOF
        spawn sudo xcodebuild -license              
        expect {
            "*License.rtf" {
                send "\r";
            }
            timeout {
                send_user "\nExpect failed first expect\n";
                exit 1;
            }
        }
        expect {
            "*By typing 'agree' you are agreeing" {
                send "agree\r"; 
                send_error "\nUser agreed to EULA\n";
             }
             "*Press 'space' for more, or 'q' to quit*" {
                 send "q";
                 exp_continue;
             }
             timeout {
                 send_error "\nExpect failed second expect\n";
                 exit 1;
             }
        }
    expect eof

    foreach {pid spawnid os_error_flag value} [wait] break

    if {$os_error_flag == 0} {
        puts "exit status: $value"
        exit $value
    } 
    else {
        puts "errno: $value"
        exit $value
    }
EOF
return $?
}

####    Function for config biicode with Jetbrains Clion    ####
####                     **DEPRECATED**                     ####
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

resolve__bii_deps() {
    bii find
    bii configure
}

configure_biicode() {
    bii setup:cpp    
    cd / # change to <path to dir containing project>
    bii init $PROJECT_DIR -l=clion
    cd $PROJECT_DIR
    bii init <new library> -l=clion
    cd <new library>
    bii configure
}

####    Functions for installing broken biicode dependencies    ####
install_biicode_server_dependencies() {
    yes | pip install wsgiref passlib bottle bottle-memcache python-memcached newrelic gevent greenlet \
    gunicorn rq Delorean PyJWT mandrill mixpanel-py mixpanel-py-async google-measurement-protocol pycrypto \
    stripe colorama pylibmc
}

install_biicode_client_dependencies() {
    yes | pip install requests colorama jinja2 pyserial
}

install_biicode_common_dependencies() {
    yes | pip install pytz python-graph-core pyyaml ply pymongo
}

main "$@" # call main with cmd-line args

####    Misc Resources    ####

#                windows package for poco                 #
#https://pocoproject.org/releases/poco-1.7.8/poco-1.7.8.zip
#                              linux specific odbc driver for mysql                                 #
#https://dev.mysql.com/get/Downloads/Connector-C/mysql-connector-c-6.1.9-linux-glibc2.5-x86_64.tar.gz

#        completely unattended apt install cmds       #
#DEBIAN_FRONTEND=noninteractive && DEBIAN_PRIORITY=critical && 
#    apt-get install -q -y \
#    -o Dpkg::Options::="--force-confnew" \
#    -o Dpkg::Options::="--force-confdef" \
#    -o Dpkg::Options::="--force-confold" \
#    <package1> <package2> <package3>

