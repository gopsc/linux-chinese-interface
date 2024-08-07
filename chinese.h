
//作者：简单
//修改：qing
/*
 * C++17支持了中文变量名，配合define预编译指令可以将代码中文化。
 * 
 * 在C语言中，变量名只能由字母、数字或是下划线组成。
 * 由于C++语言兼容C语言，最开始的C++是不能够使用多字节变量名的。
 * 
 * 接下来我会给出一个让代码中文化的头文件，并试图建立一个标准。
 * */
#ifndef __夕_中文模式
#define __夕_中文模式

/*保留字*/
#define 使用 using
#define 命名空间 namespace
#define 取别名 typedef
#define 覆盖 override
#define 取内存大小 sizeof
#define 返回 return
#define 如果 if
#define 否则 else
#define 又或者 else if
#define 真 true
#define 假 false
#define 且 and
#define 或 or
#define 空类型 void
#define 常量 const
#define 有符号 signed
#define 无符号 unsigned
#define 文件 FILE
#define 文件结束 EOF
#define 模板 template
#define 类 class
#define 结构体 struct
#define 枚举类型 enum
#define 对象 this
#define 抛出异常 throw
#define 新建 new
#define 删除 delete
#define 跳出 break
#define 继续 continue
#define 判断循环 while
#define 循环判断 do
#define 循环判断尾 while
#define 分支选择 switch
#define 支系 case
#define 缺省 default
#define 计次循环 for
#define 尝试 try
#define 获取异常 catch
#define 捕获异常 catch
#define 退出 exit
#define 公开的 public
#define 受保护的 protected
#define 私有的 private
#define 外部的 extern
#define 静态的 static
#define 虚拟的 virtual

#define 空指针 NULL
#define 整数 int
#define 字符 char
#define 短整数 short
#define 长型 long
#define 超长型 uint64_t
#define 单精度小数 float
#define 双精度小数 double
#define 图像单位 graphsize_t
#define 长度单位 length_t
#define 位置单位 position_t
#define 内存单位 memory_t
#define 像素单位 pixel_t
#define 状态 error_t
#define 状态量 status //用于状态机
#define 标识符 identifier_t
#define 设备标识符 device_t
#define 错误码 error_t
#define 逻辑型 bool
#define 可变类型 auto
#define _文件 FILE
#define _文件结束 EOF
#define _读模式 "r"
#define _写模式 "w"
#define 字符串集 std::vector<std::string>

#define 入口 main
#define 主函数 main
#define 失败 CCA_FALSE
#define 成功 CCA_OK
#define 错误 CCA_ERROR
#define 状态量 status
#define 线程关闭 SSHUT
#define 线程开始 SSTART
#define 线程暂停 SSTOP
#define 线程运行 SRUNNING

//-----------------------------------------------------------
//-----------------------------------------------------------
//类型定义

//用于记录长度的类型。
typedef int length_t;

//用于数据结点中记录区域长度的类型。
typedef length_t lengthField_t;

//用于数据结点中记录数据域长度的类型。
typedef lengthField_t lengthDataField_t;
//用于数据结点中记录指针域长度的类型。
typedef lengthField_t lengthPointerField_t;

//C语言缓存字符串的数据域长度。
#define CCAS_LENGTH 2

//图像的尺寸类型。
typedef length_t graphsize_t;
//用于存储位置信息的类型。
typedef length_t position_t;

//-----------------------------------------------------------
#include<stdint.h>
//像素点类型。
//由于不同显示器设置时的像素尺寸不一致
//建议改用unsigned char
#ifdef __x86_64__
typedef uint32_t pixel_t;
#endif
#ifdef __aarch64__
typedef uint16_t pixel_t;
#endif

//-----------------------------------------------------------
//标识符类型。
typedef int identifier_t;

//返回的错误处理类型。
//可以使用枚举类型
typedef identifier_t error_t;

#define CCA_OK        0
#define CCA_ERROR     1
#define CCA_FALSE     0
#define CCA_TRUE      1
#define CCA_NOTFOUND -1

//------------------------------------
//用于存放状态信息的类型。
//这里应该是信号量，而不是状态机
/*
typedef identifier_t status_t;

#define PPT_SHUT -1
#define PPT_STOP 0
#define PPT_START 1
#define PPT_RUNNING 2
*/
enum status
{
    SSHUT    = -1,  //线程关闭信号
    SSTOP    = 0,   //线程停止信号
    SSTART   = 1,   //线程开始信号
    SRUNNING = 2    //线程正在运行
};

//用于存放键盘事件标识符的类型。
typedef identifier_t kbcode_t;

//用于存放设备标识符的类型。
typedef identifier_t device_t;
//用于存放套接字标识符的类型。
typedef device_t socket_t;

//-----------------------------------------------------------
//内存和缓冲区
//用于存放内存的类型。
typedef void* memory_t;
//缓冲区类型。
typedef memory_t buffer_t;
//用于共享数据的通道类型。
typedef buffer_t passagw_t; 

//链表结点
//用于链表结点的域类型。
typedef memory_t field_t;
//用于链表结点的数据域类型。
typedef field_t dataField_t;
//用于链表结点的指针域类型。
typedef field_t pointerField_t;

//-----------------------------------------------------------
//-----------------------------------------------------------
//基本单元

#ifdef __cplusplus
namespace 夕 {
    
    //划分类型的最小的单位
    class 基本单元{};
    //这是一种生成后就自动生效的系统调用
    class 基本声明{};
    //这是一种接口
    class 基本工具{};

    类 单元{};
    类 声明{};
    类 接口{};
}
#endif //__cplusplus
#endif //__夕_中文模式
