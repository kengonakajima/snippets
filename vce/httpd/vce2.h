
#ifdef WIN32

#include <windows.h>

#elif linux || defined(__APPLE__)

#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>

#endif

#ifndef NO_STL
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#endif

#pragma once

namespace vce
{
	////�������X�V
	unsigned static const int build=87;////build
	unsigned static const int release=5;////release
	unsigned static const int version_date=20060607;////version_date
	////�����܂�
#ifdef WIN32

	typedef unsigned __int64 QWORD;

#elif linux || defined(__APPLE__)

	typedef unsigned long long QWORD;

#endif

#ifdef WIN32
	typedef unsigned long DWORD;
#else
	typedef unsigned int DWORD;
#endif
	typedef unsigned short WORD;
	typedef unsigned char BYTE;

#ifdef WIN32
	typedef wchar_t wchar;
#else
	typedef unsigned short wchar;
#endif
	typedef char utf8;

	enum VCE_EXCEPTION
	{
		VCE_EXCEPT_UNKNOWN=0,//!�s��
		VCE_EXCEPT_CREATESOCKETFAIL,//!Socket�쐬�����ς�
		VCE_EXCEPT_BINDFAIL,//!�|�[�g���蓖�Ď��s
		VCE_EXCEPT_LISTENINGFAIL,//!Listening���s
		VCE_EXCEPT_CONNECTFAIL,//!�ڑ����s
		VCE_EXCEPT_NAMERESOLUTIONFAIL,//!���O�������s
		VCE_EXCEPT_RECVBUFFER_FULL,//!��M�o�b�t�@�������ς�
		VCE_EXCEPT_SENDBUFFER_FULL//!���M�o�b�t�@�������ς�
	};

	enum VCE_STATE
	{
		VCE_STATE_UNKNOWN=0,//!�s��
		VCE_STATE_CONNECTING,//!�ڑ���...
		VCE_STATE_PREPROCESS,//!���낢��...
		VCE_STATE_ESTABLISHED,//!������
		VCE_STATE_LISTENING,//!���X�j���O��...
		VCE_STATE_CLOSED//!����ꂽ
	};

	enum VCE_CLOSEREASON
	{
		VCE_CLOSE_UNKNOWN=0,//!�s��
		VCE_CLOSE_LOCAL,//!��������؂���
		VCE_CLOSE_REMOTE,//!����ɐ؂�ꂽ
		VCE_CLOSE_RECVBUFFERFULL,//!��M�o�b�t�@�������ς��ɂȂ��Ă�
		VCE_CLOSE_RECVERROR,//!��M�G���[
	};

	class Base;
	class Session;
	class Listener;

	//! VCE���C���N���X
	/*!
	���ڐ�������vceCreate()�̖߂�l���󂯎��悤�ɂ���B
	���ׂĂ��g���I���폜����Ƃ���vceDelete�ɓn���B
	�����������鎖���o����B�ʏ�͈��������Ηǂ��B

	*/

	class VCE//abstract
	{
	public:
		//!VCE�̃��b�Z�[�W�|�[�����O
		/*!
		����I�ɌĂяo�����ɂ��VCE�������b�Z�[�W�����o���B
		���̃��\�b�h���Ăяo�����X���b�h����e�N���X�̃��[�U��`���z�֐����Ăяo�����B
		*/
		virtual void Poll()=0;

		//!VCE�̃��b�Z�[�W�|�[�����O����Wait
		/*!
        false�ɐݒ肷�邱�Ƃɂ��Poll�֐����Ăяo�����Ƃ��ɉ�����������f�[�^�������Ă�Wait��������Ȃ��悤�ɂ���B
		�f�t�H���g�ł�true�Ȃ̂�Poll�݂̂����[�v�����Ă�������CPU���\�[�X��Ɛ肵�Ȃ��悤�ɂȂ��Ă���B
		��������f�[�^������Ƃ���Wait�Ԋu���Z���Ȃ�̂łЂƂ̃A�v���P�[�V������CPU��Ɛ肵�Ă��ǂ��Ƃ���false�ɁB
		wait��true�̂܂܂ł�100MBPS�̏������x�͏\������B
		*/
		virtual void SetPollingWait(bool iswait)=0;

