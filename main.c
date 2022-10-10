
#include <unistd.h>
#include <stdarg.h>
#include <malloc.h>

static int ArgsCounter(const char* format) {
    unsigned int count = 0;
    for (unsigned int i = 0; format[i] != '\0'; i++) {
        if (format[i] == '%') count++;
    }
    if (count) return count;
    return -1;
}

static char* ChangeStr(char* str, unsigned int pos, char* what, unsigned int what_len, unsigned int str_len) {
    if (what_len <= 0 || str_len <= 0 || !str || !what) {
        if (str) {free(str); str=NULL;}
        if (what) {free(what); what=NULL;}
        return NULL;
    }
    if (what_len > 2) {
        char* tmp = (char*)malloc(sizeof(char) * (what_len + str_len));
        unsigned int j = 0;
        for (unsigned int i = 0; i < pos; i++) {
            tmp[i] = str[i];
        }
        for (unsigned int i = pos, j = 0; i < pos + what_len && j < what_len; i++, j++) {
             tmp[i] = what[j];
        }
        for (unsigned int i = pos + what_len, j = pos + 2; j < str_len && i < str_len + what_len; i++, j++) {
            tmp[i] = str[j];
        }
        free(str);
        str = NULL;
        tmp[str_len + what_len - 1] = '\0';
        return tmp;
    } 
    else if (what_len == 1) {
        char* tmp = (char*)malloc(sizeof(char) * str_len - 1);
        for (unsigned int i = 0; i < pos; i++) {
            tmp[i] = str[i];
        }
        tmp[pos] = what[0];
        for (unsigned int i = pos + 1; i < str_len - 1; i++) {
            tmp[i] = str[i + 1];
        }
        free(str);
        str = NULL;
        tmp[str_len - 2] = '\0';
        return tmp;
    }  
    else if (what_len == 2) {
        for (unsigned int i = pos, j = 0; i < str_len && j < what_len; i++, j++) {
            str[i] = what[j];
        }
        return str;
    }
    else { // i dont know in which case this need :) 
        return NULL;
    }
}

static unsigned int DigitCounterInIntNumber(int num) {
    unsigned int counter = 0;
    for (int tmp = num; tmp > 0; counter++) {
        tmp /= 10;
    }
    return counter;
}   

static char* IntToStr(char* str, int num, unsigned int len) {
    int tmp = num;
    for (int i = len - 1; tmp > 0 && i > -1; i--) {
        str[i] = tmp % 10 + '0';
        tmp /= 10;
    }

    return str;
}

int Print(const char* format, ...) {
    unsigned int args_count = ArgsCounter(format);
    unsigned int str_len = 0;
    for (; format[str_len] != '\0'; str_len++);
    str_len++;
    char* print_str = (char*)malloc(sizeof(char) * str_len);
    if (!print_str) return -1;
    for (unsigned int i = 0; i < str_len; i++) {
        print_str[i] = format[i];
    }
    print_str[str_len - 1] = '\0';
    char* insert_str = NULL;
    unsigned int insert_str_len = 0;
    va_list list;
    va_start(list, args_count);
    int arg_int = 0;
    for(unsigned int i = 0; i < str_len && print_str[i] != '\0'; i++) {
        if (print_str[i] == '%') {
            if (print_str[i + 1] == '\0') return -1;
            switch (print_str[i + 1]) {
            case 'd':
                arg_int = va_arg(list, int);
                insert_str_len = DigitCounterInIntNumber(arg_int);
                insert_str = (char*)malloc(sizeof(char) * insert_str_len);
                if (!insert_str) {free(print_str); return -1;}
                print_str = ChangeStr(print_str, i, IntToStr(insert_str, arg_int, insert_str_len), insert_str_len, str_len);
                if (!print_str) {free(insert_str); return -1;}
                free(insert_str);
                insert_str = NULL;
                str_len += insert_str_len;
                break;
            default:
                break;
            }
        }
    }
    va_end(list);
    write(STDOUT_FILENO, print_str, str_len);
    free(print_str);
    print_str = NULL;
}

int main() {
    Print("privet %d %d %d %d %d %d\n", 100, 10, 1, 1000, 10000, 14);
    _exit(0);
}