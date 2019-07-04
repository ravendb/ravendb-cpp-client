#!/bin/bash
RVN_INSTALL_PREFIX=${PWD}/CppClientOut
RVN_GIT_PATH="https://github.com/ravendb/raven-cpp-client.git"
RVN_GIT_PATH="https://github.com/sashatim125/raven-cpp-client.git"
CMAKE_GIT_PATH="https://github.com/Kitware/CMake"
PKGS=( "apt-update" git gcc g++ libcurl4-openssl-dev libssl-dev make )

LOG=${PWD}/install_cpp_client.log
echo $(date) > ${LOG}
TMP_CMAKE="temp_cmake"
TMP_CLIENT="temp_client"

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

ERR_STRING="${C_L_RED}${T_BOLD}Error: ${NT}${C_L_RED}"

echo -e "${C_L_GREEN}${T_BOLD}Build RavenDB Cpp Client${NC}${NT}"
echo -e "${C_D_GRAY}========================${NC}"
echo ""
echo -e "${C_L_YELLOW}(Please enter sudo password, if asked to)${NC}${NT}"
sudo echo ""

function install_packages()
{
	HADERRORS=0
	for pkg in ${PKGS[@]}; do
		echo -en "${NC}[${C_GREEN}           ${NC}] ${C_L_CYAN}Installing ${C_L_MAGENTA}${pkg}${C_L_GRAY}...${NC}${NT}"
		echo -en "\r${NC}[${C_YELLOW}Working ...${NC}]"
		if [ ${pkg} == "apt-update" ]; then
			 sudo /bin/bash -c "apt update" >> ${LOG} 2>&1
		else
			 sudo /bin/bash -c "apt install -y ${pkg}" >> ${LOG} 2>&1
		fi
		STATUS=$?
		if [ ${STATUS} -eq 0 ]; then
			echo -e "\r${NC}[${C_GREEN} INSTALLED ${NC}]"
		else
			echo -e "\r${NC}[${C_RED} ERROR     ${NC}]"
			echo ""
			echo -e "${C_RED}Exit code ${STATUS}${NC}${NT}"
			HADERRORS=1
		fi
	done
	if [ ${HADERRORS} -eq 1 ]; then
		echo ""
		echo -e "${C_RED}Build Failed.${STATUS}${NC}${NT}"
		exit 1

	fi

	# verify gcc version >= 7.30
	GCCVER=$(g++ -v |& grep "gcc version" | awk '{print $3}')
	GCCVER_MAJOR=$(echo ${GCCVER} | cut -d'.' -f1)
	GCCVER_MINOR=$(echo ${GCCVER} | cut -d'.' -f2 | head -c 1)
	if [ ${GCCVER_MAJOR} -lt 8 ]; then
		if [ ${GCCVER_MAJOR} -lt 7 ] || [ ${GCCVER_MINOR} -lt 3 ]; then 
			echo -e "${C_RED}g++ (gcc) version must be >= 7.30${NC}"
			echo -e "${C_RED}Build Failed.${NC}"
			exit 1
		fi
	fi
}

