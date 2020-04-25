#include <tinyserver/eventLoop.h>
#include <tinyserver/http/HttpRequest.h>
#include <tinyserver/http/HttpResponse.h>
#include <tinyserver/http/HttpServer.h>
#include <tinyserver/logger.h>
#include <tinyserver/sockets.h>

#include <iostream>
#include <fcntl.h>
#include <fstream>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <unistd.h>

#include "MimeTypes.h"

using namespace tinyserver;
using namespace tinyserver::http;

#define CORE_SIZE   1024 * 1024 * 500

void handleError404(HttpResponse* rsp)
{
    rsp->setStatusCode(404);
    rsp->setStatusMessage("Not Found");
    rsp->setTextBody("<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\"content=\"width=device-width, initial-scale=1.0\"><title>404</title></head><body><h1>404 Not Found</h1></body></html>");
    rsp->setKeepAlive(false);
}

void onHttpRequest(const HttpRequest &req, HttpResponse* rsp)
{
    std::string reqInfo;
    reqInfo.append("\n=====" + req.versionString() + " " + req.methodString() + "=====\n");
    reqInfo.append("=====HEADERS=====\n");
    for (auto &&v : req.headers())
    {
        reqInfo.append("===" + v.first + ": " + v.second + "\n");
    }
    reqInfo.append("=====PATH: " + req.path() + " =====\n");
    reqInfo.append("=====QUERIES=====\n");
    for (auto &&v : req.queries())
    {
        reqInfo.append("===" + v.first + ": " + v.second + "\n");
    }
    reqInfo.append("=====BODY=====\n" + req.body() + "\n================================\n");
    TLOG_DEBUG(reqInfo);

    std::string reqPath = req.path();
    if(reqPath == "/echo") {
        std::string content = req.query("content");
        if(content == "") {
            rsp->setTextBody("Please use: echo?content=echoContent");
        } else {
            rsp->setTextBody(content);
        }
    } else {
        auto filePath = "doc" + reqPath;

        struct stat sbuf;
        if (::stat(filePath.c_str(), &sbuf) < 0) {
            handleError404(rsp);
            TLOG_DEBUG("stat error");
            return;
        }
        if(S_ISDIR(sbuf.st_mode)) {
            filePath += "/index.html";
            if (::stat(filePath.c_str(), &sbuf) < 0) {
                handleError404(rsp);
                TLOG_DEBUG("stat error");
                return;
            }
        }

        auto fileLen = static_cast<size_t>(sbuf.st_size);

        const char* fileType = MimeTypes::getType(filePath.c_str());
        if (fileType) {
            rsp->setContentType(fileType);
        }

        int fileFd = open(filePath.c_str(), O_RDONLY, 0);
        if (fileFd < 0) {
            handleError404(rsp);
            TLOG_DEBUG("open error");
            return;
        }

        void *mmapRet = mmap(nullptr, fileLen, PROT_READ, MAP_PRIVATE, fileFd, 0);
        sockets::Close(fileFd);
        if (mmapRet == (void*)-1) {
            munmap(mmapRet, fileLen);
            handleError404(rsp);
            return;
        }

        rsp->setResponseData(mmapRet, fileLen);
        munmap(mmapRet, fileLen);
    }
}

int main(int argc, char** argv)
{
    Logger::Init("log.props");
    struct rlimit rlmt;
    rlmt.rlim_cur = (rlim_t)CORE_SIZE;
    rlmt.rlim_max  = (rlim_t)CORE_SIZE;
    if (setrlimit(RLIMIT_CORE, &rlmt) == -1) {
        return -1; 
    }
    uint16_t port = 8080;
    int ch;
    while ((ch = ::getopt(argc, argv, "l:d:")) != -1)
    {
        switch (ch)
        {
        case 'l':
            port = static_cast<uint16_t>(std::stoi(optarg));
            break;
        case 'd':
            Logger::SetLevel(static_cast<Logger::Level>(std::stoi(optarg)));
            break;
        
        default:
            break;
        }
    }
    
    EventLoop loop;
    InetAddress address(port);
    HttpServer httpServer(&loop, address);
    httpServer.setIOThreadNum(2);
    httpServer.setHttpCallback(onHttpRequest);
    httpServer.start();
    loop.loop();
    return 0;
}