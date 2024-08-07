#ifndef __H_FILEOP
#define __H_FILEOP

#include<stdio.h>
#include<string.h>
#include<errno.h>

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif //_WIN32

#ifdef __cplusplus
#include<iostream>
#include<fstream>
#include<filesystem>
#include"../../type.h"
namespace qing{
	
    class FilesystemOp:public Unit{
        public:

            static FILE* Openfile(char* path, const char* mode){
                //以固定模式打开文件，返回文件流。用完后要关闭。
                return fopen(path, mode);
            }//Openfile

            static int Closefile(FILE* path){
                //关闭一个文件描述符
                return fclose(path);
            }//Closefile

            static char Readchar(FILE* fp){
                //从一个已经打开的文件流读取一个字符
                //若文件已经读完，会返回文件结束符号
                return fgetc(fp);
            }//Readchar
			
            static int Writechar(int v, FILE* fp){
                //将字符写入文件的函数
                return fputc(v, fp);
            }//Writechar
			
	    static std::string Readfile(std::string path){
	        // read a file from path

		//Open the file in binary mode
		//and directly position to
		//the end of the file
		std::ifstream s(path, std::ios::binary | std::ios::ate);
		if (!s.is_open()) throw std::runtime_error(std::string("file-system::Readfile, open file: ")+strerror(errno));

		//Get current file position
		//(Because the position has already
		//been set to the end of the file,
		//the result obtained here represents
		//the file size)
		std::streamsize l = s.tellg();
		//Reposition the file pointer to
		//the beginning of the file
		s.seekg(0, std::ios::beg);

		//Create a string buffer with the same size
		//as the file size, and fill it
		//with space characters.
		std::string buf(l, ' ');
		if (!s.read(&buf[0], l)) throw std::runtime_error(std::string("file-system::Readfile, read file: ")+strerror(errno));

		//If the file ends with a newline character,
		//it might not be read into the buffer, which
		//could result in extra space characters
		//being present.
		std::string::size_type pos;
		while((pos = buf.find_last_not_of("\r\n \t")) != std::string::npos && pos != buf.length() - 1) buf.erase(pos + 1);

		//Now, the buffer contains the entire
		//content if the file
		return buf;
	
	    }//Readfile

	static void Writefile(std::string path, std::string content){
	    std::ofstream s(path);
            s << content;
	    s.close();
	}//Writefile

	static bool Exist(const std::string& path){
	    return std::filesystem::exists(path);
	}//Exist

    };//FilesystemOp
	
}//qing
#endif //cpluscplus
#endif //!__H_FILEOP
