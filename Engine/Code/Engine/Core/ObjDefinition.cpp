#include "ObjDefinition.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "VertexUtils.hpp"
#include "ObjUtils.hpp"
#include "Engine/Math/EulerAngles.hpp"
ObjDefinition::ObjDefinition(std::string const& fileName)
{
	XmlDocument cubeDoc;
	XmlResult result = cubeDoc.LoadFile(fileName.c_str());
	UNUSED(result);
	XmlElement* objElement = cubeDoc.RootElement();


	m_objFilePath = ParseXmlAttribute(*objElement, "objFile", "");
	m_diffuse = ParseXmlAttribute(*objElement, "diffuseMap", "");
	m_normal = ParseXmlAttribute(*objElement, "normalMap", "");
	m_sge = ParseXmlAttribute(*objElement, "sgeMap", "");
	m_shader = ParseXmlAttribute(*objElement, "shader", "");

	float scale = ParseXmlAttribute(*objElement, "scale", 1.f);
	Vec3 rotation = ParseXmlAttribute(*objElement, "rotation", Vec3());
	EulerAngles angle(rotation.x, rotation.y, rotation.z);

	m_modelToWorldOrigin = angle.GetAsMatrix_IFwd_JLeft_KUp();
	m_modelToWorldOrigin.AppendScaleUniform3D(scale);

	m_reverseTriangle = ParseXmlAttribute(*objElement, "reverseTriangle", false);

}

