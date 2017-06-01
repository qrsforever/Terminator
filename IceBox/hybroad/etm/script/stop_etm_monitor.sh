#!/bin/sh +x

#stop_hub()
#{
#	echo "stopping hubble"
#	killall -9 hubble
#}

stop_etm()
{
	pid=`cat $THUNDER_TEMP_DIR/xunlei.pid`
	echo "stopping etm pid=$pid"
	kill -9 $pid 2>/dev/null
}

stop_vod()
{
	echo "stopping vod_httpserver"
	killall -9 vod_httpserver
}

#stop_hub
stop_etm
stop_vod
