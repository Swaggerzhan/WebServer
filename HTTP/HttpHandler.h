//
// Created by swagger on 2021/4/6.
//

#ifndef WEBSERVER_HTTPHANDLER_H
#define WEBSERVER_HTTPHANDLER_H

#include <cstring>
#include <iostream>
#include <map>
#include "../utility.h"

using std::map;
using std::string;

class HttpHandler{


public:

    map<string, string> httpHeader;

    /**
     * 解析行，只进行行错误检测
     * @param buffer
     * @param checked_index
     * @param read_index
     * @return
     */
    LINE_STATUS parse_line(char* buffer, int &checked_index, int& read_index){
        char tmp;
        for (; checked_index < read_index; ++ checked_index){
            tmp = buffer[checked_index];
            if (tmp == '\r'){
                if ( (checked_index + 1 ) == read_index )
                    return LINE_INCOMPLETE;

                else if ( buffer[checked_index + 1] == '\n'){
                    buffer[checked_index ++ ] = '\0';
                    buffer[checked_index ++ ] = '\0';
                    return LINE_OK;
                }
                return LINE_BAD;
            } else if ( tmp == '\n' ){
                if ( (checked_index > 1 ) && buffer[checked_index - 1] == '\r'){
                    buffer[ checked_index - 1] = '\0';
                    buffer[ checked_index ++ ] = '\0';
                    return LINE_OK;
                }
                /* \n之前不是\r表明数据出错 */
                return LINE_BAD;
            }
        }
        return LINE_INCOMPLETE;
    }


    /**
     * http请求头检测
     * @param tmp
     * @param checkStatus
     * @return
     */
    HTTP_CODE parse_request_line(char *tmp, CHECK_STATUS& checkStatus){

        char* url = strpbrk(tmp, " \t");

        if ( !url )
            return BAD_REQUEST;

        *url ++ = '\0';
        char *method = tmp;

        if ( strcasecmp(method, "GET") == 0 ){
            printf("this is GET function");
        }else{
            printf("other method not suppose");
            return BAD_REQUEST;
        }

        url += strspn(url, " \t");
        char *version = strpbrk(url, " \t");
        if (!version)
            return BAD_REQUEST;

        *version ++ = '\0';
        version += strspn(version, " \t");

        if ( strcasecmp(version, "HTTP/1.1") != 0)
            return BAD_REQUEST;

        if (strncasecmp(url, "http://", 7) == 0){
            url += 7;
            url = strchr(url, '/');
        }


        if ( !url || url[0] != '/')
            return BAD_REQUEST;

        printf("request url is %s\n", url);

        checkStatus = CHECK_HEADER;
        return INCOMPLETE_REQUEST;

    }


    HTTP_CODE parse_header(char* tmp){
        if (tmp[0] == '\0')
            return GET_REQUEST;
        else if ( strncasecmp( tmp, "Host:", 5) == 0){
            tmp += 5;
            tmp += strspn(tmp, " \t");

            /*  将字符存入header中  */
            httpHeader["Host"] = tmp;

        }else{
            printf("not do right now!\n");
        }

        return INCOMPLETE_REQUEST;
    };


    /**
     * 整个请求的入口
     * @param buffer
     * @param checked_index
     * @param checkStatus
     * @param read_index
     * @param start_line
     * @return
     */
    HTTP_CODE parse_content(char* buffer, int &checked_index, CHECK_STATUS &checkStatus,
                                int &read_index, int &start_line
                            ){
        LINE_STATUS lineStatus = LINE_OK;
        HTTP_CODE retCode = INCOMPLETE_REQUEST;

        while ( (lineStatus = parse_line(buffer, checked_index, read_index)) == LINE_OK){

            char *tmp = buffer + start_line;
            start_line = checked_index;

            switch (checkStatus){
                case CHECK_REQUEST_LINE:{
                    retCode = parse_request_line(tmp, checkStatus);
                    if (retCode == BAD_REQUEST)
                        return BAD_REQUEST;
                    break;
                }

                case CHECK_HEADER:{
                    retCode = parse_content(tmp);

                    if (retCode == BAD_REQUEST )
                        return BAD_REQUEST;
                    if (retCode == GET_REQUEST)
                        return GET_REQUEST;
                    break;
                }
                default:{
                    return INTERNAL_ERROR;
                }
            }
        }
        if ( lineStatus == LINE_INCOMPLETE )
            return INCOMPLETE_REQUEST;
        else
            return BAD_REQUEST;
    }


};



#endif //WEBSERVER_HTTPHANDLER_H
