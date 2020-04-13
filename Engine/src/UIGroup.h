//@group UI

#ifndef UIGROUPS_H
#define UIGROUPS_H

#include "core_utils.h"
#include "UIWidget.h"
#include "DgDoublyLinkedList.h"

namespace Engine
{
  class UIGroup : public UIWidget
  {
  public:

    UIGroup(std::string const& name, vec3 trans, vec3 scale);
    ~UIGroup();

    bool HandleNewCursonPostion(float x, float y) override;
    bool IsInside(float x, float y) override;
    virtual void Render();

    void DoEventActivate() override;

    void Clear();
    void Add(UIWidget*);
    bool Remove(UIWidget*);

    void DepthHasChanged(UIWidget*);

  private:

    void SetInteractiveSpace();
    void NewTransform();

  private:
    mat3 m_T_Global_to_Interactive;
    Dg::DoublyLinkedList<UIWidget*> m_children;
    UIWidget* m_pFocus;
  };
}

#endif