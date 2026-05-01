#pragma once
#include <string>
#include <memory>
#include <vector>
#include "EditorProperty.h"

class ComponentInspector
{
public:
	ComponentInspector(std::string title);

	void Draw();

protected:
	std::string m_Title;
	std::vector<std::shared_ptr<IEditorProperty>> m_Properties;
};