#include "pch.h"
#include "SceneBase.h"
#include "GeoRender.h"
#include "BitmapRender.h"
#include "TextRender.h"
#include "Core.h"
//using namespace MIYABI;
//class Core;

void SceneBase::Update(double deltaTime) 
{


}

void SceneBase::Render(Renderer& renderer) {

	for (const auto& obj : m_objectList) {

		//디버그 모드 collider 상자 표시
		if (MIYABI::Core::GetInstance().IsDebugMode()) {

			if (auto geoRenderComp = obj->GetComponent<GeoRender>()) {
					
				if (geoRenderComp->IsActive()) {
					geoRenderComp->Render(renderer);	
				}
			}

		}
		//일반 그리기
		if (auto bitmapRenderComp = obj->GetComponent<BitmapRender>())
		{
			if (bitmapRenderComp->IsActive())
			{
				bitmapRenderComp->Render(renderer);
			}
		}
		if (auto textRenderComp = obj->GetComponent<TextRender>())
		{
			if (textRenderComp->IsActive())
			{
				textRenderComp->Render(renderer);
			}
		}
		obj->Render(renderer);
	}
}


void SceneBase::FixedUpdate(double fixedDeltaTime)
{


}


void SceneBase::LateUpdate(double deltaTime)
{


}