		//!�ڑ�����
		/*!
		�Z�b�V�������g��hostname:port�ɐڑ�����B
		\param s Session��h���������N���X�ւ̃|�C���^��n���B�n�����|�C���^�͓����ŊǗ�����A�����Ŏg�p����Ȃ��Ȃ�Ɛ؂藣�����B
		\param hostname �ڑ���z�X�g���Ⴕ����IP�A�h���X���L�q����������ւ̃|�C���^
		\param port �ڑ���TCP�|�[�g�ԍ�
		\param timeout �ڑ������݂鎞�ԁA�~���b�P�ʁB����߂��Ă��Ȃ���Ȃ�������EXCEPTION�R�[���B0�Ȃ炸���Ǝ��s����
		\sa Session
		\return ���j�[�NID
		*/
		virtual DWORD Connect(Session *s,const char *hostname,WORD port,unsigned int timeout=0)=0;
		virtual DWORD Connect(Session *s,const wchar *hostname,WORD port,unsigned int timeout=0)=0;
		//!�ڑ���҂��󂯂�
		/*!
		port�����X�j���O(�҂�����)����B
		\param l ���X�j���O�̑҂������Ɏg�����X�i�[�N���X�ւ̃|�C���^��n���B�n�����|�C���^�͓����ŊǗ�����A�g�p����Ȃ��Ȃ�Ɛ؂藣�����B
		\param port ���X�j���O����TCP�|�[�g�ԍ����w�肷��
		\param bindaddress ���X�j���O����IP�A�h���X���w�肷��BNULL�Ȃ玩���̎��S�A�h���X�B�܂��z�X�g�������ɑΉ����ĂȂ�
		\return ���j�[�NID
		*/
		virtual DWORD Listening(Listener *l,WORD port,const char *bindaddress=NULL)=0;

		//!�C���^�[�t�F�C�X�I�u�W�F�N�g���擾����
		/*!
		param uID �Z�b�V�����܂��̓��X�i�[��uID
		return �L���ȃZ�b�V�����܂��̓��X�i�[�A�����ȏꍇ��NULL
		sa uid_cast
		*/
		virtual void*Get(DWORD uID)=0;

		template <typename T> T uid_cast(DWORD uID)
		{
			return (T)Get(uID);
		}

		//!�Z�b�V����/���X�j���O�I�u�W�F�N�g��؂藣��
		/*!
		Connect��Listening�ɓn�����I�u�W�F�N�g��VCE�̓�����������؂藣���B
		Detach���ꂽ�I�u�W�F�N�g����API�͎g�p�ł��Ȃ��Ȃ�B
        �ʏ��Base�N���X���̃f�X�g���N�^����Ă΂��̂Ŗ����I��Detach����K�v�͂Ȃ��B
		\param s �؂藣���I�u�W�F�N�g
		\sa Connect
		\sa Listening
		*/
		virtual void Detach(Base *s)=0;

		//!�g�p����Listener/Session�̐���Ԃ�
		virtual unsigned int GetUsed()=0;

		//!���O�o�͗p�X�g���[��
		/*!
		\return NULL�������͏o�̓X�g���[��
		*/
		virtual std::ostream *SetLogger(std::ostream *target)=0;
		virtual std::ostream *Log()=0;
//		inline std::stringstream& Log_Warning(){return GetLogStream(0);}
	};

	//Utility
	//!16�i��2�����𐔒l�ɂ���
	/*!
	"10"��16 "aa"��170 "FF"��255
	*/
	int XXtoi(const char *XX);

	//selfIP(s)
	size_t GetSelfAddress(unsigned int *iparray,size_t array_qt);


