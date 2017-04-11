#!/bin/sh

# wait for goahead scripts done */
sleep 20
killall -SIGHUP goahead
killall -SIGHUP udhcpc
killall -SIGHUP udhcpd
ifconfig br0 down
ifconfig eth2 down
brctl delbr br0
config-vlan.sh 2 G01234
reg s b0100000
reg w 20 c0711111
reg w 60 c0712222
echo "Ethernet Loopback is ready"

ifconfig ra0 down
mkdir -p /etc/Wireless/RT2860
cp /etc_ro/Wireless/RT2860AP/RT2860AP.dat /etc/Wireless/RT2860/RT2860.dat
ifconfig ra0 10.10.10.254
sleep 1
#iwpriv ra0 set SSID="`nvram_get slt_wifi_test_ssid`"
iwpriv ra0 set SSID="MT7620_SLT_AP1"
sleep 1
iperf -s &
echo "WiFi link is ready."

