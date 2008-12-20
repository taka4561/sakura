#include "stdafx.h"
#include "CCodeMediator.h"
#include "charset/charcode.h"
#include "charset/CESI.h"
#include "io/CBinaryStream.h"










/*!
	������̐擪��Unicode�nBOM���t���Ă��邩�H

	@retval CODE_UNICODE   UTF-16 LE
	@retval CODE_UTF8      UTF-8
	@retval CODE_UNICODEBE UTF-16 BE
	@retval 0              �����o

	@date 2007.08.11 charcode.cpp ����ړ�
*/
ECodeType CCodeMediator::DetectUnicodeBom( const char* pS, const int nLen )
{
	uchar_t *pBuf;

	if( NULL == pS ){ return CODE_NONE; }

	pBuf = (uchar_t *) pS;
	if( 2 <= nLen ){
		if( pBuf[0] == 0xff && pBuf[1] == 0xfe ){
			return CODE_UNICODE;
		}
		if( pBuf[0] == 0xfe && pBuf[1] == 0xff ){
			return CODE_UNICODEBE;
		}
		if( 3 <= nLen ){
			if( pBuf[0] == 0xef && pBuf[1] == 0xbb && pBuf[2] == 0xbf ){
				return CODE_UTF8;
			}
		}
	}
	return CODE_NONE;
}




/*!
	SJIS, JIS, EUCJP, UTF-8, UTF-7 �𔻒� (��)

	@return SJIS, JIS, EUCJP, UTF-8, UTF-7 �̉��ꂩ�� ID ��Ԃ��D

	@note �K�؂Ȍ��o���s��ꂽ�ꍇ�́Am_dwStatus �� CESI_MB_DETECTED �t���O���i�[�����B
*/
ECodeType CCodeMediator::DetectMBCode( CESI* pcesi )
{
//	pcesi->m_dwStatus = ESI_NOINFORMATION;

	if( pcesi->GetDataLen() < (pcesi->m_apMbcInfo[0]->nSpecific - pcesi->m_apMbcInfo[0]->nPoints) * 20 ){
		// �s���o�C�g�̊������A�S�̂� 0.05% �����ł��邱�Ƃ��m�F�B
		// �S�̂�0.05%�قǂ̕s���o�C�g�́A��������B
		pcesi->SetStatus( ESI_NODETECTED );
		return CODE_NONE;
	}
	if( pcesi->m_apMbcInfo[0]->nPoints < 1 ){
		pcesi->SetStatus( ESI_NODETECTED );
		return CODE_NONE;
	}

	/*
		����󋵂��m�F
	*/
	pcesi->SetStatus( ESI_MBC_DETECTED );

	if( pcesi->m_apMbcInfo[0]->eCodeID == CODE_EUC && pcesi->m_apMbcInfo[1]->eCodeID == CODE_SJIS ){
		if( pcesi->m_apMbcInfo[0]->nPoints > 0 && pcesi->m_nMbcSjisHankata > 0 ){
			// ������ EUCJP�A������ SJIS ���������Ă��āA
			// EUCJP �̃|�C���g���� 1 �ȏォ�� SJIS ���p�J�^�J�i�����o����Ă���ꍇ�́A
			// SJIS �̃|�C���g���ɔ��p�J�^�J�i�̕������Z����B
			pcesi->m_apMbcInfo[1]->nSpecific += pcesi->m_nMbcSjisHankata;
			pcesi->m_apMbcInfo[1]->nPoints += pcesi->m_nMbcSjisHankata;
			pcesi->SortMBCInfo();
			pcesi->SetStatus( ESI_MBC_HANKATACOUNTED );
		}
	}
	if( pcesi->m_apMbcInfo[0]->eCodeID == CODE_UTF8 && pcesi->m_bMbcCesu8Enabled ){
		pcesi->SetStatus( ESI_MBC_CESU8DETECTED );
	}

	return pcesi->m_apMbcInfo[0]->eCodeID;
}