	//!DWORD�ȃA�h���X�𕶎���ɕϊ�
	/*!
	IPv4�A�h���X�𕶎���ɕϊ�����B
	�߂�l��const char*�ŁA�������͓����Ŋm�ۂ��Ă���̂��g���܂킷�̂ŘA���Ŏg���Ƃ��͒��ӂ���B
	���������e�͎��ɌĂ΂��܂ŗL��
	param ip �A�h���X
	return ������
	sa StringToAddr
	*/
	const char* AddrToString(DWORD ip);
	//!IP�A�h���X�𐔒l�ɕϊ�����
	/*!
	param ip �A�h���X�B"192.168.1.2"�̂悤�ȁB
	return �������Ȃ��Ƃ�0xffffffff���Ԃ�B
	sa AddrToString
	*/
	DWORD StringToAddr(const char *ip);

	//!�G���f�B�A���ϊ�
	/*!
	bin�̃G���f�B�A���i�o�C�g�I�[�_�[�j��ϊ�����B
	\param bin �ϊ�����32�r�b�g�l
	\return �ϊ����ꂽ32�r�b�g�l
	*/
	DWORD bswap(DWORD bin);
	//!�G���f�B�A���ϊ�
	/*!
	bin�̃G���f�B�A���i�o�C�g�I�[�_�[�j��ϊ�����B
	\param bin �ϊ�����16�r�b�g�l
	\return �ϊ����ꂽ16�r�b�g�l
	*/
	WORD xchg(WORD bin);

	//!����������
	/*!
	�������͈͓����m�̌���
	\param p ��������郁�����ʒu�ւ̐擪�|�C���^
	\param sz_p ��������郁�����T�C�Y
	\param cmp �������郁�����ʒu�ւ̐擪�|�C���^
	\param sz_cmp �������郁�����T�C�Y
	\return ���������ꍇ�͌��������ꏊ�ւ̐擪�|�C���^��Ԃ��B����ȊO�̏ꍇ��NULL
	*/
	void *memmem(const void *p,size_t sz_p,const void *cmp,size_t sz_cmp);
	//!�����񁨐��l�ϊ�
	/*!
	������𐔎��ɕϊ�����W���֐���atoi�֐��̗�����
	�W���֐���atoi�قǐ��x�͍����Ȃ������Ȑ����̂�int�^�֕ϊ��\
	\param str �ϊ����镶����
	\return ���l�A����ɕϊ��o���Ȃ������ꍇ��0
	*/
	int atoi(const char *str);

	//!���Ԃ��擾����
	/*!
	�~���b�P�ʂ̎��Ԃ��擾����B32�r�b�g�Ȃ̂�49����1��J�E���^���������B
	\return ����
	*/
	DWORD GetTime();

	//!�v���O�������~�߂�
	/*!
	�~���P��(���x�r��)�Ńv���O�������~�߂�B�~�܂��Ă�Ԃ�CPU�͑��̃v���Z�X/�X���b�h�̏������o����B
	\param millsec �~�߂鎞��
	*/
	void ThreadSleep(DWORD millsec);

