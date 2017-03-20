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

/* standard includes */
#include <stdio.h>
#include <string.h>
#include <iostream>

/* internal includes */
#include "library.h"

/* external includes */
#include "openssl/md5.h"
#include "sha.h"
#include "filters.h"
#include "hex.h"

int main() {
    CryptoPP::SHA1 sha1;
    std::string source = "SomeSuperSecretTestString";
    std::string hash = "";
    CryptoPP::StringSource(source, true, new CryptoPP::HashFilter(sha1, new CryptoPP::HexEncoder(new CryptoPP::StringSink(hash))));
    std::cout << hash;

#include <boost/lambda/lambda.hpp>
#include <iostream>
#include <iterator>
#include <algorithm>

    int main()
    {
        using namespace boost::lambda;
        typedef std::istream_iterator<int> in;

        std::for_each(
                in(std::cin), in(), std::cout << (_1 * 3) << " " );
    }
}