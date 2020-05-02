#ifndef DESH
#define DESH
#include <string>
using namespace std;

/*DES.h: declaration of the TDES、TBase64、TBase64DES class. */
/* TDES类说明：该类是DES和3DES算法类 */
class CDES {
public:
	CDES();
	virtual ~CDES();
	static char* HexToStr(string s);
	static string StrToHex(char* bytes, int bytelength);
	
	static bool EnCode();  //des加密
	static bool DeCode(); //des解密
protected:
	typedef bool(*PSubKey)[16][48];
	
	//计算并填充子密钥到SubKey数据中  
	static void SetSubKey(PSubKey pSubKey, const unsigned char Key[8]);
	
	//DES单元运算  
	static void DES(unsigned char Out[8], const unsigned char In[8], const PSubKey pSubKey, bool Type);
	
	/* 补足8位数据
	* Description: 根据协议对加密前的数据进行填充
	* @param nType: 类型：PAD类型
	* @param In: 数据串指针
	* @param Out: 填充输出串指针
	* @param datalen: 数据的长度
	* @param padlen: (in,out)输出buffer的长度，填充后的长度
	* @return true--成功；false--失败；
	*/
	static bool RunPad(bool bType, int nType, const unsigned char* In, unsigned datalen, unsigned char* Out, unsigned& padlen);
	
	/* 执行DES算法对文本加解密
	* Description    : 执行DES算法对文本加解密
	* @param bType   : 类型：加密ENCRYPT，解密DECRYPT
	* @param bMode   : 模式：ECB,CBC
	* @param In      : 待加密串指针
	* @param Out     : 待输出串指针
	* @param datalen : 待加密串的长度，同时Out的缓冲区大小应大于或者等于datalen
	* @param Key     : 密钥(可为8位,16位,24位)支持3密钥
	* @param keylen  : 密钥长度，多出24位部分将被自动裁减
	* @return true--成功；false--失败；
	*/
	static bool RunDES(bool bType, bool bMode, int PaddingMode, const unsigned char* IV, const unsigned char* In,
		unsigned char* Out, unsigned datalen, const unsigned char* Key, unsigned keylen);

private:
	/*static int hexCharToInt(char c);*/
	enum {
		ENCRYPT = 0,    // 加密  
		DECRYPT,          // 解密  
	};

	enum {
		ECB = 0,      // ECB模式  
		CBC             // CBC模式  
	};
	  
	enum {
		PAD_ISO_1 = 0,  // ISO_1填充：数据长度不足8比特的倍数，以0x00补足，如果为8比特的倍数，补8个0x00  
	};
}; 
#endif