	//!������̃o�C�g���𒲂ׂ�BNull�I�[�����B
	/*
	strlen�̃o�b�t�@�T�C�Y�w��ŁB�ʏ��strlen��NULL�I�[����ĂȂ��ꍇ�o�b�t�@�I�[�o�[�������Ă��܂��B
	*/
//	size_t strnlen(const char *str,size_t buflen=(size_t)-1);
	size_t strlen_s(const char *str,size_t buflen);
	size_t strlen(const char *str);
	//!�o�b�t�@�̒��̕�����len�ȉ����`�F�b�N����
	/*!
	������̒����𒲂ׂĔ��f�܂ł��Ă����B
	*/
	bool isstrinbuf(const BYTE *start,const BYTE *end,size_t lesslen);
	//!��������R�s�[����
	/*!
	strcpy�̃o�b�t�@�T�C�Y������
	*/
	void strncpy(char *dest,size_t destlen,const char *src,size_t srclen);
	//!��������R�s�[����
	/*!
	strcpy�̃o�b�t�@�T�C�Y������
	*/
	void strcpy_s(char *dest,size_t destlen,const char *src);
	//!�������ǉ�����
	/*!
	\return �ǉ�����������̏I�[
	*/
	char *strpush(char *dest,const char *src);
	//!UTF8������̕������𒲂ׂ�
	/*!
	UTF8�͈ꕶ���ɂP�`�S�o�C�g�g�p���Ă�̂ŁA�������𐔂��ɂ����B����ȂƂ��͂��̊֐����g���B
	\param utf8string UTF8������
	\param buflen �o�b�t�@�T�C�Y
	\return ������
	*/
	size_t utfstrnlen(const utf8 *utf8string,size_t buflen);
	//!UTF16�̕������𒲂ׂ�
	/*!
	UTF16�̕������𒲂ׂ�B�T���Q�[�g�y�A�ɑΉ����ĂȂ�
	*/
	size_t wcslen(const wchar *str);
	//!UTF16��8�̕ϊ�������
	/*!
	UTF16��8�̕ϊ�������
	\param dest �o�͐��char�z��
	\param destlen �o�͐�̃o�b�t�@�T�C�Y
	\param src UTF16�̕�����
	*/
	size_t utf16to8(utf8 *dest,size_t destlen,const wchar *src);
	//!UTF8��16�̕ϊ�������
	/*!UTF8��16�̕ϊ�������
	\param dest �o�͐��wchar�z��
	\param destlen �o�͐�̃o�b�t�@�T�C�Y(�o�C�g�P��)
	\param src UTF8�̕�����
	*/
	size_t utf8to16(wchar *dest,size_t destlen,const utf8 *src);
	//STL�g��Utl
	//!�ݒ�t�@�C����ǂݍ��݃}�b�v����
	/*!
	name=value �`���̕������conf["name"]==value�̏�Ԃ�map����B#�L��������Ƃ���������͖��������
	\param filename �ݒ�t�@�C��
	*/
	std::map<std::string,std::string> LoadSimpleConfig(const char *filename);
	//!�ݒ�t�@�C����ۑ�����
	/*!
	������
	*/
	bool SaveSimpleConfig(const char *filename,std::map<std::string,std::string> &config);
	//!���Ԃ𕶎���Ŏ擾����
	/*!
    "YYYY/MM/DD hh:mm:ss.msec"�`���̕�������擾����B���O�o�͂Ƃ��Ɏg��
	\return ���Ԃ̕�����
	*/
	const char* NowTimeString();

	//!������𕪂���
	/*!
	�uaaa bbb "c c c"�v������΁A�O�ɕ�������B��d���p���ň͂߂�ȊO�̓G�X�P�[�v�����͖����B
	\return ������ꂽ������
	*/
	std::vector<std::string> SplitString(const char *string);
	//!POST������𕪂���
	/*!
    �uaaa=bb&ccc=a%20bc�v������Γ�ɕ�������B%XX�͏o�R�[�h�����B
	*/
	std::map<std::string,std::string> SplitPost(const char *post);

	//!URI�G���R�[�h����
	/*!
	a-z,A-Z,0-9���������ׂāB
	*/
	std::string encodeURI(const char *text);
	//!URI���f�R�[�h����
	/*!
	*/
	std::string decodeURI(const char *text);


	//!VCE���������N���X
	/*!
	�����ŊǗ������邽�߂̃x�[�X�N���X
	���[�U�����̃N���X�𒼐ڎg�p���邱�Ƃ͂Ȃ�
	*/

	class Base
	{
	protected:
		Base();
		virtual ~Base();
	public:
		QWORD cache;//������:(
		//!�ŗL���ʎq
		DWORD uID;
		//!�Ǘ���
		VCE *api;
		
