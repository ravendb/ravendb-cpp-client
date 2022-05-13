#!/bin/bash
if [ "${PWD}" == "/" ]; then
	CURDIR=""
else
	CURDIR="${PWD}"
fi
RVN_INSTALL_PREFIX=${CURDIR}/CppClientOut
RVN_GIT_PATH="https://github.com/ravendb/raven-cpp-client.git"
CMAKE_GIT_PATH="https://github.com/Kitware/CMake"
PKGS=( git gcc g++ libcurl4-openssl-dev libssl-dev xxhash make libcurl4 zip unzip tar)
LOG=${CURDIR}/~build.log
echo $(date) > ${LOG}
TMP_CMAKE="temp_cmake"
TMP_CLIENT=${PWD}
BUILDDIR="cmake-build-release"
PERFORM_APT_UPGRADE=0
SUDO="sudo"
CLONE_RAVENDB=0
function print_usage() {
	echo "Usage: ${0} [ OPTIONS ]"
	echo "       OPTIONS:"
	echo "               --apt-upgrade                       perform apt-upgrade -y (non interactive with default confirmation)"
	echo "               --skip-sudo                         perform super user actions without sudo command"
	echo "               --clone-ravendb                     if script is running outside ravendb client clone, use this option"
	echo "               --install-dir=<CppClient out dir>   specify artifact directory (default: CppClientOut)"
	echo ""
}
if [ $# -gt 0 ]; then
	for arg in $@; do
		if [ ${arg} == "--apt-upgrade" ]; then
			PERFORM_APT_UPGRADE=1
		elif [ ${arg} == "--skip-sudo" ]; then
			SUDO=""
		elif [ ${arg} == "--clone-ravendb" ]; then
			CLONE_RAVENDB=1
			TMP_CLIENT="temp_client"
		elif [ ${arg} == "-h" ] || [ ${arg} == "--help" ]; then
			print_usage
			exit 0
		elif [ "$(echo -n ${arg} | cut -f1 -d'=')" == "--install-dir" ]; then
			RVN_INSTALL_PREFIX="$(echo -n ${arg} | cut -f2 -d'=')"
		else
			echo "Error: Invalid argument '${arg}'"
			print_usage
			exit 1
		fi
	done
fi

NC="\\e[39m"
C_BLACK="\\e[30m"
C_RED="\\e[31m"
C_GREEN="\\e[32m"
C_YELLOW="\\e[33m"
C_BLUE="\\e[34m"
C_MAGENTA="\\e[35m"
C_CYAN="\\e[36m"
C_L_GRAY="\\e[37m"
C_D_GRAY="\\e[90m"
C_L_RED="\\e[91m"
C_L_GREEN="\\e[92m"
C_L_YELLOW="\\e[93m"
C_L_BLUE="\\e[94m"
C_L_MAGENTA="\\e[95m"
C_L_CYAN="\\e[96m"
C_WHITE="\\e[97m"

NT="\\e[0m"
T_BOLD="\\e[1m"
T_UL="\\e[4m"
T_BLINK="\\e[5m"

function print_state_installed() { echo -e "\r${NC}[${C_GREEN} INSTALLED ${NC}] ${1} ${C_GREEN}${2}${NT}${NC}"
} 
function print_state_error() { echo -e "\r${NC}[${C_RED} ERROR     ${NC}] ${1} ${C_RED}${2}${NT}${NC}" 
}
function print_state_msg() { echo -en "\r${NC}[${C_YELLOW}${2}${NC}] ${1}${NT}${NC}" 
}
function print_build_failed() { echo -e "${C_RED}Build Failed.${STATUS}${NC}${NT}" 
}

function progress_show_and_wait()
{
	STRFIXLEN=60
	STRHALF=$(expr ${STRFIXLEN} / 2 - 2)
	while [ $(ps -ef | awk '{print $2}' | grep "^${1}$" | wc -l) -ne 0 ]; do
		STRLINE="$(tail -1 ${LOG})"
		STRLINE="$(echo -n ${STRLINE} | tr --delete '\n')"
		STRLENG=$(echo -n $STRLINE | wc -c)
		if [ ${STRLENG} -lt ${STRFIXLEN} ]; then
			STRLINE="${STRLINE}$(printf %-$(expr 60 - ${STRLENG})s)"
		elif [ ${STRLENG} -gt ${STRFIXLEN} ]; then
			STRLINE="$(echo -n ${STRLINE} | head -c${STRHALF})....$(echo -n ${STRLINE} | tail -c${STRHALF})"
		fi
		print_state_msg "${3}${C_WHITE} <${STRLINE}>" "${2}"
		sleep 1
	done
	print_state_msg "${3}${C_L_GRAY}  $(printf %-${STRFIXLEN}s)  " "${2}"
	wait ${1}
	STATUS=$?
}

function print_welcome_and_test_sudo()
{
	echo -e "${C_L_GREEN}${T_BOLD}Build RavenDB Cpp Client${NC}${NT}"
	echo -e "${C_D_GRAY}========================${NC}"
	echo ""
	echo "Logging into file: ${LOG}"
	if [ ${PERFORM_APT_UPGRADE} -eq 1 ]; then
		echo -e "${T_BLINK}Note: 'apt-get upgrade' will be performed${NT}${NC}"
	else
		echo "Note: 'apt-get upgrade' will be skipped. Run again with --apt-upgrade to include upgrade operation."
	fi
	if [ "${SUDO}" == "" ]; then
		echo "Note: Skipping sudo command"
	else
		echo -e "${C_L_YELLOW}(Please enter sudo password, if asked to)${NC}${NT}"
	fi
	${SUDO} echo ""
}

function install_packages()
{
	MAINSTR="${C_L_CYAN}Performing ${C_L_MAGENTA}apt-get update${C_L_GRAY}..."
	${SUDO} /bin/bash -c "apt-get update" >> ${LOG} 2>&1 &
        PID=$!
	progress_show_and_wait ${PID} "Updating..." "${MAINSTR}"
	if [ ${STATUS} -eq 0 ]; then
		print_state_installed "${MAINSTR}" "Successfully updated."
	else
		print_state_error "${MAINSTR}" "Exit code ${STATUS}"
		print_build_failed
		exit 1
	fi

	if [ ${PERFORM_APT_UPGRADE} -eq 1 ]; then
		MAINSTR="${C_L_CYAN}Performing ${C_L_MAGENTA}apt-get upgrade -y (non interactive)${C_L_GRAY}..."
		${SUDO} /bin/bash -c "DEBIAN_FRONTEND=noninteractive apt-get upgrade -y" >> ${LOG} 2>&1 &
		PID=$!
		progress_show_and_wait ${PID} "Upgrading.." "${MAINSTR}"
		if [ ${STATUS} -eq 0 ]; then
			print_state_installed "${MAINSTR}" "Successfully upgraded."
		else
			print_state_error "${MAINSTR}" "Exit code ${STATUS}"
			print_build_failed
			exit 1
		fi
	fi

	HADERRORS=0
	for pkg in ${PKGS[@]}; do
		MAINSTR="${C_L_CYAN}Installing ${C_L_MAGENTA}${pkg}${C_L_GRAY}..."
		print_state_msg "${MAINSTR}" "Install ..."
		${SUDO} /bin/bash -c "DEBIAN_FRONTEND=noninteractive apt-get install -y ${pkg}" >> ${LOG} 2>&1
		STATUS=$?
		if [ ${STATUS} -eq 0 ]; then
			print_state_installed "${MAINSTR}" "Successfully installed."
		else
			print_state_error "${MAINSTR}" "Exit code ${STATUS}"
			HADERRORS=1
		fi
	done
	if [ ${HADERRORS} -eq 1 ]; then
		print_build_failed
		exit 1
	fi

	# verify gcc version >= 7.30
	MAINSTR="${C_L_CYAN}Verifying  ${C_L_MAGENTA}g++ (gcc) version must be >= 7.30${C_L_GRAY}..."
	print_state_msg "${MAINSTR}" "Verifying.."
	GCCVER=$(g++ -v |& grep "gcc version" | awk '{print $3}')
	GCCVER_MAJOR=$(echo ${GCCVER} | cut -d'.' -f1)
	GCCVER_MINOR=$(echo ${GCCVER} | cut -d'.' -f2 | head -c 1)
	if [ ${GCCVER_MAJOR} -lt 8 ]; then
		if [ ${GCCVER_MAJOR} -lt 7 ] || [ ${GCCVER_MINOR} -lt 3 ]; then 
			print_state_error "${MAINSTR}" "${C_RED}Found: ${GCCVER}"
			print_build_failed
			exit 1
		fi
	fi
	print_state_installed "${MAINSTR}" "Found: ${GCCVER}"
}

# verify cmake >= 3.13
function install_cmake()
{
	MAINSTR="${C_L_CYAN}Installing ${C_L_MAGENTA}cmake (>=3.13)${C_L_GRAY} (installation may take some time)..."
	CMAKE_VERSION="Version: 0"
	print_state_msg "${MAINSTR}" "Verifying.."
	if [ $(which cmake | wc -l) -gt 0 ]; then
		CMAKE_VERSION="$(cmake --version | grep -i version | sed 's/version/Version:/g' | cut -d' ' -f2-999)"
	fi
	VNUM_MAJOR=$(echo ${CMAKE_VERSION} | cut -f2 -d' ' | cut -f1 -d'-' | cut -f1 -d'.')
	VNUM_MINOR=$(echo ${CMAKE_VERSION} | cut -f2 -d' ' | cut -f1 -d'-' | cut -f2 -d'.')
	VER_OK=0
	APT_VER_OK=0
	if [ ${VNUM_MAJOR} -gt 2 ]; then
		if [ ${VNUM_MINOR} -gt 12 ]; then
			VER_OK=1
		fi
	fi	
	if [ ${VER_OK} -eq 0 ]; then
		CMAKE_VERSION="$(apt-cache show cmake | grep Version:)"
		VNUM_MAJOR=$(echo ${CMAKE_VERSION} | cut -f2 -d' ' | cut -f1 -d'-' | cut -f1 -d'.')
		VNUM_MINOR=$(echo ${CMAKE_VERSION} | cut -f2 -d' ' | cut -f1 -d'-' | cut -f2 -d'.')
		
		if [ ${VNUM_MAJOR} -gt 2 ]; then
			if [ ${VNUM_MINOR} -gt 12 ]; then
				APT_VER_OK=1
			fi
		fi
	fi

	if [ ${VER_OK} -eq 1 ]; then
		print_state_installed "${MAINSTR}" "Already installed ${C_CYAN}${CMAKE_VERSION}"
	elif [ ${APT_VER_OK} -eq 1 ]; then
		${SUDO} /bin/bash -c "apt-get install -y cmake" >> ${LOG} 2>&1 &
		PID=$!
		progress_show_and_wait ${PID} "Working ..." "${MAINSTR}" 
		if [ ${STATUS} -eq 0 ]; then
			print_state_installed "${MAINSTR}" "Successfully installed via apt."
		else
			print_state_error "${MAINSTR}" "Exit code ${STATUS}"
			print_build_failed
			exit 1
		fi
	else
		print_state_msg "${MAINSTR}" "Cleaning..."
		if [ -d ${TMP_CMAKE} ]; then
			rm -rf ${TMP_CMAKE} >> ${LOG} 2>&1
		fi
		git clone ${CMAKE_GIT_PATH} ${TMP_CMAKE} >> ${LOG} 2>&1 &
		PID=$!
		progress_show_and_wait ${PID} "Cloning ..." "${MAINSTR}" 
		if [ ${STATUS} -ne 0 ]; then
			print_state_error "${MAINSTR}" "Failed to git clone ${CMAKE_GIT_PATH} ${TMP_CMAKE}"
			print_build_failed
			exit 1
		fi
		pushd ${TMP_CMAKE} >> ${LOG} 2>&1
		./bootstrap >> ${LOG} 2>&1 &
		PID=$!
		progress_show_and_wait ${PID} "Bootstrap.." "${MAINSTR}" 
		if [ ${STATUS} -ne 0 ]; then
			print_state_error "${MAINSTR}" "Exit code ${STATUS}"
                        print_build_failed
			popd >> ${LOG} 2>&1
			exit 1
		fi
		make >> ${LOG} 2>&1 &
		PID=$!
		progress_show_and_wait ${PID} "Make ...   " "${MAINSTR}"
		if [ ${STATUS} -ne 0 ]; then
			print_state_error "${MAINSTR}" "Exit code ${STATUS}"
                        print_build_failed
			popd >> ${LOG} 2>&1
			exit 1
		fi
		${SUDO} make install >> ${LOG} 2>&1 &
		PID=$!
		progress_show_and_wait ${PID} "Install ..." "${MAINSTR}"
		if [ ${STATUS} -ne 0 ]; then
			print_state_error "${MAINSTR}" "Exit code ${STATUS}"
                        print_build_failed
			popd >> ${LOG} 2>&1
			exit 1
		else
			print_state_installed "${MAINSTR}" "Successfully installed via make."
		fi
		popd >> ${LOG} 2>&1
		rm -rf ${TMP_CMAKE} >> ${LOG} 2>&1
	fi
}

# install ravendb client
function install_ravendb_clone()
{ 
	MAINSTR="Cloning ${C_L_MAGENTA}RavenDB Client..."
	print_state_msg "${MAINSTR}" "Cleaning..."
	if [ -d ${TMP_CLIENT} ]; then
		rm -rf ${TMP_CLIENT} >> ${LOG} 2>&1
	fi
	git clone ${RVN_GIT_PATH} ${TMP_CLIENT} >> ${LOG} 2>&1 &
	PID=$!
	progress_show_and_wait ${PID} "Cloning ..." "${MAINSTR}" 
	if [ ${STATUS} -ne 0 ]; then
		print_state_error "${MAINSTR}" "Failed to git clone ${RVN_GIT_PATH} ${TMP_CLIENT}"
		print_build_failed
		exit 1
	fi
}

function install_ravendb_build()
{
	MAINSTR="${C_L_CYAN}Installing ${C_L_MAGENTA}RavenDB Client..."
	print_state_msg "${MAINSTR}" "Preparing.."
	mkdir -p ${TMP_CLIENT}/${BUILDDIR}
	pushd ${TMP_CLIENT}/${BUILDDIR} >> ${LOG} 2>&1
	cmake -DCMAKE_INSTALL_PREFIX=${RVN_INSTALL_PREFIX} -DCMAKE_BUILD_TYPE=Release .. >> ${LOG} 2>&1
	cmake --build . --target install >> ${LOG} 2>&1 &
	PID=$!
	progress_show_and_wait ${PID} "Building..." "${MAINSTR}" 
	if [ ${STATUS} -ne 0 ]; then
		print_state_error "${MAINSTR}" "Exit code ${STATUS}"
                print_build_failed
		popd >> ${LOG} 2>&1
		exit 1
	fi
	popd >> ${LOG} 2>&1	
	print_state_installed "${MAINSTR}" "Successfully installed at ${RVN_INSTALL_PREFIX}"
}

# testing ravendb client
function test_ravendb()
{
	MAINSTR="${C_L_CYAN}Testing    ${C_L_MAGENTA}RavenDB Client..."
	print_state_msg "${MAINSTR}" "Compiling.."
	pushd ${RVN_INSTALL_PREFIX} >> ${LOG} 2>&1

	g++ hello_world.cpp -O2 -std=c++1z -I ./include/ -I ./external/ -L ./lib -lRaven_CppClient -lstdc++fs -lcurl -lssl -lcrypto -lpthread -latomic -o testExec >> ${LOG} 2>&1 &
	PID=$!
	progress_show_and_wait ${PID} "Compiling.." "${MAINSTR}" 
	if [ ${STATUS} -ne 0 ]; then
		print_state_error "${MAINSTR}" "Exit code ${STATUS}"
                print_build_failed
		popd >> ${LOG} 2>&1
		exit 1
	fi
	export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${RVN_INSTALL_PREFIX}/lib
	print_state_msg "${MAINSTR}" "Testing...."
	./testExec >> ${LOG} 2>&1	
	STATUS=$?
	if [ ${STATUS} -ne 0 ]; then
		print_state_error "${MAINSTR}" "Exit code ${STATUS}"
                print_build_failed
                popd >> ${LOG} 2>&1
		exit 1
	fi
	print_state_installed "${MAINSTR}" "Test passed successfully."
	popd >> ${LOG} 2>&1
	echo ""
	echo -e 
	echo -e "${C_GREEN}RavenDB C++ Client was ${C_L_GREEN}Successfully${C_GREEN} installed at ${NC}${NT} ${RVN_INSTALL_PREFIX}"
	echo ""
}

print_welcome_and_test_sudo
install_packages
install_cmake
if [ ${CLONE_RAVENDB} -eq 1 ]; then
	install_ravendb_clone
fi
install_ravendb_build
test_ravendb

