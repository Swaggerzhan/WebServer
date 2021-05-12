//
// Created by swagger on 2021/5/12.
//

#ifndef WEBSERVER_MIME_H
#define WEBSERVER_MIME_H

#include <map>
#include <iostream>
#include <string>

class Mime {

private:

    std::map<std::string, std::string> mime;

    typedef std::map<std::string, std::string>::iterator Iter;

public:

    Mime(){
        mime[".html"] = "text/html";
        mime[".avi"] = "video/x-msvideo";
        mime[".bmp"] = "image/bmp";
        mime[".c"] = "text/plain";
        mime[".doc"] = "application/msword";
        mime[".gif"] = "image/gif";
        mime[".gz"] = "application/x-gzip";
        mime[".htm"] = "text/html";
        mime[".ico"] = "image/x-icon";
        mime[".jpg"] = "image/jpeg";
        mime[".png"] = "image/png";
        mime[".txt"] = "text/plain";
        mime[".mp3"] = "audio/mp3";
        mime["default"] = "text/html";
    }

    std::string getAcceptType(std::string suffix){
        int pos = -1;
        for (int i=0; i<suffix.size(); i++){
            if (suffix[i] == '.')
                pos = i;
        }
        if (pos == -1 || pos == suffix.size())
            return mime[".html"];
        Iter iter = mime.find(suffix.substr(pos+1, suffix.size()-pos));
        if (iter == mime.end())
            return mime[".html"];
        return iter->second;
    }




};


#endif //WEBSERVER_MIME_H
