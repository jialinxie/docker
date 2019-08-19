#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <curl/easy.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

char ipA[16] = {0};

struct LocalIP{
    std::string ipAddr;
};

void to_json(json &j, const LocalIP &localIP){
    j = {
        {"ip", localIP.ipAddr}
    };
}

void getIP(){
    char *buf = (char *)malloc(sizeof(char) * 16);
    FILE *fp;
    fp = popen("curl -s ifconfig.me", "r");
    if(fp == NULL){
        printf("fail to popen\n");
        exit(1);
    }

    while(fgets(buf, 15, fp) != NULL){
        // printf("%s\n", buf);
    }
    pclose(fp);

    memcpy(ipA, buf, 16);

    free(buf);
}

static size_t write_callback(void *ptr, size_t size, size_t nmemb, void *userp) {
  size_t realsize = size * nmemb;
  auto j = json::parse(std::string((char *) ptr));
  std::cout << j << "\n";
  LocalIP *favor = (LocalIP *) userp;
  favor->ipAddr = j["ip"];
  return realsize;
}

int getIpFromServer(){
        char buf[16] = {0};
        LocalIP lIP;
        LocalIP receiveIP;
        getIP();
        lIP.ipAddr = ipA;

        CURLcode code = CURLE_FAILED_INIT;
        string url = "104.248.214.167:1111/wantip";
//        string url = "127.0.0.1:1111/wantip";
        CURL *curl;
        curl = curl_easy_init();
          if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());       //提交表单的URL地址

            //libcur的相关POST配置项
            curl_easy_setopt(curl, CURLOPT_POST, 1L); // POST
            //将返回结果通过回调函数写到自定义的对象中
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &receiveIP);

            code = curl_easy_perform(curl);
            if (code != CURLE_OK) {
              fprintf(stderr, "curl_easy_perform() failed: %s\n",
                      curl_easy_strerror(code));
              return code;
            } /*else {
              std::cout << "get ip from server :" << receiveIP.ipAddr << "\n";
            }*/
            curl_easy_cleanup(curl);
          }
          return code;
}

int pushIP2Server(){
    char buf[16] = {0};
    LocalIP lIP;
    LocalIP receiveIP;
    getIP();
    lIP.ipAddr = ipA;
    json j;
    j = lIP;
    string dump = j.dump();
    cout << dump << '\n';

    CURLcode code = CURLE_FAILED_INIT;
    string url = "104.248.214.167:1111/ip";
//    string url = "127.0.0.1:1111/ip";
    CURL *curl;
    curl = curl_easy_init();
      if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());       //提交表单的URL地址

        //libcur的相关POST配置项
        curl_easy_setopt(curl, CURLOPT_POST, 1L); // POST
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, dump.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, dump.size());

        //将返回结果通过回调函数写到自定义的对象中
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &receiveIP);

        code = curl_easy_perform(curl);
        if (code != CURLE_OK) {
          fprintf(stderr, "curl_easy_perform() failed: %s\n",
                  curl_easy_strerror(code));
          return code;
        } else {
          std::cout << "ip:" << receiveIP.ipAddr << "\n";
        }
        curl_easy_cleanup(curl);
      }
      return code;
}

int main(int argc, char** argv){
    // pushIP2Server();

   getIpFromServer();

    return 0;
}
