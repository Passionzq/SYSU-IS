#include "ans.hpp"

string read_from_file(string filename)
{
  string base64;
  char buf[1024];
  ifstream f(filename);
  assert(f.is_open()); // 检测文件是否打开

  while (!f.eof()) {
    f.getline(buf,1024);

    // 证书的开始标识符为“BEGIN CERTIFICATE”。
    if (string(buf).find("BEGIN CERTIFICATE") != -1) {
      continue;
    }
    // 证书的结束标识符为“END CERTIFICATE”
    else if(string(buf).find("END CERTIFICATE") != -1) {
      return base64;
    }
    // 证书的BASE64格式正文
    else {
      base64.append(buf);
    }
  }
}

void print_time_format(string timeStr) 
{
  cout << "20" << timeStr[0] << timeStr[1] << "年" << timeStr[2] << timeStr[3] << "月"
       << timeStr[4] << timeStr[5] << "日" << timeStr[6] << timeStr[7] << ":"
       << timeStr[8] << timeStr[9] << ":" << timeStr[10] << timeStr[11];
}

void print_hex_format(uchar* data, int len) 
{
  cout << hex;
  for (int i = 0; i < len; i++) {
    cout << ((data[i] & 0xf0) >> 4) << (data[i] & 0xf)<<" ";
    if (i % 30 == 29 ) cout << endl;
  }
  cout << dec << endl;
}

void print_ans()
{
  for (int i = 0; i < ansData.size(); i++) {
    ANS item = ansData[i];

    if (!strcmp(item.title.c_str(), "Version")) {
      item = ansData[++i];
      if (item.type == 0x02) {
        cout << "证书版本: ";
        cout << "V" << item.data[0] + 1 << endl;
        item = ansData[++i];
        cout << "序列号: ";
        print_hex_format(item.data, item.len);
      } 
      else {
        i--;
      }
    } 
    else if (title2string.find(item.title) != title2string.end()) {
      cout << title2string[item.title] << endl;
    } 
    else if (title2hex.find(item.title) != title2hex.end()) {
      string title = title2hex[item.title];
      item = ansData[++i];
      if (item.data != NULL) {
        cout << title;
        print_hex_format(item.data, item.len);
      } 
      else {
        i--;
      }
    } 
    else if (title2next.find(item.title) != title2next.end()) {
      cout << title2next[item.title];
      item = ansData[++i];
      cout << item.title << endl;
    } 
    else if (algorithm_object.find(item.title) != algorithm_object.end()) {
      cout << "加密算法: " << algorithm_object[item.title]<<endl;
      item = ansData[++i];
      if (item.type == 0x03) {
        cout << "\n公钥: " << endl;
        print_hex_format(item.data, item.len);
      } 
      else {
        i--;
      }
    } 
    else if (!strcmp(item.title.c_str(), "0x00")) {
      cout << "Public Key: " << endl;
      print_hex_format(item.data, item.len);
    } 
    else if (!strcmp(item.title.c_str(), "2.5.29.17")) {
      cout << "主体别名: ";
      item = ansData[++i];
      cout << item.title;
      item = ansData[++i];
      while (item.type == 0x82) {
        cout << ", " << item.title;
        item = ansData[++i];
      }
      i--;
      cout << endl;
    } 
    else if (item.title.length() > 7 && !strcmp(item.title.substr(0, 7).c_str(), "UTCTime")) {
      cout << "有效期: ";
      string beginTime = item.title.substr(7, item.title.length() - 8);
      print_time_format(beginTime);
      cout << " - ";
      item = ansData[++i];
      string endTime = item.title.substr(7, item.title.length() - 8);
      print_time_format(endTime);
      cout << endl;
    } 
  }
}

