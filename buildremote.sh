#!/bin/bash
#
##########################################################################
# Clue Configuration Manager
#
# Custom build workflow to compile annd build binaries for Splash module
#
# @version $Id: buildremote.sh 1973 2018-03-31 20:17:44Z stefan $
##########################################################################
# Set global variables and execute initialization operation

DPLDIR=/opt/clue
SRCDIR=/var/clue/sources
RLSDIR=/var/clue/releases
export MODULE="splash"
##########################################################################


function help()
{
	echo "Usage: $0 [-rtmpa]"
	echo "    -r = remove completely target installation path"
	echo "    -m = create deployment target and run compiling process"
	echo "    -t = remove old target archive and create the new one"
	echo "    -p = install prerequisites"
	echo "    -d = download the source code"
	echo "    -c = cleaning compiling outputs"
	echo "    -a = execute pre-configuration before compiling"
	echo "    -i = install compiled binaries and resources"
	echo
}


function start()
{
	if [ "${MODULE}" != "" ]; then
		NAME=$(echo "${MODULE}" | tr '[:lower:]' '[:upper:]')

		echo ; echo "=============================================================================="
		echo "Start building $NAME package" ; echo
	fi
}


function stop()
{
	if [ "${MODULE}" != "" ]; then
		NAME=$(echo "${MODULE}" | tr '[:lower:]' '[:upper:]')

		echo ; echo "End of building $NAME package"
		echo "==============================================================================" ; echo
	fi
}


function banner()
{
	if [ "$1" != "" ] ; then
		echo ; echo "------------------------------------------------------------------------------"
		echo "$1" ; echo
	fi
}


function check()
{
	if [[ $? -ne 0 ]]; then
		echo
		echo "Builing process has been stopped because of error(s) met. Check the output messages or log file.." ;
		exit $?
	fi
}


function runPreparation()
{
	if [[ "$1" = -*r* ]]; then
		rm -rf $DPLDIR
	fi
	if [[ "$1" = -*m* ]] || [ "$1" = "" ] ; then
		if [ ! -d $DPLDIR ]; then
			mkdir -p $DPLDIR
		fi
	fi
	if [[ "$1" = -*t* ]] || [ "$1" = "" ] ; then
		rm -rf ${RLSDIR}/${MODULE}.tar.gz
		if [ ! -d ${RLSDIR} ]; then
			mkdir -p ${RLSDIR}
		fi
	fi
}


function runPrerequisites()
{
	if [[ "$1" = -*p* ]] || [ "$1" = "" ] ; then
		banner "Installing prerequisites.."
		/opt/clue/bin/setup -s update -r expand
		check

		apt-get install -y build-essential autoconf autopoint libtool pkg-config cmake gcc g++ gcc-4.9 g++-4.9
		apt-get install -y libraspberrypi-dev linux-headers-$(uname -r)
		apt-get install -y git subversion wget curl tar zip unzip

		prerequisites
		/opt/clue/bin/setup -s update -r compact
	fi
}


function runDownload()
{
	if [[ "$1" = -*d* ]] || [ "$1" = "" ] ; then
		banner "Downloading source code.."
		if [ ! -d ${SRCDIR} ]; then mkdir -p ${SRCDIR}; fi

		if [ -d ${SRCDIR}/${MODULE} ]; then
			cd ${SRCDIR}/${MODULE}
			svn update
		else
			cd ${SRCDIR}
			svn checkout svn://amsd.go.ro/rootsvn/HMD.Clue/clue-1/modules/${MODULE}/src ${MODULE}
			/usr/bin/find ${SRCDIR}/${MODULE} -type f -exec chmod +x {} \;
		fi
	fi
}


function runClean()
{
	if [[ "$1" = -*c* ]] ; then
		banner "Cleaning environment.."
		clean
	fi
}


function runConfiguration()
{
	if [[ "$1" = -*a* ]] || [ "$1" = "" ] ; then
		banner "Configuring environment.."
		configure
	fi
}


function runMake()
{
	BUILDTHREADS=${BUILDTHREADS:-$(grep -c "^processor" /proc/cpuinfo)}
	[ ${BUILDTHREADS} -eq 0 ] && BUILDTHREADS=1
	export BUILDTHREADS

	if [[ "$1" = -*m* ]] || [ "$1" = "" ] ; then
		banner "Compiling source code.."
		make
	fi
}


function runMakeInstall()
{
	if [[ "$1" = -*i* ]] || [[ "$1" = -*m* ]] || [ "$1" = "" ] ; then

		if [[ "$1" != -*i* ]]; then
			if [[ "$1" = -*t* ]] || [ -d ${DPLDIR} ]; then
				mv ${DPLDIR} ${DPLDIR}-bak
			fi
		fi

		banner "Deploying binaries.."
		makeinstall
	fi
}


function runTar()
{
	if [[ "$1" != -*i* ]]; then
		if [[ "$1" = -*t* ]] || [ "$1" = "" ] ; then

			banner "Writing binary package.."
			/bin/tar -zcvf ${RLSDIR}/${MODULE}.tar.gz ${DPLDIR}

			if [ -d ${DPLDIR}-bak ]; then
				rm -rf ${DPLDIR}
				mv ${DPLDIR}-bak ${DPLDIR}
			fi
		fi
	fi
}


function run()
{
	start

	if [ "$1" = "-h" ] || [ "$1" = "--help" ]; then
		help
		exit 0
	fi

	runPreparation $1
	runPrerequisites $1
	check

	CURDIR=`pwd`
	runDownload $1
	check

	if [ ! -d ${SRCDIR}/${MODULE} ]; then
		echo -e "\tError: Package location doesn't exist: ${SRCDIR}/${MODULE}\n"
		exit 1
	fi

	cd ${SRCDIR}/${MODULE}
	runClean $1
	check

	cd ${SRCDIR}/${MODULE}
	runConfiguration $1
	check

	cd ${SRCDIR}/${MODULE}
	runMake $1
	check

	cd ${SRCDIR}/${MODULE}
	runMakeInstall $1
	check

	cd ${SRCDIR}/${MODULE}
	runTar $1
	check

	cd ${CURDIR}
	stop
}
##########################################################################
# Implement specific functions


function prerequisites()
{
	apt-get install -y libpng-dev libjpeg-dev libfreetype6-dev uthash-dev

	## Stop service and make it to not run automatically
	/opt/clue/bin/setup -s service -m "clue-splash"
}


function clean()
{
	if [ -f ${SRCDIR}/${MODULE}/Makefile ]; then
		/usr/bin/make clean
	fi
}


function configure()
{
	#nothing to do
	echo "Nothing to configure additionally.."
}


function make()
{
	/usr/bin/make -j ${BUILDTHREADS}
}


function makeinstall()
{
	/usr/bin/make install
}

##########################################################################
run "$@" 2>&1 | tee /tmp/${MODULE}.log
##########################################################################