# verify cmake >= 3.13
function install_cmake()
{
	echo -en "${NC}[${C_GREEN}           ${NC}] ${C_L_CYAN}Installing ${C_L_MAGENTA}cmake (>=3.13)${C_L_GRAY} (installation may take some time)...${NC}${NT}"
	CMAKE_VERSION="Version: 0"
		echo -en "\r${NC}[${C_YELLOW}Verifying..${NC}]"
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
		echo -e "\r${NC}[${C_GREEN} INSTALLED ${NC}] ${C_L_CYAN}Installing ${C_L_MAGENTA}cmake (>=3.13)${C_L_GRAY} (installation may take some time)...${NC} Already installed ${C_CYAN}${CMAKE_VERSION}${NT}"
	elif [ ${APT_VER_OK} -eq 1 ]; then
		echo -en "\r${NC}[${C_YELLOW}Working ...${NC}]"
		sudo /bin/bash -c "apt install -y cmake" >> ${LOG} 2>&1 &
		PID=$!
		while [ $(ps -ef | awk '{print $2}' | grep "^${PID}$" | wc -l) -ne 0 ]; do
			sleep 1
			STTLINE="$(tail -1 ${LOG})"
			STTLENG=$(echo $STTLINE | wc -c)
			if [ ${STTLENG} -lt 30 ]; then
				STTLINE="${STTLINE}$(printf "%-$(expr 30 - ${STTLENG})s")"
				STTLENG=30
			fi
			if [ ${STTLENG} -lt 60 ]; then
				STTDIFF=$(expr ${STTLENG} - 30)
				STTLINE="$(echo ${STTLINE} | head -c30)$(printf "%-$(expr 60 - ${STTLENG})s")$(echo ${STTLINE} | tail -c${STTDIFF})"
			fi
			echo -en "\r${NC}[${C_YELLOW}Working ...${NC}] ${C_L_CYAN}Installing ${C_L_MAGENTA}cmake (>=3.13)${C_L_GRAY} <$(echo ${STTLINE} | head -c30)...$(echo ${STTLINE} | tail -c30)> ${NC}"
		done
		wait ${PID}
		STATUS=$?
		echo -en "\r${NC}[${C_YELLOW}Finishing..${NC}] ${C_L_CYAN}Installing ${C_L_MAGENTA}cmake (>=3.13)${C_L_GRAY}$(printf "%-60s")${NC}"
		if [ ${STATUS} -eq 0 ]; then
			echo -e "\r${NC}[${C_GREEN} INSTALLED ${NC}]"
		else
			echo -e "\r${NC}[${C_RED} ERROR     ${NC}]"
			echo ""
			echo -e "${C_RED}Exit code ${STATUS}${NC}${NT}"
			echo ""
			echo -e "${C_RED}Build Failed.${STATUS}${NC}${NT}"
			exit 1
		fi
	else
		echo -en "\r${NC}[${C_YELLOW}Cloning ...${NC}]"
		if [ -d ${TMP_CMAKE} ]; then
			rm -rf ${TMP_CMAKE} >> ${LOG} 2>&1
		fi
		git clone ${CMAKE_GIT_PATH} ${TMP_CMAKE} >> ${LOG} 2>&1 &
		PID=$!
		while [ $(ps -ef | awk '{print $2}' | grep "^${PID}$" | wc -l) -ne 0 ]; do
			sleep 1
			STTLINE="$(tail -1 ${LOG})"
			STTLENG=$(echo $STTLINE | wc -c)
			if [ ${STTLENG} -lt 30 ]; then
				STTLINE="${STTLINE}$(printf "%-$(expr 30 - ${STTLENG})s")"
				STTLENG=30
			fi
			if [ ${STTLENG} -lt 60 ]; then
				STTDIFF=$(expr ${STTLENG} - 30)
                                STTLINE="$(echo ${STTLINE} | head -c30)$(printf "%-$(expr 60 - ${STTLENG})s")$(echo ${STTLINE} | tail -c${STTDIFF})"
			fi
			echo -en "\r${NC}[${C_YELLOW}Cloning ...${NC}] ${C_L_CYAN}Installing ${C_L_MAGENTA}cmake (>=3.13)${C_L_GRAY} <$(echo ${STTLINE} | head -c30)...$(echo ${STTLINE} | tail -c30)> ${NC}"
		done
		wait ${PID}
		STATUS=$?
		echo -en "\r${NC}[${C_YELLOW}Finishing..${NC}] ${C_L_CYAN}Installing ${C_L_MAGENTA}cmake (>=3.13)${C_L_GRAY}$(printf "%-60s")${NC}"
		if [ ${STATUS} -ne 0 ]; then
			echo -e "\r${NC}[${C_GREEN}   ERROR   ${NC}] ${C_L_CYAN}Installing ${C_L_MAGENTA}cmake (>=3.13)${C_L_GRAY}...${C_L_RED}Failed to git clone ${CMAKE_GIT_PATH} ${TMP_CMAKE}${NT}"
			echo -e "${C_RED}Build Failed.${STATUS}${NC}${NT}"
			exit 1
		fi
		echo -en "\r${NC}[${C_YELLOW}Bootstrap..${NC}]"
		pushd ${TMP_CMAKE} >> ${LOG} 2>&1
		./bootstrap >> ${LOG} 2>&1 &
		PID=$!
		while [ $(ps -ef | awk '{print $2}' | grep "^${PID}$" | wc -l) -ne 0 ]; do
			sleep 1
			STTLINE="$(tail -1 ${LOG})"
			STTLENG=$(echo $STTLINE | wc -c)
			if [ ${STTLENG} -lt 30 ]; then
				STTLINE="${STTLINE}$(printf "%-$(expr 30 - ${STTLENG})s")"
				STTLENG=30
			fi
			if [ ${STTLENG} -lt 60 ]; then
				STTDIFF=$(expr ${STTLENG} - 30)
                                STTLINE="$(echo ${STTLINE} | head -c30)$(printf "%-$(expr 60 - ${STTLENG})s")$(echo ${STTLINE} | tail -c${STTDIFF})"
			fi
			echo -en "\r${NC}[${C_YELLOW}Bootstrap..${NC}] ${C_L_CYAN}Installing ${C_L_MAGENTA}cmake (>=3.13)${C_L_GRAY} <$(echo ${STTLINE} | head -c30)...$(echo ${STTLINE} | tail -c30)> ${NC}"
		done
		wait ${PID}
		STATUS=$?
		echo -en "\r${NC}[${C_YELLOW}Finishing..${NC}] ${C_L_CYAN}Installing ${C_L_MAGENTA}cmake (>=3.13)${C_L_GRAY}$(printf "%-60s")${NC}"
		if [ ${STATUS} -ne 0 ]; then
			echo -e "\r${NC}[${C_RED} ERROR     ${NC}]"
			echo ""
			echo -e "${C_RED}Exit code ${STATUS}${NC}${NT}"
			echo ""
			echo -e "${C_RED}Build Failed.${STATUS}${NC}${NT}"
			popd >> ${LOG} 2>&1
			exit 1
		fi
		echo -en "\r${NC}[${C_YELLOW} make ...  ${NC}]"
		make >> ${LOG} 2>&1 &
		PID=$!
		while [ $(ps -ef | awk '{print $2}' | grep "^${PID}$" | wc -l) -ne 0 ]; do
			sleep 1
			STTLINE="$(tail -1 ${LOG})"
			STTLENG=$(echo $STTLINE | wc -c)
			if [ ${STTLENG} -lt 30 ]; then
				STTLINE="${STTLINE}$(printf "%-$(expr 30 - ${STTLENG})s")"
				STTLENG=30
			fi
			if [ ${STTLENG} -lt 60 ]; then
				STTDIFF=$(expr ${STTLENG} - 30)
                                STTLINE="$(echo ${STTLINE} | head -c30)$(printf "%-$(expr 60 - ${STTLENG})s")$(echo ${STTLINE} | tail -c${STTDIFF})"
			fi
			echo -en "\r${NC}[${C_YELLOW} make ...  ${NC}] ${C_L_CYAN}Installing ${C_L_MAGENTA}cmake (>=3.13)${C_L_GRAY} <$(echo ${STTLINE} | head -c30)...$(echo ${STTLINE} | tail -c30)> ${NC}"
		done
		wait ${PID}
		STATUS=$?
		echo -en "\r${NC}[${C_YELLOW}Finishing..${NC}] ${C_L_CYAN}Installing ${C_L_MAGENTA}cmake (>=3.13)${C_L_GRAY}$(printf "%-60s")${NC}"
		if [ ${STATUS} -ne 0 ]; then
			echo -e "\r${NC}[${C_RED} ERROR     ${NC}]"
			echo ""
			echo -e "${C_RED}Exit code ${STATUS}${NC}${NT}"
			echo ""
			echo -e "${C_RED}Build Failed.${STATUS}${NC}${NT}"
			popd >> ${LOG} 2>&1
			exit 1
		fi
		echo -en "\r${NC}[${C_YELLOW}Installing ${NC}]"
		sudo make install >> ${LOG} 2>&1
		STATUS=$?
		if [ ${STATUS} -ne 0 ]; then
			echo -e "\r${NC}[${C_RED} ERROR     ${NC}]"
			echo ""
			echo -e "${C_RED}Exit code ${STATUS}${NC}${NT}"
			echo ""
			echo -e "${C_RED}Build Failed.${STATUS}${NC}${NT}"
			popd >> ${LOG} 2>&1
			exit 1
		else
			echo -e "\r${NC}[${C_GREEN} INSTALLED ${NC}]"		
		fi
		popd >> ${LOG} 2>&1
		rm -rf ${TMP_CMAKE} >> ${LOG} 2>&1
	fi
}

