#!/bin/bash
thisDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $thisDIR
source ~/.econotagsource
contikiroot=..
libmc=../../libmc1322x
rplBR=$contikiroot/examples/ipv6/rpl-border-router/border-router_redbee-econotag.bin
tunslip=$contikiroot/tools/tunslip6
gdb=$contikiroot/expanded-prereqs/bin/gdb-7.4/gdb/gdb
openocd=$contikiroot/expaned-prereqs/bin/openocd-0.5.0/src/openocd
#ECONOTAGBIN=/~/workspace/contiki/examples/new-ipv6/empty_redbee-econotag.bin
if [ "$2" = "guessTTY" ] || [ "$2" = "" ];
then
	usbdev=/dev/ttyUSB$(ls /dev/ttyUSB* | grep -v grep | grep -o [0-9] | tail -1)
else
	usbdev=/dev/ttyUSB$2
fi
echo "targeted: $usbdev"
TARGET=redbee-econotag
perloader=$contikiroot/cpu/mc1322x/tools/mc1322x-load.pl
bbmcpath=$contikiroot/cpu/mc1322x/tools/ftditools/bbmc
flasherapp=$libmc/tests/flasher_redbee-econotag.bin
application=$ECONOTAGBIN
gdbfile=${ECONOTAGBIN/_redbee-econotag.bin/.elf}
#601: Boarder Router
file=$(basename $ECONOTAGBIN)
wmctrl -a MCBURN
if [ "$file" = "udp-frz-blast_redbee-econotag.bin" ]
then
mac=0x1e000,0x01020304,0x01060708
elif [ "$file" = "border-router_redbee-econotag.bin" ]
then
mac=0x1e000,0x01020304,0x02060708

elif [ "$file" = "empty_redbee-econotag.bin" ]
then
mac=0x1e000,0x01020304,0x03060708
elif [ "$file" = "WW-power_redbee-econotag.bin" ]
then
mac=0x1e000,0x01020304,0x03060708
elif [ "$file" = "WW-ledcontroller_redbee-econotag.bin" ]
then
mac=0x1e000,0x01020304,0x18060708
else mac=0x1e000,0x01020304,0x01160708
fi
echo $mac $file


if [ "$1" = "" ]
then
  echo "Usage: $0 [romburn | ramburn | macburn | erase | debug | border | tunslip] usb#"
  exit
fi
if [ "$1" = debug ]
then
    killall openocd
    $bbmcpath -l redbee-econotag erase
    openocd >/dev/null 2>&1 &
    $gdb $gdbfile
fi

if [ "$1" = border ]
then 
$bbmcpath -l redbee-econotag erase
mac=0x1e000,0x01020304,0x02060708
$perloader -f $flasherapp -s $rplBR -t $usbdev $mac
fi


if [ "$1" = tunslip ]
then 
sudo $tunslip -s $usbdev aaaa::1/64
fi


if [ "$1" = romburn ]
then 
$bbmcpath -l redbee-econotag erase
$perloader -f $flasherapp -s $application -t $usbdev $mac
fi

if [ "$1" = ramburn ]
then
#$perloader -f $application -t $usbdev -c '/home/travis/workspace/contiki/cpu/mc1322x/tools/ftditools/bbmc -l redbee-econotag reset'
#$bbmcpath -l redbee-econotag erase
#$perloader -f $application -t $usbdev -c "$bbmcpath -l redbee-econotag reset"
$perloader -f $application -t $usbdev $mac -c "$bbmcpath -l redbee-econotag reset"
echo $perloader -f $application -t $usbdev $mac -c "$bbmcpath -l redbee-econotag reset"
fi

if [ "$1" = erase ]
then
ECMD="$bbmcpath -l redbee-econotag erase"
echo $ECMD
$ECMD
fi

if [ "$1" = macburn ]
then
    read -p "Enter the last 8 bits in hex.  Your address will be: [aaaa::603:201:807:6xx]" address
    address=`echo $address | rev`
    mac="0x1e000,0x01020304,0x0$address""60708"
    echo $mac
    
$bbmcpath -l redbee-econotag erase
echo $perloader -e -f $flasherapp -z -t $usbdev -c "$bbmcpath -l redbee-econotag reset" $mac
fi
echo "DONE...$1"
