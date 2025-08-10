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
	RenderTimer::GetInstance().Update();
}

void SceneBase::Render(Renderer& renderer) {

	for (const auto& obj : m_objectList) {

		if (auto bitmapRenderComp = obj->GetComponent<BitmapRender3D>())
		{
			if (bitmapRenderComp->IsActive())
			{
				bitmapRenderComp->Render(renderer);
			}
		}
		obj->Render(renderer);
	}
	for (const auto& ui : m_UIList) {

		if (auto bitmapRenderComp = ui->GetComponent<BitmapRender3D>())
		{
			if (bitmapRenderComp->IsActive())
			{
				bitmapRenderComp->Render(renderer);
			}
		}
		ui->Render(renderer);
	}
	for (const auto& notUniqueObject : m_notUniqueObjectList)
	{
		if (auto bitmapRenderComp = notUniqueObject->GetComponent<BitmapRender3D>())
		{
			if (bitmapRenderComp->IsActive())
			{
				bitmapRenderComp->Render(renderer);
			}
		}
		notUniqueObject->Render(renderer);
	}
	renderer.UITextBegin();
	//text 루틴 추가
	//m_textList 에서 object와 유사한 구조로
	renderer.DrawUIText(L"99", -680.0f, 440.0f,100.0f,100.0f,2);
	renderer.DrawUIText(L"5959", -850.0f, -200.0f, 260.0f, 100.0f,2);

	renderer.UITextEnd();
}

void RenderObject(Object* obj, Renderer& renderer)
{

	if (MIYABI::Core::GetInstance().IsDebugMode())
	{
		if (auto geo = obj->GetComponent<GeoRender>())
			if (geo->IsActive())
				geo->Render(renderer);
	}


	if (auto bmp = obj->GetComponent<BitmapRender3D>())
		if (bmp->IsActive())
			bmp->Render(renderer);

	obj->Render(renderer);

}


void SceneBase::FixedUpdate(double fixedDeltaTime)
{


}


void SceneBase::LateUpdate(double deltaTime)
{


}

