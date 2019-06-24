#!/usr/bin/env bash

RAVEN_CPP_CLIENT_HEADER=raven_cpp_client.h

printf "#pragma once\n\n"> $RAVEN_CPP_CLIENT_HEADER
find . | grep "\.h$" | grep -v $RAVEN_CPP_CLIENT_HEADER | sed 's/^\.\//#include \"/' | sed 's/\.h$/\.h\"/' >> ${RAVEN_CPP_CLIENT_HEADER}
