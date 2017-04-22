#include "MyUtilities.h"

// ColorPalette�N���X�̐ÓI�萔�����o������
const int ColorPalette::Red    = GetColor( 255,   0,   0 );
const int ColorPalette::Blue   = GetColor(   0,   0, 255 );
const int ColorPalette::Green  = GetColor(   0, 255,   0 );
const int ColorPalette::Cyan   = GetColor(   0, 255, 255 );
const int ColorPalette::Yellow = GetColor( 255, 255,   0 );

// ��� - ��[�����̃^�C�v
void DrawArrow2D( Vector2D bgn, Vector2D end, unsigned int Color, int FillFlag, int Thickness )
{
	static const double AllowHeadHight=10.0f; // ���̖�̕����̍���
	static const double AllowHeadWidth= 6.0f; // ���̖�̕����̕��i1/2�j

	static Vector2D PosL( -AllowHeadHight,  AllowHeadWidth );
	static Vector2D PosR( -AllowHeadHight, -AllowHeadWidth );

	// bgn �� end �����܂�ɂ��������ƕςȂ��Ƃ��N����
	// ���`���������߂āA������������`�悷��
	if( (end-bgn).sqlen() < 0.01 ){
		DrawLine2D( bgn.toPoint(), end.toPoint(), Color, Thickness );
		return ;
	}

	// �ϊ���̊��x�N�g�����v�Z
	Vector2D head2D = (end-bgn).normalize();
	Vector2D side2D = head2D.side();

	// ���̎O�p�`�̈ʒu�����[�J�����W�����[���h���W�ɕϊ� 
	Vector2D psl = Vector2DToWorldSpace( PosL, head2D, side2D ) + end;
	Vector2D psr = Vector2DToWorldSpace( PosR, head2D, side2D ) + end;

	// �`��

	if( FillFlag )
	{
		Vector2D newend=end-(AllowHeadHight-1)*head2D; // ���������Ȃ�Ɩ_�̕��������̐�[����͂ݏo��̂Ŗ_�̒��������������
		DrawLine2D( bgn.toPoint(), newend.toPoint(), Color, Thickness ); // �{��
		DrawTriangle2D( psl.toPoint(), end.toPoint(), psr.toPoint(), Color, TRUE );
	}
	else
	{
		DrawLine2D( bgn.toPoint(), end.toPoint(), Color, Thickness ); // �{��
		DrawLine2D( psl.toPoint(), end.toPoint(), Color, Thickness ); // ���ӂP
		DrawLine2D( psr.toPoint(), end.toPoint(), Color, Thickness ); // ���ӂQ
	}

};