
#include <charay.h>
#include <bwio.h>
#include <panic.h>
#include <string.h>

int getCharMap(char a){
    int offset = 0;
    if(a>='0' && a<='9'){
        return a-'0';
    }
    offset += 10;
    if(a>='a' && a<='z'){
        return a-'a' + offset;
    }
    offset += 26;
    if(a>='A' && a<='Z'){
        return a -'A' + offset;
    }
    offset += 26;
    if(a=='-'){
        return offset + 1;
    }
    offset += 1;
    if(a=='_'){
        return offset + 1;
    }
    offset += 1;
    if(a==':'){
        return offset + 1;
    }
    offset += 1;
    if(a=='='){
        return offset + 1;
    }
    offset += 1;
    if(a=='!'){
        return offset + 1;
    }
    offset += 1;
    if(a=='$'){
        return offset + 1;
    }
    offset += 1;
    if(a=='@'){
        return offset + 1;
    }
    offset += 1;
    if(a=='*'){
        return offset + 1;
    }
    offset += 1;
    return offset;
}

//AlphaNumeric Hash:
//Accepts 0-9, a-z, A-Z, -, _, :, =, !
//Range:   10 + 26 + 26 + 5 = 67
int alphaNumericHash(char* name){
    int i, hash = 0;
    int base = getCharMap(' ');
    for(i=0;name[i];i++){
        hash*=base;
        hash+=getCharMap(name[i]);
    }
    return hash;
}
