#ifndef SCENE_MATERIAL_MGR_H
#define SCENE_MATERIAL_MGR_H

#include <vector>
#include "SceneMaterial.h"
#include "Tools.h"

#define CHECK_ATTR(a) (a == NULL ? "" : a)
#define CHECK_ATTR2(a,b) (a == NULL ? b : a)

class SceneMaterialMgr
{
private:
  std::vector<SceneMaterial> m_MaterialList;
  SceneMaterialMgr(){}
  SceneMaterialMgr(SceneMaterialMgr const&);
  void operator=(SceneMaterialMgr const&);

public:
  ~SceneMaterialMgr(){}
  static SceneMaterialMgr& GetInstance()
  {
    static SceneMaterialMgr _sceneMaterialMgr;
    return _sceneMaterialMgr;
  }
  // - GetMaterial - Returns the nth SceneMaterial
  SceneMaterial GetMaterial (int matIndex) { return m_MaterialList[matIndex]; }
  SceneMaterial GetMaterial (std::string matName)
  {
    unsigned int numMats = (unsigned int)m_MaterialList.size ();
    for (unsigned int n = 0; n < numMats; n++)
    {
      if (matName == m_MaterialList[n].name)
        return m_MaterialList[n];
    }
    return SceneMaterial ();
  }
  unsigned int size()
  {
    return m_MaterialList.size();
  }
  bool Load(XMLNode tempNode)
  {
    unsigned int numMaterials = tempNode.nChildNode ("material");
    for (unsigned int n = 0; n < numMaterials; n++)
    {
      XMLNode tempMaterialNode = tempNode.getChildNode("material", n);
      if (tempMaterialNode.isEmpty ())
        return false;
      SceneMaterial tempMaterial;
      tempMaterial.name = CHECK_ATTR(tempMaterialNode.getAttribute("name"));
      tempMaterial.texture = CHECK_ATTR(tempMaterialNode.getChildNode("texture").getAttribute("filename"));
      tempMaterial.diffuse = Tools::ParseColor (tempMaterialNode.getChildNode("diffuse"));
      tempMaterial.specular = Tools::ParseColor (tempMaterialNode.getChildNode("specular"));
      tempMaterial.shininess = atof(CHECK_ATTR(tempMaterialNode.getChildNode("specular").getAttribute("shininess")));
      tempMaterial.transparent = Tools::ParseColor (tempMaterialNode.getChildNode("transparent"));
      tempMaterial.reflective = Tools::ParseColor (tempMaterialNode.getChildNode("reflective"));
      tempMaterial.refraction_index = Tools::ParseColor (tempMaterialNode.getChildNode("refraction_index"));

      m_MaterialList.push_back (tempMaterial);
    }
    return true;
  }
};

#endif