		virtual void Attached();
		//!VCE�����Ǘ�����؂藣���ꂽ�Ƃ��ɌĂ΂��
		/*!
		Connect��Listening���ɓn�����N���X���g�p����Ȃ��Ȃ�ƌĂ΂��B
		Detach���ꂽ��Ȃ�delete���鎖���ł���B
		*/
		virtual void Detached();
		//!�v�l�֐�
		/*!
		vce::Poll���Ă΂ꂽ�Ƃ��Ɉꏏ�ɂ��̊֐����Ă΂��B�I�[�o�[���C�h�\�B
		*/
		virtual void Think();

		bool operator ==(Base &target);
		bool operator !=(Base &target);
		//!�����I�u�W�F�N�g���`�F�b�N����
		bool IsEqual(Base &target);

		//!��Ԃ��擾����
		/*!
		\sa VCE_STATE
		*/
		virtual VCE_STATE GetState();

		//!��O�擾�p�֐�
		/*!
		�N���e�B�J���łȂ���O���N�������Ƃ��ɌĂ΂��B
		\sa VCE_EXCEPTION
		*/
		virtual void Exception(VCE_EXCEPTION type);

		//!socket�n���h�����擾����
		int GetSocket();

		//!VCE�����Ŏg���Ă邩�ǂ���(delete������_�����ǂ���)
		bool IsUsing();
	};


	//!VCE���X�i�[�N���X
	/*!
	Listenig����Ă���|�[�g�֐ڑ��������Ƃ��ɓ�����`���邽�߂̃N���X�B������T�[�o�B
	\sa vce::Listening
	\sa AutoAccepter
	*/
	class Listener
		:public Base
	{
	public:
		Listener();
		virtual ~Listener();
		//!�ڑ��v��
		/*!
		���̊֐����I�[�o�[���C�h���A�ڑ����󂯂�Z�b�V�����N���X��Ԃ��B�ڑ������ۂ���ꍇ��NULL��Ԃ�
		\param remote_addr �ڑ����Ă����A�h���X
		\param remote_port �ڑ����Ă����|�[�g
		\return NULL�Ȃ狑��
		*/
		virtual Session *Accepter(DWORD remote_addr,WORD remote_port)=0;

		void Close();
		//!�Z�b�V�������A���X�g�����
		/*!
		���̃��X�i�[����h�������Z�b�V�����ꗗ���擾����B
		\param child �󂯎��|�C���^�z��
		\param array_maxlen �󂯎��z��ɓ������ő�v�f��
		\return �Z�b�V�����̐�,�Ⴕ���͔z��ɃR�s�[������
		*/
		unsigned int GetChilds(Session **child,unsigned int array_maxlen);
		unsigned int GetChilds();
		VCE_STATE GetState();
#ifndef NO_STL
		template <typename T> void GetChilds(std::vector<T*> &ch)
		{
			ch.resize(GetChilds());
			GetChilds((Session**)&ch[0],(unsigned int)ch.capacity());
		}
#endif
	};

	//!VCE�X�g���[���Z�b�V�����N���X
	/*!
	�Z�b�V�����𑀍삷��̂Ɏg���B���̃N���X���p�����A�e���z�֐����I�[�o�[���C�h���Ă����B
	\sa vce::Connect
	\sa Listener::Accept
	*/