# install ravendb client
function install_ravendb_clone()
{ 
	echo -en "${NC}[${C_GREEN}           ${NC}] ${C_L_CYAN}Installing ${C_L_MAGENTA}RavenDB Client...${NC}${NT}"
	echo -en "\r${NC}[${C_YELLOW}Cloning ...${NC}]"
	if [ -d ${TMP_CLIENT} ]; then
		rm -rf ${TMP_CLIENT} >> ${LOG} 2>&1
	fi
	git clone ${RVN_GIT_PATH} ${TMP_CLIENT} >> ${LOG} 2>&1 &
	PID=$!
	while [ $(ps -ef | awk '{print $2}' | grep "^${PID}$" | wc -l) -ne 0 ]; do
		sleep 1
		STTLINE="$(tail -1 ${LOG})"
		STTLENG=$(echo $STTLINE | wc -c)
		if [ ${STTLENG} -lt 30 ]; then
			STTLINE="${STTLINE}$(printf "%-$(expr 30 - ${STTLENG})s")"
			STTLENG=30
		fi
		if [ ${STTLENG} -lt 60 ]; then
			STTDIFF=$(expr ${STTLENG} - 30)
			STTLINE="$(echo ${STTLINE} | head -c30)$(printf "%-$(expr 60 - ${STTLENG})s")$(echo ${STTLINE} | tail -c${STTDIFF})"
		fi
		echo -en "\r${NC}[${C_YELLOW}Cloning ...${NC}] ${C_L_CYAN}Installing ${C_L_MAGENTA}RavenDB Client...${C_L_GRAY} <$(echo ${STTLINE} | head -c30)...$(echo ${STTLINE} | tail -c30)> ${NC}"
	done
	wait ${PID}
	STATUS=$?
	echo -en "\r${NC}[${C_YELLOW}Finishing..${NC}] ${C_L_CYAN}Installing ${C_L_MAGENTA}RavenDB Client...${C_L_GRAY}$(printf "%-60s")${NC}"
	if [ ${STATUS} -ne 0 ]; then
		echo -e "\r${NC}[${C_GREEN}   ERROR   ${NC}] ${C_L_CYAN}Installing ${C_L_MAGENTA}RavenDB Client...${C_L_RED}Failed to git clone ${RVN_GIT_PATH} ${TMP_CLIENT}${NT}"
		echo -e "${C_RED}Build Failed.${STATUS}${NC}${NT}"
		exit 1
	fi

	# TODO: to remove later
	#############################
	echo -en "\r${NC}[${C_YELLOW}Updating...${NC}]"
	pushd ${TMP_CLIENT} >> ${LOG} 2>&1
	git checkout WorkingBranch >> ${LOG} 2>&1
	STATUS=$?
	if [ ${STATUS} -ne 0 ]; then
		echo -e "\r${NC}[${C_RED} ERROR     ${NC}]"
		echo ""
		echo -e "${C_RED}Exit code ${STATUS}${NC}${NT}"
		echo ""
		echo -e "${C_RED}Build Failed.${STATUS}${NC}${NT}"
		popd >> ${LOG} 2>&1
		exit 1
	fi

	cat CMakeLists.txt | sed -e 's/choose if building tests/#choose if building tests/g' > CMakeLists.txt2
	mv CMakeLists.txt2 CMakeLists.txt
	popd >> ${LOG} 2>&1
	#############################
}

