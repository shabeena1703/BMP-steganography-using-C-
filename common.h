#ifndef STEG_COMMON_H 
#define STEG_COMMON_H
#define MAGIC_STRING "#*" 

#define RED "\033[1;31m" 
#define GREEN "\033[1;32m" 
#define YELLOW "\033[1;33m" 
#define MAGENTA "\033[1;35m" 
#define CYAN "\033[1;36m" 
#define WHITE "\033[1;37m" 
#define RESET "\033[0m"

enum Status { failure, success }; 
enum Opr_type { unsupported, encode, decode };
#endif