#!/bin/sh
##########################################################################################
# Clue Splash Service - Dedicated script and API to run the screen content for all splash
# use-cases. This script drives the installer process and also the OS status in case MCPi
# package was not installed.
##########################################################################################

SETUPTOOL="/opt/clue/bin/setup"
SPLASHBIN="/opt/clue/bin/splash"
SPLASHSVC="/opt/clue/share/splash/service.sh"

setup=n
splash=n

test -x ${SPLASHBIN} || exit 1
test -x ${SPLASHSVC} || exit 1
test -x ${SETUPTOOL} || exit 1

for x in $(cat /proc/cmdline); do
	case $x in
		splash)
			splash=y
			;;
		setup)
			setup=y
			;;
	esac
done

##########################################################################################

ttl()
{
	local version=$(${SETUPTOOL} -g propcfg VERSION_ID /etc/os-release)
	version=$(echo "${version}" | sed -r 's/[" ]//g')

	if [ $# -eq 2 ]; then
		${SPLASHBIN} --image=splash --console --reset
		${SPLASHBIN} --message="Clue OS v${version}" --xpoint=-2021 -y5 --head
		${SPLASHBIN} --message="${2}" -x5 -y5 --keep
		${SPLASHBIN} --shape=progressbar --ypoint=-34 --properties="width=500,height=50,linewidth=4,percent=${1}" --keep
		${SPLASHBIN} --shape=hline --xpoint=1 --ypoint=-67 --properties="x2point=-2" --keep
		export barctrl=1
	elif [ $# -eq 1 ]; then
		${SPLASHBIN} --image=splash --console --reset
		${SPLASHBIN} --message="Clue OS v${version}" --xpoint=-2021 -y5 --head
		${SPLASHBIN} --shape=hline --xpoint=1 --ypoint=-67 --properties="x2point=-2" --keep
		${SPLASHBIN} --message="${1}" --ypoint=-34 --keep
		unset barctrl
	fi

	sleep 1
}


end()
{
	if [ "$1" != "" ]; then
		${SPLASHBIN} --message="${1}" --xpoint=10 --ypoint=-89 --keep
	else
		${SPLASHBIN} --message="Execution complete!" --xpoint=10 --ypoint=-89 --keep
	fi

	[ -z $barctrl ] || ${SPLASHBIN} --shape=progressbar --ypoint=-34 --properties="width=500,height=50,linewidth=4,percent=-100"
	unset barctrl

	local waitime=10
	while [ $waitime -gt 0 ]; do
		${SPLASHBIN} --message="> Reboot in ${waitime} seconds" --xpoint=50 --ypoint=-1213 --properties="fontcolor=#ffffff"
		waitime=$((waitime-1))
		sleep 1
	done

	${SPLASHBIN} --message="> Reboot now" --xpoint=50 --ypoint=-1213 --properties="fontcolor=#96c2e5"
	sleep 1 && reboot
}


msg()
{
	${SPLASHBIN} --message="${1}" --xpoint=10 --ypoint=-89 --properties="fontcolor=#96c2e5"

	sleep 1
}


err()
{
	[ -z $barctrl ] || ${SPLASHBIN} --shape=progressbar --ypoint=-34 --properties="width=500,height=50,linewidth=4" --wipe
	${SPLASHBIN} --message="${1}" --xpoint=10 --ypoint=-89 --properties="fontcolor=#cc002f" --keep

	unset barctrl
	sleep 1
	exit 1
}


bar()
{
	if [ -z $barctrl ]; then
		${SPLASHBIN} --shape=progressbar --ypoint=-34 --properties="width=500,height=50,linewidth=4,percent=${1}"
	else
		${SPLASHBIN} --shape=progressbar --ypoint=-34 --properties="width=500,height=50,linewidth=4,percent=-${1}"
	fi
}

##########################################################################################

doSetup()
{
	if [ -f /var/.expandfs ]; then
		ttl "System Setup"

		msg "Resizing data partition.."
		${SETUPTOOL} -s filesys -ee || err
		msg "Creating temporary service for file system resizing.."
		${SETUPTOOL} -s filesys -es || err

		rm -rf /var/.expandfs
		end "File system resizing is complete!"
	else
		ttl 0 "System Setup"
		local IC=$(${SETUPTOOL} -g network -ic)

		if [ "${IC}" = "1" ]; then
			msg "Expanding software repository.."
			${SETUPTOOL} -s update -re || err
			bar 5

			msg "Updating software catalog.."
			local PACKS="$(${SETUPTOOL} -g update -p)"
			bar 15

			if [ "${PACKS}" != "" ]; then
				msg "Upgrading packages: ${PACKS}"
				/usr/bin/apt-get upgrade -y -o Dpkg::Options::="--force-confdef" -o Dpkg::Options::="--force-confold" >/dev/null 2>&1 || err
			else
				msg "No updates found!" && sleep 3
			fi
			bar 30

			msg "Compacting software repository.."
			${SETUPTOOL} -s update -rc || err
			bar 35

			msg "Updating software catalog.."
			${SETUPTOOL} -s update -ru || err
			bar 40
		else
			msg "No Internet access, system upgrade is skipped.." && sleep 3
			bar 40
		fi

		msg "Creating cache to the recent shared libraries.."
		ldconfig || err
		bar 45

		msg "Creating SSH keys.."
		if [ ! -d /root/.ssh ]; then
			/usr/bin/ssh-keygen -t rsa -b 2048 -N "" -f /root/.ssh/id_rsa >/dev/null 2>&1 || err
		fi
		bar 55

		msg "Updating Initial RAM disk image.."
		sed -i "s/ setup//g" /boot/cmdline.txt
		update-initramfs -u >/dev/null 2>&1
		bar 80

		msg "Cleaning environment.."
		apt-get --yes --purge autoremove >/dev/null 2>&1 || err
		apt-get --yes autoclean >/dev/null 2>&1 || err
		apt-get --yes clean >/dev/null 2>&1 || err
		rm -rf /var/log/*
		rm -rf /tmp/*
		bar 95

		end "Installation complete!"
	fi
}


doInfo()
{
	local ipaddr="N/A"
	if [ $(/sbin/ifconfig | /bin/grep -c "Bcast") -ne 0 ]; then
		ipaddr=`/sbin/ifconfig | /bin/grep "Bcast" | /usr/bin/cut -d ":" -f 2 | /usr/bin/cut -d " " -f 1`
		ipaddr=$(echo "$ipaddr" | tr "\n" " ")
	elif [ $(/sbin/ifconfig | /bin/grep -c "broadcast") -ne 0 ]; then
		ipaddr=`/sbin/ifconfig | /bin/grep "broadcast" | sed -e 's/^[ \t]*//' | /usr/bin/cut -d " " -f 2`
		ipaddr=$(echo "$ipaddr" | tr "\n" " ")
	fi

	local sysver=$(${SETUPTOOL} -g propcfg VERSION_ID /etc/os-release)
	sysver=$(echo "${sysver}" | sed -r 's/[" ]//g')

	local services=""
	IFS=$'|'
	for file in $(${SETUPTOOL} -g services -l); do
		case $file in
			clue-*)
				if [ "${file}" != "clue-splash" ]; then
					local sign="-"
					local sname=$(echo "$file" | cut -f2 -d"-")
					local status=$(${SETUPTOOL} -g service -se $sname)
					if [ "$1" = "-s" -a "$2" = "$sname" -a "$3" = "stop" ]; then
						sign="-"
					elif [ "${status}" = "running" ] || [ "$1" = "-s" -a "$2" = "$sname" -a "$3" = "start" ]; then
						sign="+"
					fi
					if [ "${services}" = "" ]; then
						services="${sign}${sname}"
					else
						services="${services} ${sign}${sname}"
					fi
				fi
				;;
			*)
				continue
				;;
		esac
	done

	${SPLASHBIN} --image=splash --console --reset
	${SPLASHBIN} --message="Clue OS v${sysver}" --xpoint=-2021 -y5 --head
	${SPLASHBIN} --shape=hline --xpoint=1 --ypoint=-67 --properties="x2point=-2" --head

	if [ "${ipaddr}" != "" ]; then
		${SPLASHBIN} --message="$(uname -srmno) - ${ipaddr}" --xpoint=10 --ypoint=-89
	else
		${SPLASHBIN} --message="$(uname -srmno)" --xpoint=10 --ypoint=-89
	fi

	local idx=0
	local sno=1
	local msg=""
	local img="on"
	for svc in ${services} ; do
		sno=$(($sno+1))
	done
	for svc in ${services} ; do
		msg="${svc#+}"
		msg="${msg#-}"
		idx=$(($idx+1))
		case "${svc}" in
			+*)
				img="on";;
			-*)
				img="off";;
		esac
		${SPLASHBIN} --image=$img --xpoint=-${idx}${sno} --ypoint=-34 --head
		${SPLASHBIN} --message="${msg}" --xpoint=-${idx}${sno} --ypoint=-3545 --head
	done
}

##########################################################################################

case "$1" in
    start)
    	if [ "$splash" = "y" ]; then
			if [ "$setup" = "y" ]; then
				${SPLASHSVC} setup &
			else
				mcpistatus=$(${SETUPTOOL} -g service -ss "clue-mcpi.service")
				if [ "${mcpistatus}" == "active" ] && [ "$(/sbin/runlevel | cut -d' ' -f2)" != "5" ]; then
					${SPLASHBIN} --message="starting services.." --ypoint=-34 --wipe
					${SPLASHBIN} --message="starting MCPi.." --ypoint=-34
					${SPLASHBIN} --exit=30
				else
					${SPLASHSVC} info &
				fi
			fi
		fi
		;;
    stop)
    	if [ "$splash" = "y" ]; then
			${SPLASHBIN} --image=splash --console --reset
			${SPLASHBIN} --message="stopping services.." --ypoint=-34
		fi
		;;
	setup)
		if [ "$splash" = "y" ]; then
			doSetup
		fi
		;;
	info)
		if [ "$splash" = "y" ]; then
			doInfo
		fi
		;;
	*)
		echo "Usage: $0 {start|stop|setup|info}"
		exit 1
		;;
esac
exit 0
##########################################################################################