function install_ravendb_build()
{
	echo -en "${NC}[${C_GREEN}           ${NC}] ${C_L_CYAN}Installing ${C_L_MAGENTA}RavenDB Client...${NC}${NT}"
	echo -en "\r${NC}[${C_YELLOW}Preparing..${NC}]"
	pushd ${TMP_CLIENT}/cmake-build-release >> ${LOG} 2>&1
	cmake -DCMAKE_INSTALL_PREFIX=${RVN_INSTALL_PREFIX} -DCMAKE_BUILD_TYPE=Release .. >> ${LOG} 2>&1
	echo -en "\r${NC}[${C_YELLOW}Building...${NC}]"
	cmake --build . --target install >> ${LOG} 2>&1 &
	PID=$!
	while [ $(ps -ef | awk '{print $2}' | grep "^${PID}$" | wc -l) -ne 0 ]; do
		sleep 1
		STTLINE="$(tail -1 ${LOG})"
		STTLENG=$(echo $STTLINE | wc -c)
		if [ ${STTLENG} -lt 30 ]; then
			STTLINE="${STTLINE}$(printf "%-$(expr 30 - ${STTLENG})s")"
			STTLENG=30
		fi
		if [ ${STTLENG} -lt 60 ]; then
			STTDIFF=$(expr ${STTLENG} - 30)
                        STTLINE="$(echo ${STTLINE} | head -c30)$(printf "%-$(expr 60 - ${STTLENG})s")$(echo ${STTLINE} | tail -c${STTDIFF})"
		fi
		echo -en "\r${NC}[${C_YELLOW}Building...${NC}] ${C_L_CYAN}Installing ${C_L_MAGENTA}RavenDB Client...${C_L_GRAY} <$(echo ${STTLINE} | head -c30)...$(echo ${STTLINE} | tail -c30)> ${NC}"
	done
	wait ${PID}
	STATUS=$?
	echo -en "\r${NC}[${C_YELLOW}Finishing..${NC}] ${C_L_CYAN}Installing ${C_L_MAGENTA}RavenDB Client...${C_L_GRAY}$(printf "%-60s")${NC}"

	STATUS=$?
	if [ ${STATUS} -ne 0 ]; then
		echo -e "\r${NC}[${C_RED} ERROR     ${NC}]"
		echo ""
		echo -e "${C_RED}Exit code ${STATUS}${NC}${NT}"
		echo ""
		echo -e "${C_RED}Build Failed.${STATUS}${NC}${NT}"
		popd >> ${LOG} 2>&1
		exit 1
	fi
	popd >> ${LOG} 2>&1	
	echo -e "\r${NC}[${C_GREEN} INSTALLED ${NC}]"	
}

