#pragma once

#include "../UIObject/UIObject.h"

namespace PM3D_API
{
class UICanvas : public UIObject
{
public:
	static constexpr DirectX::XMFLOAT2 referenceResolution { 1920, 1080 };
	
	UICanvas() : UIObject("Canvas")
	{
	}

	UICanvas(const std::string& name) : UIObject(name, referenceResolution)
	{
	}

	~UICanvas() override;
	
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawSelf() const override;

	void AddChild(std::unique_ptr<UIObject>&& child) override;

protected:
	bool isDeleted = false;
};
}
