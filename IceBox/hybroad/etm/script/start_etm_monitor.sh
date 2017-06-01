#!/bin/sh +x

ETM_ARGS=""
HUB_ARGS=""
VOD_ARGS=""

#--system_path: 设置程序的工作目录,程序会在此目录下创建各分区软链接
ETM_ARGS="$ETM_ARGS --system_path=$THUNDER_SYSTEM_DIR"

#--etm_cfg: 指定etm配置文件路径,若不存在,会在必要时刻创建此文件,保存会员验证等信息
ETM_ARGS="$ETM_ARGS --etm_cfg=$THUNDER_CONFIG_DIR/etm.ini"

#--disk_cfg: 指定磁盘检测配置文件路径
ETM_ARGS="$ETM_ARGS --disk_cfg=$THUNDER_CONFIG_DIR/thunder_mounts.cfg"

#--log_cfg: 指定日志配置文件路径, 此文件必须存在并正确，否则程序无法启动。
ETM_ARGS="$ETM_ARGS --log_cfg=$THUNDER_CONFIG_DIR/log.ini"

#--pid_file: 指定pid文件路径。程序启动后在此文件第一行写入pid
ETM_ARGS="$ETM_ARGS --pid_file=$THUNDER_TEMP_DIR/xunlei.pid"

#--license: 指定程序所使用的license
ETM_ARGS="$ETM_ARGS --license=$THUNDER_LICENSE"

#--preferred_network_adapter: 指定程序首选网卡, 目前只会对默认MAC造成影响
ETM_ARGS="$ETM_ARGS --preferred_network_adapter=$THUNDER_NETWORK_ADAPTER"

#--hubble_report_pipe_path: 管道路径
ETM_ARGS="$ETM_ARGS --hubble_report_pipe_path=$THUNDER_TEMP_DIR/etm_hubble_report.pipe"

#--ntfs_type: 设置NTFS驱动类型,支持预分配磁盘空间. 0:不支持(ntfs-3g等) 1:Tuxera NTFS(tntfs) 2:UFSD
ETM_ARGS="$ETM_ARGS --ntfs_type=$THUNDER_NTFS_TYPE"

#--vod_port: 指定VOD监听地址.默认为8002.必须同时向etm和vod_httpserver传递相同的值
if [ -n "THUNDER_VOD_PORT" ]; then
    ETM_ARGS="$ETM_ARGS --vod_port=$THUNDER_VOD_PORT"
fi

#--lc_port: 该参数为本地端口号,获取本地信息时使用,默认值为9000
LC_PORT=9000
if [ -n "$THUNDER_LC_PORT" ]; then
    LC_PORT=$THUNDER_LC_PORT
    HUB_ARGS="$HUB_ARGS --lc_port=$THUNDER_LC_PORT"
fi

if [ -n "$THUNDER_DEVICEID" ]; then
    ETM_ARGS="$ETM_ARGS --deviceid=$THUNDER_DEVICEID"
fi

if [ -n "$THUNDER_HARDWAREID" ]; then 
    ETM_ARGS="$ETM_ARGS --hardwareid=$THUNDER_HARDWAREID"
fi

if [ -n "$THUNDER_VOD_PORT" ]; then
    VOD_ARGS="$VOD_ARGS --vod_port=$THUNDER_VOD_PORT"
fi

if [ -n "$THUNDER_LC_PORT" ]; then
    VOD_ARGS="$VOD_ARGS --etm_port=$THUNDER_LC_PORT"
fi

HUB_ARGS="$HUB_ARGS --system_path=$THUNDER_SYSTEM_DIR"

#--hubble_report_pipe_path: 管道路径
HUB_ARGS="$HUB_ARGS --hubble_report_pipe_path=$THUNDER_TEMP_DIR/etm_hubble_report.pipe"

#--etm_cfg: etm.ini配置文件的路径,如果没有指定lc_port参数,hubble进程需要从etm.ini获取端口
HUB_ARGS="$HUB_ARGS --etm_cfg=$THUNDER_CONFIG_DIR/etm.ini"

#--hubble_log_cfg: hubble_log.ini配置文件的存放路径
HUB_ARGS="$HUB_ARGS --hubble_log_cfg=$THUNDER_CONFIG_DIR/hubble_log.ini"

start_etm()
{
    mkdir -p ${THUNDER_SYSTEM_DIR}
	echo "executing ${THUNDER_BIN_DIR}/etm --listen_addr=0.0.0.0:$LC_PORT $ETM_ARGS"
	( ${THUNDER_BIN_DIR}/etm $ETM_ARGS --listen_addr=0.0.0.0:$LC_PORT $@ & )
	( ${THUNDER_BIN_DIR}/vod_httpserver $VOD_ARGS & )
}

#start_hubble()
#{
#	( ${THUNDER_BIN_DIR}/hubble $HUB_ARGS $@ & )
#}
#check_hubble_status()
#{
#	ps | grep hubble | grep -v grep > /dev/null 2>&1
#	return $?
#}

start_etm $@
