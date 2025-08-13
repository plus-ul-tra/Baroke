#pragma once
#include "Object.h"
#include "Transform.h"
#include "BitmapRender.h"
#include "Mediator.h"
#include "CreateObject.h"
#include "SoundManager.h"

enum StoneType // 돌 종류
{
	White,
	Black,
	Joker
};
enum StoneAbility // 능력 혹은 이름
{
	None, // 흑돌 백돌 // 빈 조커 슬롯
	//---------------- 일반 (set 1)
	//jokerDouble, 
	jokerOmok, 
	jokerSamok,
	jokerSammok,

	//----------------  야생(set 2)
	jokerEvolution,
	jokerDansu, // 함수 구현 완료
	jokerEgg, // 함수 구현 완료
	jokerOstrichEgg, // 함수 구현 완료
	jokerPeacock, // 함수 구현 완료

	//---------------- 우주 (set 3)
	jokerTeleport,
	jokerExplode, // 함수 구현 완료
	jokerMagnetic,
	jokerBlackhole, // 함수 구현 완료

	//---------------- 단청 (set 4)
	jokerFusion, // 함수 구현 완료
	jokerTriunion, // 함수 구현 완료
	jokerQuadunion, // 함수 구현 완료

	//---------------- 할로윈 (set 6)
	jokerSplit, // 함수 구현 완료
	jokerWaxseal, // 함수 구현 완료
	jokerFlip, // 함수 구현 완료
	jokerOthello, // 함수 구현 완료
	jokerMrchan, // 함수 구현 완료

	//---------------- 자연 (set 7)
	jokerShadow, // 함수 구현 완료
	jokerLight,
	jokerTime, // 함수 구현 완료
	jokerWind, // 함수 구현 완료

	Count
};
enum JokerType
{
	Wild = 0,
	Space = 1,
	Dancheong = 2,
	Halloween = 3,
	Natural = 4,

	Default
};

class Stone : public Object
{
protected:
	Transform* m_transform {};
	BitmapRender3D* m_sprite {};
	unique_ptr<OneTimeEffect> m_placeEffect; // 돌 착수 효과
	unique_ptr<OneTimeEffect> m_removeEffect; // 돌 제거 효과

	float m_size = 0; // 돌 크기
	float m_offset = 0; // 돌 간격

	double m_lerpTime = 0.0; // 이동 보간 시간
	double m_lerpElapsedTime = 0.0; // 경과 시간
	XMVECTOR m_lerpStartPosition = XMVectorZero(); // 이동 시작 위치
	XMVECTOR m_lerpEndPosition = XMVectorZero(); // 이동 끝 위치

	XMVECTOR m_removePosition = XMVectorZero(); // 제거 위치
	bool m_isRemoving = false;
	double m_queueRemoveTime = 0.0;
	StoneAbility ability = StoneAbility::None;

public:
	Stone() = default;

	JokerType m_jokerType = JokerType::Default;

	void Update(double deltaTime) override; // 돌 업데이트 함수
	void Render(Renderer& renderer) override;
	void Move(POINT position, double duration = 1.0); // 돌 이동 함수
	void DeathMove(double duration = 1.0);

	bool m_isRemoved = false;
	void Remove(double duration = 0.0) { m_isRemoving = true; m_queueRemoveTime = duration; }

	POINT GetPosition() const;
	void ChangeColor(bool isBlack = true);
	StoneAbility GetAbility() const { return ability; }
	BitmapRender3D* GetSprite() const { return m_sprite; }
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

		m_sprite = AddComponent<BitmapRender3D>("T_whitestone.png", (size - offset) * 1.5f, (size - offset) * 1.5f);
		//m_sprite->SetShaderType("SetRed"); 
		m_sprite->SetOrder(1);
		m_sprite->SetActive(true);


		m_placeEffect = make_unique<OneTimeEffect>(static_cast<float>(position.x) + size / 2, static_cast<float>(position.y) + size / 2, size - offset, size - offset, "drop_pattern2.json");