	class Session
		:public Base
	{
	public:
		enum LowLevelFlags
		{
			LLF_NONE=0,
			LLF_ENCRYPT_DEFAULT=1,//������
		}llflags;

		//!���X�i�[�ւ̃|�C���^
		/*!
		�p�b�V�u�ڑ��̏ꍇ�̓��X�j���O���Ă��e�N���X�ւ̃|�C���^������B
        �A�N�e�B�u�ڑ��̏ꍇ��NULL
		*/
		Listener *parent;

		Session();
		virtual ~Session();

		//!�Z�b�V���������
		/*!
		��������B

		\sa Close
		\sa Closed
		*/
		void ForceClose();
		//!���M�\��̃f�[�^�𑗐M���I�������Z�b�V���������
		/*!
		�����ؒf�����A���M�o�b�t�@����ɂȂ�Ƒ��M����A��M���I���Ǝ�M������B
		�P���̃��N�G�X�g���o���A����ɑ΂��郊�v���C�������󂯎�鎞���Ɏg���ƕ֗��B
		\sa ForceClose
		\sa Closed
		*/
		void Close();
		//!�ڑ����s����߂�
		/*!
		Connect�������Ƃ�timeout��҂����ɂ�����߂�Ƃ��Ɏg���B���̌��Exception��ConnectFail�R�[�������B
        \sa vce::Connect
		*/
        void CancelConnecting();
		//!�f�[�^�𑗐M����
		/*!
		�f�[�^�𑗐M�o�b�t�@�ɏ������݁A���M����B
		\param p ���M�f�[�^�ւ̐擪�|�C���^
		\param sz ���M�f�[�^�̃o�C�g�T�C�Y�̒���
		\return ���M�����f�[�^���Asz�̒l�����̂܂ܕԂ�
		*/
		size_t Send(const void *p,size_t sz);
		//!��M�o�b�t�@�����b�N���A��M�o�b�t�@�̃f�[�^���Q�Ƃ���
		/*!
		��M�o�b�t�@�̓��e�𒼐ڎQ�Ƃ���ɂ͂��̊֐����g���B
		���b�N������K���A�����b�N���Ȃ���΂Ȃ�Ȃ��B

		\param sz OUT,��M�o�b�t�@�̃o�C�g�T�C�Y�̒���
		\return ��M�o�b�t�@�ւ̐擪�|�C���^�Asz�܂ł̊Ԃ͎Q�Ƃ̂݉\�BNULL�̏ꍇ�͎��s�B
		\sa Unlock_Readbuf
		*/
		const BYTE *Lock_Readbuf(size_t &sz);
		//!��M�o�b�t�@���A�����b�N���A��M�o�b�t�@�̃f�[�^���폜����
		/*!
		���b�N������M�o�b�t�@���A�����b�N����̂Ɏg���B
		�܂���M�o�b�t�@�̐擪����̃f�[�^�𓯎��ɍ폜���鎖���o����B
		\param shrink �폜����o�C�g�T�C�Y�̒���
		*/
		void Unlock_Readbuf(size_t shrink);

		//!�ڑ������������Ƃ��ɌĂ΂��
		virtual void Connected();
		//!�ڑ��������Ƃ��ɌĂ΂��
		virtual void Closed(VCE_CLOSEREASON type);
		//!�Ȃ�炩�̃f�[�^����M�����Ƃ��ɌĂ΂��
		/*!
		���̉��z�֐����I�[�o�[���C�h����M�����������B
		TCP�Z�b�V�����Ȃ��M����TCP�X�g���[���̃f�[�^�����̂܂ܗ���B
	      
		\param p ��M�f�[�^�ւ̃|�C���^
		\param sz ��M�f�[�^�̃o�C�g�T�C�Y
		\return ���������o�C�g���A0�Ȃ��M�o�b�t�@�̃f�[�^�͍폜���Ȃ��B1�ȏ�Ȃ�o�b�t�@�̐擪����o�C�g�P�ʂō폜����B
		*/
		virtual size_t Recv(const BYTE *p,size_t sz);

		//!�ڑ����IPv4TCP�A�h���X���擾����
		/*!
		\param addr IP�A�h���X���i�[����ϐ��ւ̎Q��
		\param port �|�[�g�ԍ����i�[����ϐ��ւ̎Q��
		\return �擾�ł�����true�A�ł��Ȃ�������false
		*/
		bool GetRemoteAddress(DWORD &addr,WORD &port);

		//!�ڑ�����IPv4TCP�A�h���X���擾����
		/*!
		\param addr IP�A�h���X���i�[����ϐ��ւ̎Q��
		\param port �|�[�g�ԍ����i�[����ϐ��ւ̎Q��
		\return �擾�ł�����true�A�ł��Ȃ�������false
		*/
		bool GetLocalAddress(DWORD &addr,WORD &port);

		//!���M�o�b�t�@�̎g�p��
		/*!
		���M�o�b�t�@���g�p�ʂ�������ƃ������Ċm�ۂ��������邽�߁A�V�r�A�ȏ󋵂ł͂��̊֐��Ń`�F�b�N���Ȃ��瑗�M����悤�ɂ���B
		\return ���M�o�b�t�@�g�p��
		*/
		size_t GetUsageSendBuffer();

		VCE_STATE GetState();
	};

