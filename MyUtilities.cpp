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
// �^����ꂽ�ʒu�ɁA�i���[���h���W����݁j�����[�J�����W�̊�ꎲ��\������֐�
void DrawCoordi(MATRIX M, double scale)
{
	Vector3D vUnitX(M.m[0][0], M.m[0][1], M.m[0][2]);
	Vector3D vUnitY(M.m[1][0], M.m[1][1], M.m[1][2]);
	Vector3D vUnitZ(M.m[2][0], M.m[2][1], M.m[2][2]);
	Vector3D vTgtPs(M.m[3][0], M.m[3][1], M.m[3][2]);

	// �^�[�Q�b�gpos��`�悷��
	DrawLine3D(vTgtPs.toVECTOR(), (vTgtPs + scale*vUnitX).toVECTOR(), ColorPalette::Red  ); // x����Ԑ��ŕ`��
	DrawLine3D(vTgtPs.toVECTOR(), (vTgtPs + scale*vUnitY).toVECTOR(), ColorPalette::Green); // y����ΐ��ŕ`��
	DrawLine3D(vTgtPs.toVECTOR(), (vTgtPs + scale*vUnitZ).toVECTOR(), ColorPalette::Blue ); // z������ŕ`��
};

// ###############################################
// ########## class VisualFootprint
// ###############################################

// �O�Ղ��L�^
void VisualFootprint::Update(Vector3D Pos)
{
	CurIndex = ++CurIndex%TrajectoryList.size();
	TrajectoryList[CurIndex] = Pos;
};

// �O�Ղ�`��
void VisualFootprint::Render()
{
	// �O�Ղ�`��
	for (int i = 1; i<TrajectoryList.size(); i++)
	{
		int s = (CurIndex + i) % TrajectoryList.size();
		int e = (CurIndex + i + 1) % TrajectoryList.size();
		DrawLine3D(
			TrajectoryList[s].toVECTOR(),
			TrajectoryList[e].toVECTOR(),
			m_iColor);
	}

};