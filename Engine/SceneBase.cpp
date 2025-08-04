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

		if (auto bitmapRenderComp = obj->GetComponent<BitmapRender3D>())
		{
			if (bitmapRenderComp->IsActive())
			{
				bitmapRenderComp->Render(renderer);
			}
		}
		obj->Render(renderer);
		//if (auto textRenderComp = obj->GetComponent<TextRender>())
		//{
		//	if (textRenderComp->IsActive())
		//	{
		//		textRenderComp->Render(renderer);
		//	}
		//}
	}



	// --- UI
	//for (const auto& ui : m_UIList) {


	//	//�Ϲ� �׸���
	//	if (auto bitmapRenderComp = ui->GetComponent<BitmapRender3D>())
	//	{
	//		if (bitmapRenderComp->IsActive())
	//		{
	//			bitmapRenderComp->Render(renderer, "DefaultShader");
	//		}
	//	}
	//	ui->Render(renderer);
	//}


}
//
void RenderObject(Object* obj, Renderer& renderer)
{
	// ����׿� GeoRender
	if (MIYABI::Core::GetInstance().IsDebugMode())
	{
		if (auto geo = obj->GetComponent<GeoRender>())
			if (geo->IsActive())
				geo->Render(renderer);
	}

	// �Ϲ� ��Ʈ�� ����
	if (auto bmp = obj->GetComponent<BitmapRender3D>())
		if (bmp->IsActive())
			bmp->Render(renderer);

	// ������Ʈ ��ü ����
	obj->Render(renderer);

}


void SceneBase::FixedUpdate(double fixedDeltaTime)
{


}


void SceneBase::LateUpdate(double deltaTime)
{


}

