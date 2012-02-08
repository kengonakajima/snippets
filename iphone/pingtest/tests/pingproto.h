#include "vce2.h"

#pragma once
#include "vce2serialize.h"


class ping_proto
	:public vce::Codec
{
protected:
	void Parsed(const vce::VUint8 *dat,size_t sz);
public://functions
	enum FUNCTIONS
	{
		FUNCTION_PING,
		FUNCTIONS_END
	};
public://err codes
	enum RECVERROR
	{
		RECVERROR_INVALID_FUNCTION,
		RECVERROR_INVALID_FUNCTIONFORMAT,
		RECVERROR_DISABLE_FUNCTION,
		RECVERROR_CALLLIMIT
	};
protected:
	void Attached();
	void Connected();
	void Closed(vce::VCE_CLOSEREASON type);
	virtual void RecvException(FUNCTIONS func,RECVERROR code);
	void StatusUpdate();
public:
	ping_proto();
public://enum
public://const

protected://variables
	unsigned int LastUpdate;
	bool SendFunctionEnable[FUNCTIONS_END];
	bool RecvFunctionEnable[FUNCTIONS_END];
	struct _FunctionStatus
	{
		_FunctionStatus();
		void Update();
		unsigned int RecvCallCount;
		unsigned int RecvCallLimit;
		unsigned int RecvCallLimitTime;
		unsigned int DecreaseTime;
	};
	_FunctionStatus FunctionStatus[FUNCTIONS_END];
	void SetRecvFunctionCallLimit(FUNCTIONS func,unsigned int limit,unsigned int limittime);
public://size of length
	static const vce::VUint32 size_of_bool=1;
	static const vce::VUint32 size_of_int=4;
	static const vce::VUint32 size_of_short=2;
	static const vce::VUint32 size_of_char=1;
	static const vce::VUint32 size_of_dword=4;
	static const vce::VUint32 size_of_word=2;
	static const vce::VUint32 size_of_byte=1;
	static const vce::VUint32 size_of_float=4;
	static const vce::VUint32 size_of_double=8;
	static const vce::VUint32 size_of_qword=8;
	static const vce::VUint32 size_of_string=1;
	static const vce::VUint32 size_of_unicode=4;
	static const vce::VUint32 size_of_vce_dc_VBOOL=sizeof(vce::VBOOL);
	static const vce::VUint32 size_of_vce_dc_utf8=sizeof(vce::utf8);
	static const vce::VUint32 size_of_vce_dc_VSint8=sizeof(vce::VSint8);
	static const vce::VUint32 size_of_vce_dc_VUint8=sizeof(vce::VUint8);
	static const vce::VUint32 size_of_vce_dc_VSint16=sizeof(vce::VSint16);
	static const vce::VUint32 size_of_vce_dc_VUint16=sizeof(vce::VUint16);
	static const vce::VUint32 size_of_vce_dc_VSint32=sizeof(vce::VSint32);
	static const vce::VUint32 size_of_vce_dc_VUint32=sizeof(vce::VUint32);
	static const vce::VUint32 size_of_vce_dc_VSint64=sizeof(vce::VSint64);
	static const vce::VUint32 size_of_vce_dc_VUint64=sizeof(vce::VUint64);
public://struct
	//struct for send
public://config
	void SendEnable(FUNCTIONS func,bool enable);
	void RecvEnable(FUNCTIONS func,bool enable);
protected://recv func
	virtual void recv_ping(vce::VSint32 time);//<  
public://send func
	bool send_ping(vce::VSint32 time);//<  
public://other
	static const unsigned int xml_crc=0x2e4690a1;
	static const unsigned int generated_time=0x4a07e50b;
  std::ostream* getLog() const
  {
    return log;
  }

 void setLog(std::ostream *new_log)
 {
   log = new_log;
 }

 std::ostream *log;
};

class ping_proto_server
	:public ping_proto
{
protected:
	void Attached();
protected://recv func
public://send func
};

class ping_proto_client
	:public ping_proto
{
protected:
	void Attached();
protected://recv func
public://send func
};



namespace vce_gen_serialize
{
}