	//!�����Z�b�V�����J�n�N���X
	/*!
	�ڑ��������Ƃ��Ɏ����ŃT�[�o�p�Z�b�V�����𐶐����A�Ԃ����߂̃e���v���[�g�N���X�B
	���̃N���X�𐶐���Listening�ɒ��ړn�������ł悢�B
	\sa net::Listening
	*/
	template <class T>class AutoAccepter:public Listener
	{
		bool enable;
		const unsigned int maxconnections;
	protected:
		virtual Session *Accepter(DWORD remote_addr,WORD remote_port){
			if(maxconnections&&GetChilds()>=maxconnections)
				return NULL;
			return enable?new T:NULL;};
	public:
		AutoAccepter(int maxconn=0):maxconnections(maxconn){
			enable=true;}
		//!����̐ڑ���������B
		void Enable(){
			enable=true;}
		//!����̐ڑ������ۂ���B
		void Disable(){
			enable=false;}
		void Detached(){
			delete this;}
	};

	//!VCE�p�P�b�g���C���[�N���X
	/*!
	Session�̔h���N���X�ŃX�g���[���f�[�^���p�[�X/�}�[�W���A�p�P�b�g�f�[�^�ɂ��ăf�[�^�̑���M������B
	���M(�}�[�W)�����p�P�b�g�T�C�Y���A��M(�p�[�X)����ď������鎖���ł���B
	�X�g���[���̏ꍇ�A5�o�C�g��2�񑗐M�����10�o�C�g�����؂�Ɏ�M����A�f�[�^�ɋ�؂肪�����̂ŁA��؂�Ƃ��͂�������g���B
	*/
	class Codec
		:public Session
	{
	private:
		//!�����Œ�`����Ă����M�֐��B�g��Ȃ��B
		size_t Recv(const BYTE *p,size_t sz);
	protected:
		//!�p�[�X���ꂽ�p�P�b�g����M���邽�߂̊֐�
		/*!
		�p�[�X����A�p�P�b�g�P�ʂ̃f�[�^��������ƌĂ΂��B
		\param p ��M�f�[�^�ւ̃|�C���^
		\param sz 
		*/
		virtual void Parsed(const BYTE *p,size_t sz)=0;
	public:
		//!�f�[�^���}�[�W���A���M����
		/*!
		�f�[�^�𑗐M����Ƃ��Ɏg���Bsz�T�C�Y���̃f�[�^����M���ɓ��B���A�p�[�X�����
		\prama p ���M����f�[�^�ւ̃|�C���^
		\param sz ���M����f�[�^�̃o�C�g�T�C�Y�̒���
		\return ���s�����-1�A���������ꍇ��0�ȏ�̃p�P�b�g�T�C�Y
		*/
		virtual int Merge(const void *p,size_t sz);
	};

	//!Multi Protocol Interface Node.
	class MPINode
		:public Codec
	{
		friend class MPISession;
	public:
		MPINode(unsigned char number);
		class MPISession *pnode;
		const unsigned char MPINumber;
	public:
		int Merge(const void *p,size_t sz);
	};

