#pragma once
#include "Object.h"
#include "Transform.h"
#include "BitmapRender.h"

enum StoneType // 돌 종류
{
	White,
	Black,
	Joker
};
enum StoneAbility // 능력 혹은 이름
{
	None, // 흑돌 백돌
	//---------------- 일반 (set 1)
	jokerDouble,
	jokerOmok,
	jokerSamok,
	jokerSammok,

	//----------------  야생(set 2)
	jokerEvolution, // 리소스 X
	jokerDansu,
	jokerEgg, // 함수 구현 완료
	jokerOstrichEgg, // 함수 구현 완료
	jokerPeacock, // 함수 구현 완료

	//---------------- 우주 (set 3)
	jokerTeleport,
	jokerExplode, // 함수 구현 완료
	jokerMagnetic,
	jokerBlackhole,

	//---------------- 단청 (set 4)
	jokerFusion, // 함수 구현 완료
	jokerTriunion, // 함수 구현 완료
	jokerQuadunion, // 함수 구현 완료

	//---------------- 할로윈 (set 6)
	jokerSplit, // 함수 구현 완료
	jokerWaxseal,
	jokerFlip,
	jokerOthello,
	jokerMrchan,

	//---------------- 자연 (set 7)
	jokerShadow,
	jokerLight,
	jokerTime,
	jokerWind,
};

class Stone : public Object
{
protected:
	Transform* m_transform {};
	BitmapRender3D* m_sprite {};

	float m_size = 0; // 돌 크기
	float m_offset = 0; // 돌 간격

	double m_lerpTime = 0.0; // 이동 보간 시간
	double m_lerpElapsedTime = 0.0; // 경과 시간
	XMVECTOR m_lerpStartPosition = XMVectorZero(); // 이동 시작 위치
	XMVECTOR m_lerpEndPosition = XMVectorZero(); // 이동 끝 위치

public:
	Stone() = default;

	void Update(double deltaTime) override; // 돌 업데이트 함수
	void Move(POINT position, double duration = 1.0); // 돌 이동 함수

	POINT GetPosition() const;
};

class WhiteStone : public Stone
{
public:
	WhiteStone(POINT position, float size, int offset)
	{
		m_size = size;
		m_offset = offset;

		m_transform = AddComponent<Transform>();
		m_transform->SetPosition(XMVectorSet(static_cast<float>(position.x) + size / 2, static_cast<float>(position.y) + size / 2, 0.0f, 1.0f));
		m_transform->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
		m_transform->SetRotation(0.0f);

		m_sprite = AddComponent<BitmapRender3D>("White.png", size - offset, size - offset);
		m_sprite->SetOrder(1);
		m_sprite->SetActive(true);
	}
};

class BlackStone : public Stone
{
public:
	BlackStone(POINT position, float size, int offset)
	{
		m_size = size;
		m_offset = offset;

		m_transform = AddComponent<Transform>();
		m_transform->SetPosition(XMVectorSet(static_cast<float>(position.x) + size / 2, static_cast<float>(position.y) + size / 2, 0.0f, 1.0f));
		m_transform->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
		m_transform->SetRotation(0.0f);

		m_sprite = AddComponent<BitmapRender3D>("Black.png", size - offset, size - offset);
		m_sprite->SetOrder(1);
		m_sprite->SetActive(true);
	}
};

struct JokerInfo // 조커 돌 정보
{
	string fileName = "JokerEgg.png"; // 조커 돌 이미지 파일 이름

	int blackReturn = 0; // 조커 돌 능력 사용 후 반환 비용(흑돌)

	int coolTime = 0; // 조커 돌 능력 쿨타임
	int lifeSpan = 0; // 조커 돌 능력 지속 시간

	int functionDuration = 0; // 조커 돌 능력 함수 지속 시간 // 0이면 한번만 실행
	int functionVariable = 0; // 조커 돌 능력 함수에 사용되는 변수 // 능력에 따라 다름
};
extern unordered_map<StoneAbility, JokerInfo> m_jokerInfoMap; // 조커 돌 능력 정보 맵 // 능력의 기본값 저장용

class JokerStone : public Stone
{
public:
	JokerStone(POINT position, float size, int offset, StoneAbility ability)
	{
		m_jokerInfo = m_jokerInfoMap[ability]; // 조커 돌 능력 정보 가져오기(값 복사)
		m_size = size;
		m_offset = offset;

		m_transform = AddComponent<Transform>();
		m_transform->SetPosition(XMVectorSet(static_cast<float>(position.x) + size / 2, static_cast<float>(position.y) + size / 2, 0.0f, 1.0f));
		m_transform->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
		m_transform->SetRotation(0.0f);

		m_sprite = AddComponent<BitmapRender3D>(m_jokerInfo.fileName.c_str(), size - offset, size - offset);
		m_sprite->SetOrder(1);
		m_sprite->SetActive(true);
	}

	JokerInfo m_jokerInfo; // 조커 돌 능력 정보
};