# testing ravendb client
function test_ravendb()
{
	echo -en "${NC}[${C_GREEN}           ${NC}] ${C_L_CYAN}Testing    ${C_L_MAGENTA}RavenDB Client...${NC}${NT}"
	echo -en "\r${NC}[${C_YELLOW}Compiling..${NC}]"
	pushd ${RVN_INSTALL_PREFIX} >> ${LOG} 2>&1
# ########### REMOVE LATER:
echo '#include <string>' > tmpfile.tmp
echo '#include <iostream>' >> tmpfile.tmp
echo '#include "RavenException.h"' >> tmpfile.tmp
echo '#include "GetCppClassName.h"' >> tmpfile.tmp
echo '#include "AllTopologyNodesDownException.h"' >> tmpfile.tmp
echo "" >> tmpfile.tmp
cat hello_world.cpp | sed -e 's/#include/\/\/ #include/' > hello_world.cpp_tmp
rm hello_world.cpp
mv tmpfile.tmp hello_world.cpp
cat hello_world.cpp_tmp >> hello_world.cpp
rm hello_world.cpp_tmp
# ############################
	g++ hello_world.cpp -std=c++1z -I ./include/ -I ./external/ -L ./lib -lRaven_CppClient -lstdc++fs -lcurl -lssl -lcrypto -lpthread -latomic -o testExec >> ${LOG} 2>&1
	STATUS=$?
	if [ ${STATUS} -ne 0 ]; then
		echo -e "\r${NC}[${C_RED} ERROR     ${NC}]"
		echo ""
		echo -e "${C_RED}Exit code ${STATUS}${NC}${NT}"
		echo ""
		echo -e "${C_RED}Build Failed.${STATUS}${NC}${NT}"
		exit 1
	fi
	export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${RVN_INSTALL_PREFIX}/lib
	echo -en "\r${NC}[${C_YELLOW}Testing....${NC}]"
	popd >> ${LOG} 2>&1
	pushd ${RVN_INSTALL_PREFIX} >> ${LOG} 2>&1
	./testExec >> ${LOG} 2>&1	
	STATUS=$?
	if [ ${STATUS} -ne 0 ]; then
		echo -e "\r${NC}[${C_RED} ERROR     ${NC}]"
		echo ""
		echo -e "${C_RED}Exit code ${STATUS}${NC}${NT}"
		echo ""
		echo -e "${C_RED}Build Failed.${STATUS}${NC}${NT}"
		exit 1
	fi
	echo -e "\r${NC}[${C_GREEN}  SUCCESS  ${NC}]"		
	popd >> ${LOG} 2>&1
	echo ""
	echo -e 
	echo -e "${C_GREEN}RavenDB C++ Client was ${C_L_GREEN}Successfully${C_GREEN} installed${NC}${NT}"
	echo ""
}


install_packages
install_cmake
install_ravendb_clone
install_ravendb_build
test_ravendb

