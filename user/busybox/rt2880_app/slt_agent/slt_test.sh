#!/bin/sh

get_arg()
{
for var in "$@"
do
	if [ "$var" != "" ]; then
		if [ "`expr $var : "\(test_all_f=\)"`" == "test_all_f=" ]; then
			TEST_ALL_F=`echo $var | cut -d "=" -f 2`
			arg1="test_all_f=$TEST_ALL_F"
		elif [ "`expr $var : "\(test_item=\)"`" == "test_item=" ]; then
			TEST_ITEM=`echo $var | cut -d "=" -f 2`
			arg2="test_item=$TEST_ITEM"
		elif [ "`expr $var : "\(wifi_only=\)"`" == "wifi_only=" ]; then
			TEST_ITEM=`echo $var | cut -d "=" -f 2`
			arg3="wifi_only=$WIFI_ONLY"
		fi
	fi
done 
}

wifi_ping_test()
{
	num=4
	wifi_result=""
	while [ "$num" -gt 0 -a "$wifi_result" == "" ]
	do
		if [ "$num" -lt 4 ]; then
			killall ping
		fi
		num=`expr $num - 1`
		ping 10.10.10.254 -c 1 > /tmp/slt_wifi_result &
		sleep 2
		wifi_result=`cat /tmp/slt_wifi_result | grep "64 bytes from 10.10.10.254"`
		if [ "$wifi_result" != "" ]; then
			echo "#################################"
			echo "SLT WiFi pass $num"
			echo 1 > /proc/slt/wifi
			echo "#################################"
		else
			echo "#################################"
			echo "SLT WiFi failed $num"
			echo "#################################"
			if [ "$num" = "0" ]; then
				echo 2 > /proc/slt/wifi
				iwpriv ra0 stat
			fi
		fi
	done
}

if [ "$1" = "RT6855" ] || [ "$1" = "RT6856" ] || [ "$1" = "MT7620" ]; then	
	get_arg $*
	
	if [ "$1" = "MT7620" ]; then
		WIFI_ONLY=`nvram_get slt_wifi_only`
		if [ "$WIFI_ONLY" = "1" ]; then
			arg3="wifi_only=1"
		fi

		mkdir -p /etc/Wireless/RT2860
		cp /etc_ro/Wireless/iNIC/RT2860AP.dat /etc/Wireless/RT2860/RT2860.dat
		#insmod /lib/modules/2.6.36/kernel/drivers/net/wireless/rt2860v2_sta/rt2860v2_sta.ko
		ifconfig ra0 10.10.10.253
		iwpriv ra0 set NetworkType=Infraa
		iwpriv ra0 set AuthMode=OPEN
		iwpriv ra0 set EncrypType=NONE
		#iwpriv ra0 set SSID="`nvram_get slt_wifi_test_ssid`"
		iwpriv ra0 set SSID=MT7620_SLT_AP1
	fi

	# EPHY test
	ifconfig eth2 up
	ifconfig eth2 down
	config-vlan.sh 3 12345
	switch vlan set 6 7 11111011

	# USB Host test
	num=5
	umount /media/*
	mkdir -p /media/sda
	mkdir -p /media/sdb
	while [ "$num" -gt 0 ]
	do
		mdev -s
		mount /dev/sda /media/sda
		mount /dev/sda1 /media/sda

		# USB Port 2
		if [ "$1" = "RT6856" ]; then
			mount /dev/sdb /media/sdb
			mount /dev/sdb1 /media/sdb
			if [ -f /media/sda/slt_test1 -a -f /media/sdb/slt_test1 ]; then
				break;
			fi
		elif [ -f /media/sda/slt_test1 ]; then
			break;
		fi
	
		num=`expr $num - 1`
		sleep 1
	done

	# SD
	num=3
	umount /media/mmc
	mkdir -p /media/mmc
	if [ "$1" = "MT7620" ]; then
		while [ "$num" -gt 0 -a ! -f /media/mmc/slt_test1 ]
		do
			mdev -s
			mount /dev/mmcblk0 /media/mmc
			mount /dev/mmcblk0p1 /media/mmc
			num=`expr $num - 1`
			sleep 1
		done
	fi

	rmmod slt
	insmod /lib/modules/2.6.36/kernel/arch/mips/ralink/slt/slt.ko $arg1 $arg2 $arg3

	if [ "$1" = "MT7620" ]; then
		wifi_ping_test
	fi

	exit 0
fi


