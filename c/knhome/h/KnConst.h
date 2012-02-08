/*
 *  constant declaration for KUNOICHI API
 */

#ifndef _KN_CONST_H_
#define _KN_CONST_H_

#include "KnLendef.h"

#define KNM_ROUNDUP(n,r)	((((n)+((r)-1))/(r))*(r))
#define	KND_LEN_ALIGN		4
#define KNM_ALIGN(n)		(((n)/(KND_LEN_ALIGN)+1)*(KND_LEN_ALIGN))
#define KNM_ALIGN2(n)		KNM_ROUNDUP(n, KND_LEN_ALIGN)

/*
 *  system parameters
 */
#ifdef BIG_CORE
#define	KND_MAXUSER		3000	/* maximum number of users */
#else
#define	KND_MAXUSER		50	/* for test */
#endif
#define	KND_PAYLOAD_MAXLEN	1024	/* user payload max size */
#if defined(WIN32)
#define	KND_THREAD_STACK_SIZE	32768
#else
#define	KND_THREAD_STACK_SIZE	0	/* use system default */
#endif /* WIN32 */

/*
 * length of system parameters 
 */
#define KND_PASSWORD_LEN	15	/* user password length */
#define KND_USERID_LEN		11	/* user identifier length */
#define KND_PASSWORD_DIGEST	16	/* password digest length */
#define	KND_FILEPATH_MAXLEN	256

/*
 *  module identifiers
 */
#define	KND_MOD_CN	0x41	/* client network API */
#define	KND_MOD_CS	0x44	/* client space management API */
#define	KND_MOD_CP	0x45	/* client portable library */
#define	KND_MOD_SN	0x01	/* server network API */
#define	KND_MOD_SU	0x02	/* server user management API */
#define	KND_MOD_SD	0x03	/* server data management API */
#define	KND_MOD_SS	0x04	/* server space management API */
#define	KND_MOD_SP	0x05	/* server portable library */
#define	KND_MOD_UTL	0x21	/* utility library */
#define	KND_MOD_BI	0x22	/* big int library */
#define	KND_MOD_PT	0x23	/* portable library */
#define	KND_MOD_MAIN	0x11	/* main module */

#endif /* _KN_CONST_H_ */
