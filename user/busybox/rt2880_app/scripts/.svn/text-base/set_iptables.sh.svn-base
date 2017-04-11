#!/bin/sh
#
# $Id: //WIFI_SOC/MP/SDK_4_2_0_0/RT288x_SDK/source/user/rt2880_app/scripts/set_iptables.sh#4 $
#
# usage: set_iptables.sh
#

tarMAC=$2

case $1 in
	"addDropRule")
			iptables -I FORWARD -m mac --mac-source $tarMAC   -j DROP
		;;	
	"deleteDropRule")
			iptables -D FORWARD -m mac --mac-source $tarMAC   -j DROP
		;;	
esac
