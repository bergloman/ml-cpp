/*
 * Copyright Elasticsearch B.V. and/or licensed to Elasticsearch B.V. under one
 * or more contributor license agreements. Licensed under the Elastic License;
 * you may not use this file except in compliance with the Elastic License.
 */
#include <test/CTestTmpDir.h>

#include <core/CLogger.h>

#include <boost/filesystem.hpp>

#include <errno.h>
#include <pwd.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

namespace ml {
namespace test {

std::string CTestTmpDir::tmpDir() {
    // Try to create a user-specific sub-directory of the temporary directory so
    // that multiple users sharing the same server don't clash.  However, if
    // this fails for any reason drop back to just raw /tmp.
    struct passwd pwd;
    ::memset(&pwd, 0, sizeof(pwd));
    static const size_t BUFSIZE(16384);
    char buffer[BUFSIZE] = {'\0'};
    struct passwd* result(nullptr);
    ::getpwuid_r(::getuid(), &pwd, buffer, BUFSIZE, &result);
    if (result == nullptr || result->pw_name == nullptr) {
        LOG_ERROR(<< "Could not get current user name: " << ::strerror(errno));
        return "/tmp";
    }

    std::string userSubdir("/tmp/");
    userSubdir += result->pw_name;

    try {
        // Prior existence of the directory is not considered an error by
        // boost::filesystem, and this is what we want
        boost::filesystem::path directoryPath(userSubdir);
        boost::filesystem::create_directories(directoryPath);
    } catch (std::exception& e) {
        LOG_ERROR(<< "Failed to create directory " << userSubdir << " - " << e.what());
        return "/tmp";
    }

    return userSubdir;
}
}
}