/*!
	UTF-16 LE/BE �𔻒�.

	@retval CODE_UNICODE    UTF-16 LE �����o���ꂽ
	@retval CODE_UNICODEBE  UTF-16 BE �����o���ꂽ
	@retval 0               UTF-16 LE/BE �Ƃ��Ɍ��o����Ȃ�����

	@note CESI::GuessUtf16Bom �̌��ʂ��m�F���邾���B
*/
ECodeType CCodeMediator::DetectUnicode( CESI* pcesi )
{
//	pcesi->m_dwStatus = ESI_NOINFORMATION;

	if( pcesi->GetBOMType() == ESI_BOMTYPE_UNKNOWN ){
		pcesi->SetStatus( ESI_NODETECTED );
		return CODE_NONE;
	}

	pcesi->SetStatus( ESI_WC_DETECTED );
	return pcesi->m_aWcInfo[ pcesi->GetBOMType() ].eCodeID;
}




/*
	���{��R�[�h�Z�b�g����

	�߂�l�z2007.08.14 kobake �߂�l��int����ECodeType�֕ύX
	SJIS		CODE_SJIS
	JIS			CODE_JIS
	EUC			CODE_EUC
	Unicode		CODE_UNICODE
	UTF-8		CODE_UTF8
	UTF-7		CODE_UTF7
	UnicodeBE	CODE_UNICODEBE
*/
ECodeType CCodeMediator::CheckKanjiCode( const char* pBuf, int nBufLen )
{
	ECodeType nret;
	CESI* pcesi;

	try{
		pcesi = new CESI();
	}catch( ... ){
		pcesi = NULL;
	}
	if( pcesi == NULL ){
		return CODE_DEFAULT;
	}


	/*
		����󋵂́A
		DetectMBCode(), DetectUnicode() ����
		cesi.m_dwStatus �ɋL�^����B
	*/

	pcesi->SetInformation( pBuf, nBufLen, CODE_SJIS );
	nret = DetectUnicode( pcesi );
	if( nret != CODE_NONE && pcesi->GetStatus() != ESI_NODETECTED ){
		return nret;
	}
	nret = DetectMBCode( pcesi );
	if( nret != CODE_NONE && pcesi->GetStatus() != ESI_NODETECTED ){
		return nret;
	}
	return CODE_DEFAULT;
}

/*
	���{��R�[�h�Z�b�g����
*/
ECodeType CCodeMediator::CheckKanjiCode( CESI* pcesi )
{
	ECodeType nret;

	/*
		����󋵂́A
		DetectMBCode(), DetectUnicode() ����
		cesi.m_dwStatus �ɋL�^����B
	*/

	nret = DetectUnicode( pcesi );
	if( nret != CODE_NONE ){
		return nret;
	}
	nret = DetectMBCode( pcesi );
	if( nret != CODE_NONE ){
		return nret;
	}
	return CODE_DEFAULT;
}



/*
|| �t�@�C���̓��{��R�[�h�Z�b�g����
||
|| �y�߂�l�z2007.08.14 kobake �߂�l��int����ECodeType�֕ύX
||	SJIS		CODE_SJIS
||	JIS			CODE_JIS
||	EUC			CODE_EUC
||	Unicode		CODE_UNICODE
||	UTF-8		CODE_UTF8
||	UTF-7		CODE_UTF7
||	UnicodeBE	CODE_UNICODEBE
||	�G���[		CODE_ERROR
*/
ECodeType CCodeMediator::CheckKanjiCodeOfFile( const TCHAR* pszFile )
{
	// �I�[�v��
	CBinaryInputStream in(pszFile);
	if(!in){
		return CODE_ERROR;
	}

	// �f�[�^���擾
	int nBufLen = in.GetLength();
	if( nBufLen > CheckKanjiCode_MAXREADLENGTH ){
		nBufLen = CheckKanjiCode_MAXREADLENGTH;
	}

	// 0�o�C�g�Ȃ�SJIS����
	if( 0 == nBufLen ){
		return CODE_DEFAULT;
	}

	// �f�[�^�m��
	CMemory cMem;
	cMem.AllocBuffer(nBufLen);
	void* pBuf = cMem.GetRawPtr();

	// �ǂݍ���
	nBufLen = in.Read(pBuf, nBufLen);

	// �N���[�Y
	in.Close();

	// ���{��R�[�h�Z�b�g����
	ECodeType nCodeType = DetectUnicodeBom( reinterpret_cast<const char*>(pBuf), nBufLen );
	if( nCodeType == CODE_NONE ){
		// Unicode BOM �͌��o����܂���ł����D
		nCodeType = CheckKanjiCode( reinterpret_cast<const char*>(pBuf), nBufLen );
	}

	return nCodeType;
}