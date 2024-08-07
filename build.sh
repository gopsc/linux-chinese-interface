#定义参数

#cpr=/usr/local/cuda-12.3/bin/nvcc
#cpr=/mnt/1/qs_20240801/src/tools-master/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/arm-linux-gnueabihf-g++
cpr=clang++

cop='-std=c++20 -lX11 -lpthread'
#cop='-lX11 -lpthread'
dop=''
out_name='lci'

#调整参数
if [ $# -eq 1 ]; then
    if [ $1 = 'debug' ]; then
ls    
        echo 'Compile with debug mode...'
        dop='-DDEBUG'
	out_name='debug'

    fi
fi

#继续定义参数
in_names=(\
    "cJSON-master/cJSON" \
    "main" \
    "soft/model/fsm" \
    "soft/os" \
    "soft/terminal/DisableEcho" "soft/terminal/HideCursor" "soft/terminal/PrimaryTerminal" \
    "soft/data/CNode" "soft/data/Flow" "soft/data/string" \
    "soft/Printer/PrinterData" "soft/Printer/PaperCutter" \
    "soft/Printer/BasicPrinter" "soft/Printer/StandardPrinter" \
    "soft/Painter/CmdPainter" "soft/Painter/Area/Area" \
    "soft/Painter/font_library/ku500" "soft/Painter/font_library/showzhi" \
    "soft/thread/CommonThread")


echo 'Compile start...'

#创建构建目录
if [ ! -d ./build ]; then
    mkdir ./build
fi

o_names=()
logs=()
#遍历输入
for name in "${in_names[@]}"; do
    input="${name}.cpp"
    output="./build/${name}.o"
    log="./build/${name}.log"
    #创建文件夹
    dir=`dirname $output`
    if [ ! -d "${dir}" ]; then
        mkdir -p ${dir}
    fi
    #准备编译
    echo "$input"
    res=`${cpr} ${cop} ${dop} -c ${input} -o ${output} &>${log} &`
    if [ "$res" != "" ]; then
        echo $res
    fi
    o_names+=("${output}")
    logs+=("${log}")
done

#等待编译结束
while true ; do
    #检查进程中是否有编译任务
    if ps aux | grep -v grep | grep "$cpr" > /dev/null;then
        sleep 0.1
    else
        break
    fi
done

for name in "${logs[@]}"; do
    cat $name
done

echo "link start..."
${cpr} ${cop} ${dop} -o ${out_name} ${o_names[@]}

echo 'done.'
rm -rf ./build
