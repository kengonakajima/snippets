#include "pingproto.h"



#define AdjustAlign(n) ((((std::size_t)n)+3)/4*4)

ping_proto::ping_proto()
{
#ifndef VCE_GEN_NOLOG
	log=NULL;
#endif
	for(int i=0;i<FUNCTIONS_END;i++)
	{
		SendFunctionEnable[i]=true;
		RecvFunctionEnable[i]=true;
	}
}

ping_proto::_FunctionStatus::_FunctionStatus()
{
	RecvCallCount=0;
	RecvCallLimit=0;
	RecvCallLimitTime=0;
}

void ping_proto::_FunctionStatus::Update()
{
	unsigned int now=vce::GetTime();
	if(RecvCallLimit&&RecvCallLimitTime)
	{
		if(RecvCallCount)
		{
			int interval=(int)((float)RecvCallLimit/RecvCallLimitTime*1000);
			while(DecreaseTime<now)
			{
				if(RecvCallCount)
					RecvCallCount--;
				DecreaseTime+=interval;
			}
		}
		else
			DecreaseTime=now;
	}
}


void ping_proto::RecvEnable(FUNCTIONS func,bool enable)
{
	RecvFunctionEnable[func]=enable;
}

void ping_proto::SendEnable(FUNCTIONS func,bool enable)
{
	SendFunctionEnable[func]=enable;
}

void ping_proto::SetRecvFunctionCallLimit(FUNCTIONS func,unsigned int limit,unsigned int limittime)
{
	FunctionStatus[func].RecvCallLimit=limit;
	FunctionStatus[func].RecvCallLimitTime=limittime;
	FunctionStatus[func].DecreaseTime=vce::GetTime();
}

void ping_proto::StatusUpdate()
{
	for(int i=0;i<FUNCTIONS_END;i++)
		FunctionStatus[i].Update();
}

void ping_proto::Parsed(const vce::VUint8 *p,size_t sz)
{
	vce_gen_serialize::pack buf;
	buf.first=const_cast<vce::VUint8*>(p);
	buf.last=buf.first+sz;
	vce::VUint16 func_id;
	if(!vce_gen_serialize::Pull(func_id,buf)){RecvException(FUNCTIONS_END,RECVERROR_INVALID_FUNCTION);return;};
	StatusUpdate();
	switch(func_id)
	{
	//recvfunc ping
	case 101:{
		_FunctionStatus &status=FunctionStatus[FUNCTION_PING];
		if(!RecvFunctionEnable[FUNCTION_PING]){
			RecvException(FUNCTION_PING,RECVERROR_DISABLE_FUNCTION);break;}
		if(status.RecvCallLimit&&status.RecvCallLimitTime)
		{
			if(status.RecvCallCount>status.RecvCallLimit)
			{
				RecvException(FUNCTION_PING,RECVERROR_CALLLIMIT);
				break;
			}
			status.RecvCallCount++;
		}
		vce::VUint8 *work=api->BufferPush(AdjustAlign(size_of_int+size_of_dword));
		vce::VCE::AutoPop autopop(api);
		vce::AutoBuf<vce::VSint32> time(work);
		if(!vce_gen_serialize::Pull(time.var,buf)){RecvException(FUNCTION_PING,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(buf.first!=buf.last){
			RecvException(FUNCTION_PING,RECVERROR_INVALID_FUNCTIONFORMAT);break;}
		if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")->ping_proto::recv_ping( "<<"time="<<time<<" "<<") ["<<(int)(buf.first-p)<<"]byte"<<std::endl;
		recv_ping(time);
	break;}
	default:{
		RecvException(FUNCTIONS_END,RECVERROR_INVALID_FUNCTION);
		}break;
	}
}

void ping_proto::RecvException(FUNCTIONS id,RECVERROR code)
{
#ifndef VCE_GEN_NOLOG
	if(log)
		*log<<vce::NowTimeString()<<" ping_proto::RecvException(function="<<id<<",code="<<code<<") close...\r\n";
#endif

	ForceClose();
}

void ping_proto::Connected()
{
	vce::VUint32 addr;
	vce::VUint16 port;
	this->GetRemoteAddress(addr,port);
#ifndef VCE_GEN_NOLOG
	if(log)*log<<vce::NowTimeString()<<" ping_proto::Connected ("<<uID<<")from "<<vce::AddrToString(addr)<<":"<<port<<std::endl;
#endif
}

void ping_proto::Closed(vce::VCE_CLOSEREASON)
{
	vce::VUint32 addr;
	vce::VUint16 port;
	this->GetRemoteAddress(addr,port);
#ifndef VCE_GEN_NOLOG
	if(log)*log<<vce::NowTimeString()<<" ping_proto::Closed ("<<uID<<")from "<<vce::AddrToString(addr)<<":"<<port<<std::endl;
#endif
}

void ping_proto::Attached()
{
	vce::Codec::Attached();
}

void ping_proto::recv_ping(vce::VSint32)
{
}

bool ping_proto::send_ping(vce::VSint32 time)
{
	if(!SendFunctionEnable[FUNCTION_PING])
		return false;
	if(!api)return false;
	const std::size_t worksize=2+AdjustAlign(size_of_int+size_of_dword);
	vce::VUint8 *work=api->BufferPush(worksize);
	vce::VCE::AutoPop autopop(api);
	vce_gen_serialize::pack buf;
	buf.first=work;
	buf.last=buf.first+worksize;
	vce::VUint16 func_id=101;
	if(!vce_gen_serialize::Push(func_id,buf))return false;
	if(!vce_gen_serialize::Push(time,buf))return false;
	if(log)*log<<vce::NowTimeString()<<" (id:"<<uID<<")<-ping_proto::send_ping( "<<"time="<<time<<" "<<") ["<<(int)(buf.first-work)<<"]byte"<<std::endl;
	if(!Merge(work,buf.first-work))return false;
	return true;
}

void ping_proto_server::Attached()
{
	ping_proto::Attached();
}

void ping_proto_client::Attached()
{
	ping_proto::Attached();
}

