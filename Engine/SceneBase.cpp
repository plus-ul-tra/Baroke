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

		//일반 그리기
		if (auto bitmapRenderComp = obj->GetComponent<BitmapRender3D>())
		{
			if (bitmapRenderComp->IsActive())
			{
				bitmapRenderComp->Render(renderer,"SpriteShader");
			}
		}

		// 안씀
		/*if (auto textRenderComp = obj->GetComponent<TextRender>())
		{
			if (textRenderComp->IsActive())
			{
				textRenderComp->Render(renderer);
			}
		}
		obj->Render(renderer);*/
	}



	// --- UI
	for (const auto& ui : m_UIList) {

		//일반 그리기
		if (auto bitmapRenderComp = ui->GetComponent<BitmapRender3D>())
		{
			if (bitmapRenderComp->IsActive())
			{
				bitmapRenderComp->Render(renderer, "SpriteShader");
			}
		}
		ui->Render(renderer);
	}


}
//
//void RenderObject(Object* obj, Renderer& renderer)
//{
//	// 디버그용 GeoRender
//	if (MIYABI::Core::GetInstance().IsDebugMode())
//	{
//		if (auto geo = obj->GetComponent<GeoRender>())
//			if (geo->IsActive())
//				geo->Render(renderer);
//	}
//
//	// 일반 비트맵 렌더
//	if (auto bmp = obj->GetComponent<BitmapRender3D>())
//		if (bmp->IsActive())
//			bmp->Render(renderer);
//
//	// 오브젝트 자체 렌더
//	obj->Render(renderer);
//
//}


void SceneBase::FixedUpdate(double fixedDeltaTime)
{


}


void SceneBase::LateUpdate(double deltaTime)
{


}

