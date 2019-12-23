#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <assert.h>
#include <map>
#include <vector>

using namespace std;

typedef unsigned char uchar;

typedef struct {
  int len;     
  int stored_len;  
} LEN;

typedef struct {
  string title;
  int len;
  uchar* data;
  int type;
} ANS;  

// 从cer文件中读取BASE64字符串
string read_from_file(string filename);

// 将字符串转换成“时间格式”来打印
void print_time_format(string timeStr);

// 将字符串转换成十六进制来打印 (序列号、公钥)
void print_hex_format(uchar* data, int len);

// 打印最终的结果
void print_ans(); 

// 对解码后的BASE64格式的X509进行解析
void get_ANS(uchar* data, int begin, int end);

// 根据传入过来的BASE64格式的X509进行解析
void parseX509(string data);

// BASE64编码中的元素
static const string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// 字符串打印
static map<string, string> title2string = {
    {"1.3.6.1.5.5.7.3.1", "服务器身份验证: True"},
    {"1.3.6.1.5.5.7.3.2", "客户端身份验证: True"},
    {"2.5.29.37", "扩展密钥用法:"},
    {"2.5.29.31", "CRL Distribution Points:"},
    {"1.2.840.10045.2.1", "EC Public Key:"},
    {"Extension", "扩展字段:"},
    {"2.23.140.1.2.2","组织验证:"},
    {"1.3.6.1.5.5.7.1.1", "AuthorityInfoAccess:"},
    {"2.5.29.19", "基本约束:"},
    {"1.3.6.1.5.5.7.3.2", "客户端身份验证: True"}
};

// 十六进制打印
static map<string, string> title2hex = {
    {"1.2.840.10045.3.1.7","推荐椭圆曲线域: \n"},
    {"2.5.29.35", "授权密钥标识符: "},
    {"2.5.29.14", "主体密钥标识符: "}
};

// 需要连续相关内容的打印
static map<string, string> title2next = {
    {"1.3.6.1.5.5.7.2.1", "OID for CPS qualifier: "},
    {"1.3.6.1.5.5.7.48.1", "OCSP: "},
    {"1.3.6.1.5.5.7.48.2", "id-ad-caIssuers: "},
    {"1.3.6.1.4.1.311.60.2.1.1", "所在地: "},
    {"1.3.6.1.4.1.311.60.2.1.3", "国家: "},
    {"1.3.6.1.4.1.311.60.2.1.2", "州或省: "},
    {"2.5.4.3", "通用名称: "},
    {"2.5.4.5", "序列号: "},
    {"2.5.4.6", "国家名: "},
    {"2.5.4.7", "位置名: "},
    {"2.5.4.8", "州省名: "},
    {"2.5.4.9", "街区地址: "},
    {"2.5.4.10", "组织名: "},
    {"2.5.4.11", "组织单位名: "},
    {"2.5.4.12", "标题: "},
    {"2.5.4.13", "描述: "},
    {"2.5.4.15", "业务类别: "},
    {"2.5.29.32", "证书策略: "},
    {"2.5.29.15", "使用密钥: "}
};

// 加密算法打印
static map<string, string> algorithm_object = {
    {"1.2.840.10040.4.1", "DSA"},
    {"1.2.840.10040.4.3" , "sha1DSA"},
    {"1.2.840.113549.1.1.1" ,"RSA"},
    {"1.2.840.113549.1.1.2" , "md2RSA"},
    {"1.2.840.113549.1.1.3" , "md4RSA"},
    {"1.2.840.113549.1.1.4" , "md5RSA"},
    {"1.2.840.113549.1.1.5" , "sha1RSA"},
    {"1.3.14.3.2.29", "sha1RSA"},
    {"1.2.840.113549.1.1.13", "sha512RSA"},
    {"1.2.840.113549.1.1.11","sha256RSA"}
};

static vector<ANS> ansData;