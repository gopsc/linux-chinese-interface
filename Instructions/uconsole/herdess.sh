#!/bin/bash
#注意，这是herdess服务的启动文件

#下面这个地址是程序的目录
path=/home/qing/prj/herdess/20240418/

chvt 8
echo -ne "\033[?25l" >> /dev/tty8
$path/夕 $path/uconsole.csc