		m_removePosition = XMVectorSet(-640.0f, 120.0f, 0.0f, 1.0f);
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

		m_sprite = AddComponent<BitmapRender3D>("T_Blackstone.png", (size - offset) * 1.5f, (size - offset) * 1.5f);
		//m_sprite->SetShaderType("SetRed"); 
		m_sprite->SetOrder(1);
		m_sprite->SetActive(true);


		m_placeEffect = make_unique<OneTimeEffect>(static_cast<float>(position.x) + size / 2, static_cast<float>(position.y) + size / 2, size - offset, size - offset, "drop_pattern1.json");

		m_removePosition = XMVectorSet(-800.0f, 120.0f, 0.0f, 1.0f);

		SoundManager::GetInstance().PlaySoundOnce("stonePlace.wav"); // 흑돌 착수 사운드 재생
	}
};

struct JokerStoneInfo // 조커 돌 정보
{
	string fileName = "Empty.png"; // 조커 돌 이미지 파일 이름
	string toolTipName = "Empty.png"; // 조커 돌 툴팁 이름
	string soundName = "stonePlace.wav"; // 조커 돌 착수 사운드 이름

	JokerType jokerType = JokerType::Default;

	int coolTime = 0; // 조커 돌 능력 쿨타임
	int lifeSpan = 0; // 조커 돌 능력 지속 시간

	int functionDuration = 0; // 조커 돌 능력 함수 지속 시간 // 0이면 한번만 실행
	int functionVariable = 0; // 조커 돌 능력 함수에 사용되는 변수 // 능력에 따라 다름

	int costBlack = 0;
	int returnBlack = 0;

	int costWhite = 0; // 상점 비용
	int rarity = 0;
	bool isStone = false; // 돌인지 여부
	bool inShop = true; // 상점에 나오는지 여부 기본 트루

	StoneType stoneType = StoneType::Joker;
};
extern unordered_map<StoneAbility, JokerStoneInfo> m_jokerInfoMap; // 조커 돌 능력 정보 맵 // 능력의 기본값 저장용
extern unordered_map<StoneAbility, string> m_jokerAnimationMap; // 조커 돌 애니메이션 맵 // 능력의 애니메이션 이름 저장용

class JokerStone : public Stone
{
public:
	JokerStone(POINT position, float size, int offset, StoneAbility ability, JokerType jokerType = JokerType::Default)
	{
		m_jokerInfo = m_jokerInfoMap[ability]; // 조커 돌 능력 정보 가져오기(값 복사)
		m_size = size;
		m_offset = offset;

		m_transform = AddComponent<Transform>();
		m_transform->SetPosition(XMVectorSet(static_cast<float>(position.x) + size / 2, static_cast<float>(position.y) + size / 2, 0.0f, 1.0f));
		m_transform->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
		m_transform->SetRotation(0.0f);

		if (m_jokerAnimationMap.find(ability) != m_jokerAnimationMap.end())
		{
			m_sprite = AddComponent<BitmapRender3D>(m_jokerAnimationMap[ability], (size - offset) * 1.5f, (size - offset) * 1.5f);
		}
		else m_sprite = AddComponent<BitmapRender3D>(m_jokerInfo.fileName, (size - offset) * 1.5f, (size - offset) * 1.5f);
		m_sprite->SetOrder(2);
		m_sprite->SetActive(true);

		m_jokerType = jokerType; // 조커 타입 설정
		this->ability = ability;


		m_placeEffect = make_unique<OneTimeEffect>(static_cast<float>(position.x) + size / 2, static_cast<float>(position.y) + size / 2, size - offset, size - offset, "drop_pattern3.json");

		m_removePosition = XMVectorSet(750.0f, 0.0f, 0.0f, 1.0f);

		SoundManager::GetInstance().PlaySoundOnce(m_jokerInfo.soundName); // 조커 돌 착수 사운드 재생
	}

	JokerStoneInfo m_jokerInfo; // 조커 돌 능력 정보
	void UpdateAbility(StoneAbility newAb);
};