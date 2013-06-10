/*!	@file
	@brief �v���O�C���ݒ�_�C�A���O�{�b�N�X

	@author Uchi
	@date 2010/3/22
*/
/*
	Copyright (C) 2010, Uchi

	This software is provided 'as-is', without any express or implied
	warranty. In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose, 
	including commercial applications, and to alter it and redistribute it 
	freely, subject to the following restrictions:

		1. The origin of this software must not be misrepresented;
		   you must not claim that you wrote the original software.
		   If you use this software in a product, an acknowledgment
		   in the product documentation would be appreciated but is
		   not required.

		2. Altered source versions must be plainly marked as such, 
		   and must not be misrepresented as being the original software.

		3. This notice may not be removed or altered from any source
		   distribution.
*/
#ifndef SAKURA_CDLGPLUGINOPTION_7BD4A901_BC40_4CA1_8311_85B8CAA783F08_H_
#define SAKURA_CDLGPLUGINOPTION_7BD4A901_BC40_4CA1_8311_85B8CAA783F08_H_

#include "dlg/CDialog.h"
#include "plugin/CPluginManager.h"

class CPropPlugin;

/*!	@brief �u�v���O�C���ݒ�v�_�C�A���O

	���ʐݒ�̃v���O�C���ݒ�ŁC�w��v���O�C���̃I�v�V������ݒ肷�邽�߂�
	�g�p�����_�C�A���O�{�b�N�X
*/

// �ҏW�ő咷
#define MAX_LENGTH_VALUE	1024

typedef std::wstring wstring;

// �^ 
static const wstring	OPTION_TYPE_BOOL = wstring( L"bool" );
static const wstring	OPTION_TYPE_INT  = wstring( L"int" );
static const wstring	OPTION_TYPE_SEL  = wstring( L"sel" );
static const wstring	OPTION_TYPE_DIR  = wstring( L"dir" );

class CDlgPluginOption : public CDialog
{
public:
	/*
	||  Constructors
	*/
	CDlgPluginOption();
	~CDlgPluginOption();

	/*
	||  Attributes & Operations
	*/
	int DoModal( HINSTANCE, HWND, CPropPlugin*, int );	/* ���[�_���_�C�A���O�̕\�� */

protected:
	/*
	||  �����w���p�֐�
	*/
	BOOL	OnInitDialog( HWND, WPARAM wParam, LPARAM lParam );
	BOOL	OnBnClicked( int );
	BOOL	OnNotify( WPARAM wParam, LPARAM lParam );
	BOOL	OnCbnSelChange( HWND hwndCtl, int wID );
	BOOL	OnEnChange( HWND hwndCtl, int wID );
	BOOL	OnActivate( WPARAM wParam, LPARAM lParam );
	LPVOID	GetHelpIdTable( void );

	void	SetData( void );	/* �_�C�A���O�f�[�^�̐ݒ� */
	int		GetData( void );	/* �_�C�A���O�f�[�^�̎擾 */

	void	ChangeListPosition( void );					// �ҏW�̈�����X�g�r���[�ɍ����Đؑւ���
	void	MoveFocusToEdit( void );					// �ҏW�̈�Ƀt�H�[�J�X���ڂ�
	void	SetToEdit( int );
	void	SetFromEdit( int );
	void	SelectEdit( int );							// �ҏW�̈�̐؂�ւ�
	void	SepSelect( wstring, wstring*, wstring* );	// �I��p�����񕪉�
	void	SelectDirectory( int iLine );				// �f�B���N�g����I������

private:
	CPlugin*		m_cPlugin;
	CPropPlugin*	m_cPropPlugin;
	int 			m_ID;			// �v���O�C���ԍ��i�G�f�B�^���ӂ�ԍ��j
	int				m_Line;			// ���ݕҏW���̃I�v�V�����s�ԍ�
	std::tstring	m_sReadMeName;	// ReadMe �t�@�C����
};

#endif /* SAKURA_CDLGPLUGINOPTION_7BD4A901_BC40_4CA1_8311_85B8CAA783F08_H_ */