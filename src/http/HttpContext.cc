#include "HttpContext.h"
#include <fstream>
#include "Logger.h"

#include "FormDataParser.h"

#define save_pic_path "../../root/picture/"

// 假设这个方法在 parseRequest 中被调用，且已经解析出了图片数据



// 解析请求行
bool HttpContext::processRequestLine(const char *begin, const char *end)
{
    bool succeed = false;
    const char *start = begin;
    const char *space = std::find(start, end, ' ');

    // 不是最后一个空格，并且成功获取了method并设置到request_
    if (space != end && request_.setMethod(start, space))
    {
        // 跳过空格
        start = space+1;
        // 继续寻找下一个空格
        space = std::find(start, end, ' ');
        if (space != end)
        {
            // 查看是否有请求参数
            const char* question = std::find(start, space, '?');
            if (question != space)
            {
                // 设置访问路径
                request_.setPath(start, question);
                // 设置访问变量
                request_.setQuery(question, space);
            }
            else
            {
                request_.setPath(start, space);
            }
            start = space+1;
            // 获取最后的http版本
            succeed = (end-start == 8 && std::equal(start, end-1, "HTTP/1."));
            if (succeed)
            {
                if (*(end-1) == '1')
                {
                    request_.setVersion(HttpRequest::kHttp11);
                }
                else if (*(end-1) == '0')
                {
                    request_.setVersion(HttpRequest::kHttp10);
                }
                else
                {
                    succeed = false;
                }
            }
        }
    }  
    return succeed;
}

// return false if any error
bool HttpContext::parseRequest(Buffer* buf, Timestamp receiveTime)
{
    bool ok = false;
    bool hasMore = true;
    while (hasMore)
    {
        // 请求行状态
        if (state_ == kExpectRequestLine)
        {
            // 找到 \r\n 位置
            const char* crlf = buf->findCRLF();
            if (crlf)
            {
                // 从可读区读取请求行
                // [peek(), crlf + 2) 是一行
                ok = processRequestLine(buf->peek(), crlf);
                if (ok)
                {
                    request_.setReceiveTime(receiveTime);
                    // readerIndex 向后移动位置直到 crlf + 2
                    buf->retrieveUntil(crlf + 2);
                    // 状态转移，接下来解析请求头
                    state_ = kExpectHeaders;
                }
                else
                {
                    hasMore = false;
                }
            }
            else
            {
                hasMore = false;
            }
        }
        // 解析请求头
        else if (state_ == kExpectHeaders)
        {
            const char* crlf = buf->findCRLF();
            if (crlf)
            {
                // 找到 : 位置
                const char* colon = std::find(buf->peek(), crlf, ':');
                if (colon != crlf)
                {
                    // 添加状态首部
                    request_.addHeader(buf->peek(), colon, crlf);
                }
                else // colon == crlf 说明没有找到 : 了，直接返回 end
                {
                    // empty line, end of header
                    // FIXME:
                    state_ = kExpectBody;
                    // hasMore = false;
                }
                buf->retrieveUntil(crlf + 2);
            }
            else
            {
                hasMore = false;
            }
        }
        // 在请求体解析部分调用此方法
        else if (state_ == kExpectBody)
        {
            // 解析请求体
            //解析请求方法
            std::cout << "A"<<request_.method() << std::endl;
            if(request_.method() == HttpRequest::Method::kPost){
                int contentLength = stoi(request_.getHeader("Content-Length"));
                std::string contenttype = request_.getHeader("Content-Type");
                std::cout<< "B"<< "   "<<contentLength<< "    " << buf->readableBytes()<< std::endl;
            
                if (contentLength > 0 ){
                    if(contenttype.find("multipart/form-data") != -1)
                    {
                        //解析普通表单数据
                        std::string pattern = "boundary=";
                        int i = contenttype.find("; " + pattern, 0);
                        i += pattern.size() + 2;
                        std::string boundary = contenttype.substr(i);
                        std::shared_ptr<std::string> sptr_data(new std::string(buf->retrieveAllAsString()));
                        contentLength -= sptr_data->size();
                        while(contentLength > 0){
                            int *saveerrno;
                            buf->readFd(buf->fd(), saveerrno);
                            std::string line = buf->retrieveAsString(buf->readableBytes());
                            sptr_data->append(line);
                            contentLength -= line.size();
                        }
                        FormDataParser fdp(sptr_data, 0, "--" + boundary);
                        auto p = fdp.parse();

                        for (std::vector<FormItem>::iterator it = p->begin(); it != p->end(); ++it) {
                            std::string filename = (*(it)).getFileName();
                            if (filename != "") {  // 如果文件名不为空，那么说明是个文件，那么就存储为test.zip
                                std::string content = (*(it)).getContent();
                                saveImageToFile(content, save_pic_path+filename);
                                if(request_.getBodyForm(filename) == "")
                                    request_.addBodyForm(filename,save_pic_path+filename);
                                else
                                    LOG_INFO("文件已存在");
                            } else {
                                if(request_.getBodyForm((*(it)).getName()) == "")
                                    request_.addBodyForm((*(it)).getName(),(*(it)).getContent());
                                else
                                    LOG_INFO("文件已存在");
                            }

                            // std::cout << (*(it)).getName() <<std::endl; //<< (*(it)).getContent() <<std::endl;
                        }
                        state_ = kGotAll;
                        hasMore = false; // 请求处理完毕
                    }else{
                        std::cout<< "ContentType:"<< contenttype << std::endl;
                        request_.setBody(std::move(buf->retrieveAsString(contentLength)));
                        state_ = kGotAll;
                        hasMore = false; // 请求处理完毕
                    }
                }else{
                    LOG_ERROR("解析请求体失败");
                    hasMore = false;
                }
            }else{
                state_ = kGotAll;
                hasMore = false;
            }
        }
    }
    return ok;
}



void HttpContext::saveImageToFile(const std::string& data, const std::string& filename)
{
    std::ofstream outFile(filename, std::ios::binary);
    if (outFile)
    {
        outFile.write(data.data(), data.size());
        outFile.close();
    }
    else
    {
        // 处理文件打开失败的情况
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
    }
}