void get_ANS(uchar* data, int begin, int end) 
{
  LEN lens;
  int temp, hold, type, i = begin;
  uchar* text;
  string title;

  while (i < end) {
    int type = data[i++], k = i;

    lens.stored_len = 1;
    if (data[k] & 0x80) {
      int len = 0, len_len = data[k] & 0x7f;
      k++;
      for (int j = 0; j < len_len; j++) {
        len <<= 8;
        len += data[k + j];
      }
      lens.len = len;
      lens.stored_len += len_len;
    } 
    else {
      lens.len = data[k] & 0x7f;
    }

    if (i + lens.stored_len <= end) {
      i += lens.stored_len;
    }

    if (lens.len <0 || i + lens.len > end) {
      break;
    }
    
    title = "";
    switch (type) {
      case 0x00:
        text = new uchar[end - begin];
        for (int t = begin + 1; t < end; t++) {
          text[t - begin - 1] = data[t];
        }
        ansData.push_back({"0x00", end - begin - 1, text, type});
        break;

      case 0x01:  // 布尔类型
        temp = 0xff;
        for (int t = 0; t < lens.len; t++) {
          temp &= data[i + t];
        }
        ansData.push_back(
            {temp == 0 ? "False" : "True", lens.len, NULL, type});
        break;
      
      case 0x02:  // 整数类型
      case 0x80:  // 直接输出
        text = new uchar[lens.len];
        for (int t = 0; t < lens.len; t++) {
          text[t] = data[i + t];
        }
        ansData.push_back({title, lens.len, text, type});
        break;

      case 0x03:  // Bit String 类型
        text = new uchar[lens.len - 1];
        for (int t = 0; t < lens.len - 1; t++) {
          text[t] = data[i + t + 1];
        }
        ansData.push_back({"", lens.len - 1, text, type});
        break;

      case 0x04:  // OCTET STRING
        get_ANS(data, i, i + lens.len);

      case 0x05:
        break;

      case 0x06:  // Object Identifier
        title = "";
        temp = data[i] & 0x7f;
        hold = min(temp / 40, 2);
        title.append(to_string(min(temp / 40, 2)));
        title.append(".");
        title.append(to_string(temp - 40 * hold));
        title.append(".");
        hold = 2;
        temp = 0;
        for (int t = 1; t < lens.len; t++) {
          temp <<= 7;
          temp += data[i + t] & 0x7f;
          if (!(data[i + t] & 0x80)) {
            title.append(to_string(temp));
            title.append(".");
            hold++;
            temp = 0;
          }
        }
        ansData.push_back(
            {title.substr(0, title.length() - 1), lens.len, NULL, type});
        break;

      case 0x30:  // 结构体序列
        get_ANS(data, i, i + lens.len);
        break;

      case 0x31:  // Set序列
        get_ANS(data, i, i + lens.len);
        break;

      case 0xa0:  // 证书版本
        title = "Version";
        ansData.push_back({title, lens.len, NULL, type});
        get_ANS(data, i, i + lens.len);
        break;

      case 0xa3:  // 扩展字段
        title = "Extension";
        ansData.push_back({title, lens.len, NULL, type});
        get_ANS(data, i, i + lens.len);
        break;

      case 0x17:  // 时间戳
        title = "UTCTime";
      case 0x13:  // 字符串
      case 0x82:  // subjectUniqueID
      case 0x16:  // IA5String类型
      case 0x0c:  // UTF8String类型
      case 0x86:  // 特殊IA5String类型
        for (int t = 0; t < lens.len; t++) {
          title += (char)data[i + t];
        }
        ansData.push_back({title, lens.len, NULL, type});
        break;
      
      default:
        i--;
        if (i + lens.len > end) {
          text = new uchar[end - begin];
          for (int t = 0; t < end - begin; t++) {
            text[t] = data[i + t];
          }
          ansData.push_back({"", end - begin, text, type});
        } 
        else {
          text = new uchar[lens.len];
          for (int t = 0; t < lens.len; t++) {
            text[t] = data[i + t];
          }
          ansData.push_back({"", lens.len, text, type});
        }
        i = end;
    }
    i += lens.len;
  }
}

void parseX509(string data) 
{
  // 对BASE64进行补位
  while (data.length() % 4 != 0) {
    data.append("=");
  }

  // BASE64中的一位是6bits， 6*4 = 3*8
  int index = 0, len = (data.length() / 4) * 3;
  uchar text[len] = {0};

  for (int i = 0; i < data.length(); i += 4) {
    uchar base64_6bits[4];

    // 找到BASE64中的字符对应的位置
    for (int j = 0; j < 4; j++) {
      base64_6bits[j] = base64_chars.find(data[i + j]);
    }
    // 将四个6bits的二进制数转换成三个8bits(1byte 也即uchar)
    text[index + 0] = ((base64_6bits[0] & 0x3f) << 2) | ((base64_6bits[1] & 0x30) >> 4);
    text[index + 1] = ((base64_6bits[1] & 0xf) << 4) | ((base64_6bits[2] & 0x3c) >> 2);
    text[index + 2] = ((base64_6bits[2] & 0x3) << 6) | (base64_6bits[3] & 0x3f);
    index += 3;
  }

  ansData.clear();
  get_ANS(text, 0, len);

  print_ans();
}