	//!Multi Protocol Interface System
	class MPISession
		:public Codec
	{
		std::map<unsigned char,MPINode*> node;
	protected:
		void Parsed(const BYTE *p,size_t sz);
	public:
//		int Merge(const void *p,size_t sz,unsigned char MPInum);
		void Register(MPINode *node);
		template <typename N>N*GetNode(){for(std::map<unsigned char,MPINode*>::iterator i=node.begin();i!=node.end();i++)if(dynamic_cast<N*>(i->second))return dynamic_cast<N*>(i->second);return NULL;}
	protected://wrapper
		void Attached();//�Snode��Attached���Ă�
		void Connected();//�ȉ�����
		void Closed(VCE_CLOSEREASON type);
		void Detached();
		void Exception(VCE_EXCEPTION type);
		void Think();
	};
	//!�P��HTTP�T�[�o
	/*!
	*/
	class HTTPServer
		:public Session
	{
		size_t Recv(const vce::BYTE *p,size_t sz);
	protected:
		virtual bool Request(std::string method,std::string URI,std::map<std::string,std::string> header,const char *body,size_t body_len)=0;
		void Response(int status,std::map<std::string,std::string> header,const char *body,size_t body_len);
	};



	//Monitor���g���ɂ�dynamic_cast��L���ɂ��Ȃ���΂Ȃ�Ȃ�
	//!VCE���p�N���X�A�����������邽�߂̃��j�^�[
	/*!
	HTTP���g���u���E�U�ォ��VCE�̏󋵂����邱�Ƃ��o����B
	vce->Listening(new vce::AutoAccepter<vce::Monitor>,8080);�Ƃ����8080�ԃ|�[�g���g�����j�^�����O�ł���
	*/
	class Monitor
		:public Session
	{
		char lastpath[1024];
	public:
		Monitor();
		~Monitor();
	private:
		void Connected();
		void Closed(VCE_CLOSEREASON type);
		void Exception(VCE_EXCEPTION type);
		size_t Recv(const BYTE *p,size_t sz);
		void SendHTML(const char *str);
		void SendText(const char *str);
		void Reply(const char *path,const char *get_param,const char *post);
	};

	//!Monitor Extension
    /*!
	���j�^�[�Ɗ֘A���Ă��낢��g�����߂̃x�[�X�N���X�BSession��Listener�ƈꏏ�ɑ��d�p�����邱�ƂŎg����悤�ɂȂ�Bdynamic_cast�������ɂȂ��Ă�Ǝg���Ȃ��B
	*/
	class MonitorConsole
	{
		friend class Monitor;
	protected:
		MonitorConsole();
		virtual ~MonitorConsole();
		//!�R�}���h���󂯎�����Ƃ��ɌĂ΂��B
		virtual void Request(const char *command );
	public:
		//!���O�o�͗p�̃X�g���[��
		std::stringstream clog;
	};

	//!Monitor ���܂�
	/*!
    Monitor�Ƃ����MonitorConsole�����������I�}�P�N���X�B
	������p������Request���Ē�`����B
	*/
	class MonitorConsoleListener
		:public AutoAccepter<Monitor>
		,public MonitorConsole
	{
	};

	class MonitorStatusDetail
	{
		friend class Monitor;
	protected:
		MonitorStatusDetail();
		~MonitorStatusDetail();
		//!�R�}���h�Ƃ͕ʂ�POST������
		virtual void Post(const char *post);
	public:
		//!�ڍ׃y�[�W�p�̃X�e�[�^�XHTML
		std::stringstream html;
	};

	//!vce�I�u�W�F�N�g�𐶐�����
	/*!
	�܂�VCE�I�u�W�F�N�g�������API���g����悤�ɂ���B������邱�Ƃ��o����
	*/
	VCE *VCECreate(std::ostream *log=NULL);
	//!vce�I�u�W�F�N�g���폜����
	/*!
	VCECreate�ō��ꂽ�I�u�W�F�N�g�͕K�����̊֐��ō폜���Ȃ���΂Ȃ�Ȃ��B�������J����e��㏈��������
	*/
	void VCEDelete(VCE *n);
}
