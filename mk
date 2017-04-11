#!/bin/bash

PARAM_NAME=""
PARAM_VALUE=""
CUR_INDEX=""
GUESTID=""
GUESTIDPARM=""
GUESTIDPARM1=""
GUESTIDPARM2=""
PARMNUM=$#

Do_ModifyDefaultConfig()
{
#### HE_ROUTE_SN=86999900000014060000 => HE_ROUTE_SN=86999900000014100000######
	newflag="_new"
	defaultfile="/vendors/Ralink/MT7620/RT2860_default_vlan"
	eqiflag="="
	newvalue="867777000000"
	newdate=$(date +%y%m)
	suffixparts="0000"

	filepath=$(pwd)
	echo $filepath
	filename="$filepath$defaultfile"
	newconfigure="$filepath$defaultfile$newflag"
	echo $filename
	rm -rf $newconfigure
	while read line; do
		name=`echo $line|awk -F '=' '{print $1}'`
		value=`echo $line|awk -F '=' '{print $2}'`
		if [ "$name" = "HE_ROUTE_SN" ]; then
			echo $name
			echo "$name$eqiflag$newvalue$newdate$suffixparts"
			echo "$name$eqiflag$newvalue$newdate$suffixparts" >>$newconfigure
		else
			echo $line >>$newconfigure
		fi
	done < $filename
	cp -rf $newconfigure $filename
}

UsageEntry()
{
  echo "usage:"
  echo "$0 <guest_id>"
  echo "guest_id: "
  echo "	9999  试用"
  echo "	7777  商用"
  echo "	00  通用"
  echo "	32  北京"
  echo "	33  天津"
  echo "	34  河北"
  echo "	35  山西"
  echo "	36  内蒙古"
  echo "	37  辽宁"
  echo "	38  吉林"
  echo "	39  黑龙江"
  echo "	40  上海"
  echo "	41  江苏"
  echo "	42  浙江"
  echo "	43  安徽"
  echo "	44  福建"
  echo "	45  江西"
  echo "	46  山东"
  echo "	47  河南"
  echo "	48  湖北"
  echo "	49  湖南"
  echo "	50  广东"
  echo "	51  海南"
  echo "	52  广西"
  echo "	53  重庆"
  echo "	54  四川"
  echo "	55  贵州"
  echo "	56  云南"
  echo "	57  陕西"
  echo "	58  甘肃"
  echo "	59  青海"
  echo "	60  宁夏"
  echo "	61  新疆"
  echo "	62  西藏"
  echo ""
}



Do_Init()
{
	echo "=============== copy compile files =============="
	cp -rf user/rt2880_app/scripts/build.config user/rt2880_app/scripts/.config
	cp -rf uClibc++/build.config uClibc++/.config

	# clean all objects and configurations
	echo "=============== make mrproper =============="
	make mrproper

	# copy basic compile configuration of platform
	echo "=============== copy basic compile configuration of platform =============="
	cp build.config .config
	cp build.autoconf.h autoconf.h

	# load default compile settings
	echo "=============== load default compile settings =============="
	chmod u+x config/setconfig
	config/setconfig defaults
	config/setconfig final
	cp -rf vendors/Ralink/MT7620/config/4M_32M_config.vendor-2.6.36.x ./config/.config
}

Do_Build()
{
	# build firmware
	echo "=============== build firmware =============="
	make dep
	make
}

Do_Main()
{
	if [ ! -s ./.buildfirst ]; then
		echo "######## S E T  D E F A U L T  C O N F I G  ######"
		echo "buildfirst" > ./.buildfirst
		Do_Init
	  echo
	fi
	Do_Build
}

#Do_SetGuestId()
{
	# check paramter - number
	if [ $PARMNUM -lt 1 ]; then
		UsageEntry
		echo ""
		exit 1
	fi

	GUESTIDPARM=$1

	if [ "$GUESTIDPARM" -eq "7777" ]; then
		GUESTID="7-777"
	elif [[ $GUESTIDPARM -eq "9999" ]]; then
		GUESTID="9-999"
	elif [[ $GUESTIDPARM -eq "00" ]]; then
		GUESTID="0-000"
	elif [[ "$GUESTIDPARM" -ge "32" && "$GUESTIDPARM" -le "62" ]]; then
		GUESTIDPARM1="${GUESTIDPARM:0-0:1}"
		GUESTIDPARM2="${GUESTIDPARM:0-1:1}"
		GUESTID="${GUESTIDPARM1}-${GUESTIDPARM2}08"
	else
		exit 1
	fi

	export "HEROUTEGUST=$GUESTID"
	echo "Export guest_id($GUESTID)..."
}

#Do_SetGuestId
#Do_ModifyDefaultConfig
Do_Main

