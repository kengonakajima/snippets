#include "vce2.h"

class htsv:public vce::HTTPServer
{
	bool Request(std::string method,std::string URI,std::map<std::string,std::string> header,const char *body,size_t body_len)
	{
		std::map<std::string,std::string> res_header;
		std::string res;
		if(method=="GET"&&URI=="/index.html")
		{
			res+="<html><body>-INDEX-</body></html>";
			res_header["Content-type"]="text/html";
		}
		else
		{
			res+="method="+method+"\r\n";
			res+="URI="+URI+"\r\n";
			res+="UA="+header["User-Agent"]+"\r\n";        
			res_header["Content-type"]="text/plain";
		}

		res_header["Server"]="test";
		Response(200,res_header,res.data(),res.length());
		return true;
	}
};

int main()
{
	vce::VCE *api=vce::VCECreate();
	api->Listening(new vce::AutoAccepter<htsv>,10080);
	while(true)
		api->Poll();
	vce::VCEDelete(api);
}