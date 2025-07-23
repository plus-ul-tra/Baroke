#include "pch.h"
#include "SceneBase.h"
#include "GeoRender.h"
#include "Core.h"
//using namespace MIYABI;
//class Core;

void SceneBase::Update(double deltaTime) 
{


}

void SceneBase::Render(Renderer& renderer) {

	for (const auto& obj : m_objectList) {

		//����� ��� collider ���� ǥ��
		if (MIYABI::Core::GetInstance().IsDebugMode()) {

			if (auto geoRenderComp = obj->GetComponent<GeoRender>()) {
					
				if (geoRenderComp->IsActive()) {
					geoRenderComp->Render(renderer);	
				}
			}

		}
		//�Ϲ� �׸���
	}


}


void SceneBase::FixedUpdate(double fixedDeltaTime)
{


}


void SceneBase::LateUpdate(double deltaTime)
{


}
