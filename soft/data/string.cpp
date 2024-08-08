#include "string.h"
namespace qing {
    //-------------------------------------------------------------------------
    string::string(){
        char *s = new char[1];
        s[0] = '\0';
        this->data = s;
    }

    string::string(const char *p){
        this->data = string::get_copy((char*)p);
    }

    string::string(char *p){
        this->data = p;
    }

    string::string(const string& m){
        char *p = m.get();
        this->data = string::get_copy(p);
    }

    string::~string(){
        if(this->data) delete[] this->data;
    }

    //-------------------------------------------------------------------------
    // Accessor

    char* string::get() const{
        return this->data;
    }

    //=========================================================================
    // operator

    bool string::operator==(const string& m) const{
        char *p1 = m.get();
        char *p2 = this->get();
        return 0 == string::compare(p1, p2);
    }

    bool string::operator==(const char* p1) const{
        char *p2 = this->get();
        return 0 == string::compare(p1, p2);
    }

    bool string::operator!=(const string& m) const{
        char *p1 = m.get();
        char *p2 = this->get();
        return 0 != string::compare(p1, p2);
    }

    bool string::operator!=(const char* p1) const{
        char *p2 = this->get();
        return 0 != string::compare(p1, p2);
    }

    string& string::operator=(const string& m){
        if(this != &m)
        {
            char *p1 = m.get();
            char *p2 = this->get();
            this->data = string::get_copy(p1);
            delete[] p2;
        }
        return *this;
    }

    string& string::operator=(const char *t){
        char *p1 = (char*)t;
        char *p2 = this->get();
        this->data = string::get_copy(p1);
        delete[] p2;
        return *this;
    }

    string& string::operator=(char *t){
        char *p = this->get();
        this->data = t;
        delete[] p;
        return *this;
    }

    //-------------------------------------------------------------------------
    string string::operator+(const string& m) const{
        char *p1 = this->get();
        char *p2 = m.get();
        return string(string::get_cat(p1, p2));
    }

    //=========================================================================
    // code

    string string::to_ANSI() const{
        char *src = this->get();
        char *t = string::utf8_to_ansi(src);
        return string(t);
    }

#ifdef __linux__

    char* string::utf8_to_ansi(char* utf8String){
        //UTF-8 string to ANSI
        //
        //@qing，20231120：青木编程楠舍科技刘仁。
        //@qing，20240528：输出字符串的长度不确定

	//Character set conversion descriptor variable
        iconv_t ic;

        ic = iconv_open("GB18030", "UTF-8");
        if (ic == (iconv_t)-1) throw std::runtime_error(std::string("utf8_to_ansi, iconv_open: ")+strerror(errno));

        //set variable
        //@qing，20240531：设置输出字符串的大小为输入的两倍可能不太合适
        size_t InputLength = strlen(utf8String);
        size_t OutputLength = InputLength * 2;

        char* ansiString = (char*)malloc(OutputLength);
        if (ansiString == NULL) throw std::runtime_error(std::string("utf8_to_ansi, malloc")+strerror(errno));

        char *InputBuffer  = (char*)utf8String;
        char *OutputBuffer =        ansiString;
				
        size_t result = iconv(ic, &InputBuffer, &InputLength, &OutputBuffer, &OutputLength);

        if (result == (size_t)-1) {
            free(ansiString);
            throw std::runtime_error(std::string("utf8_to_ansi, iconv")+strerror(errno));
        }

        //Close the character set conversion descriptor
        iconv_close(ic);

	//Return the converted string
        *OutputBuffer = '\0';
        return ansiString;

    }//utf8_to_ansi

    wchar_t* string::utf8_to_wchar(const char* utf8_str){
        /* convert to wchar */
	if (!utf8_str) throw std::runtime_error(std::string("utf8_to_wchar: ") + strerror(errno));

	size_t utf8_len = strlen(utf8_str);
	size_t wide_len = string::count_utf8(utf8_str);

	//allocate memory space
	wchar_t *wide_str = (wchar_t*)malloc((wide_len+1)*sizeof(wchar_t));
	if (!wide_str) throw std::runtime_error(std::string("utf8_to_wchar: ") + strerror(errno));

	//This object can save the converted location information
	mbstate_t state = {0};
	const char *p = utf8_str;
	wchar_t *w = wide_str;

	//Check the length required for the conversion
	size_t ret = mbsrtowcs(NULL, &p, 0, &state);
	if (ret == (size_t)-1){
	    free(wide_str);
	    throw std::runtime_error(std::string("utf8_to_wchar: ") + strerror(errno));
	}

	//Reset the status and start the conversion
	memset(&state, 0, sizeof(mbstate_t));
	p = utf8_str;

	ret = mbsrtowcs(w, &p, utf8_len, &state);
	if(ret == (size_t)-1){
	    free(wide_str);
	    throw std::runtime_error(std::string("uft8_to_wchar: ") + strerror(errno));
	}

	// If the number of wide characters converted is 
	// less than the expected number of
	// characters, an error has occurred
	if (ret != wide_len){
	    free(wide_str);
	    errno = EILSEQ; //Illegal byte sequece
	    throw std::runtime_error(std::string("utf8_to_wchar: ") +strerror(errno));
	}

	//Append string terminator
	w[wide_len] = L'\0';
	return wide_str;
    
    }//utf8_to_wchar

#endif

#ifdef _WIN32

    
    char* string::utf8_to_ansi(char* utf8_string){

        char* utf8String = utf8字符串;
        //设置变量
        int utf8StringLength  = strlen(utf8String);
        //获取UTF-8字符串的长度  
         
        int wideStringLength = MultiByteToWideChar(CP_UTF8, 0, utf8String, utf8StringLength, NULL, 0);  
        wchar_t* wideString = (wchar_t*)malloc(wideStringLength * sizeof(wchar_t));  
        int changeLength = MultiByteToWideChar(CP_UTF8, 0, utf8String, utf8StringLength, wideString, wideStringLength);  
        wideString[changeLength] = '\0';
        // 首先，将UTF-8转换为宽字符（UTF-16） 
      
        int ansiStringLength = WideCharToMultiByte(CP_ACP, 0, wideString, wideStringLength, NULL, 0, NULL, NULL);  
        char* ansiString = (char*)malloc(ansiStringLength * sizeof(char));  
        changeLength = WideCharToMultiByte(CP_ACP, 0, wideString, wideStringLength, ansiString, ansiStringLength, NULL, NULL);  
        ansiString[changeLength] = '\0';
        // 然后，将宽字符（UTF-16）转换为ANSI字符串
    
        //printf("ANSI String: %s\n", ansiString);  
        free(wideString);  
        //free(ansiString); 

        return ansiString;
    }

#endif
}
