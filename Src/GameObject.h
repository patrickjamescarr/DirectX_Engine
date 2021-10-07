#pragma once
#include "Bindable.h"
#include "IndexBuffer.h"

/// <summary>
/// base class for any game object that can be drawn via the graphics pipeline
/// </summary>
class GameObject
{
friend class Chest;
public:
	GameObject() = default;
	virtual ~GameObject();
	virtual DirectX::SimpleMath::Matrix GetTransform() const noexcept = 0;
	void Draw(DX::DeviceResources & deviceResources) const;
protected:
    void AddBind(std::unique_ptr<Bindable> bind);
    void AddIndexBuffer(std::unique_ptr<IndexBuffer> indexBuffer);
private:
    const IndexBuffer* m_indexBuffer = nullptr;
    std::vector<std::unique_ptr<Bindable>> binds;
    std::string name;
};
