//@group UI

#ifndef UIWIDGET_H
#define UIWIDGET_H

#include "core_utils.h"
#include <string>

namespace Engine
{
  struct UIData
  {
    union
    {
      bool bVal;
      int iVal;
      float fVal;
      char* pcVal;
    };
  };

  typedef void (*fnUICallback)(UIData const &);

  enum class UICallback
  {
    GainFocus,
    LoseFocus,
    Activate,   //Button
    NewValue,   //Slider, Lists
    Toggle,     //Checkbox
    COUNT
  };

  class UIWidget
  {
  public:

    //TODO add rotation to widgets
    UIWidget(std::string const& name,
             vec3 trans, vec3 scale,
             bool isInteractive, int depth = 0
    );
    virtual ~UIWidget();

    virtual bool IsInside(float x, float y);

    //return: handled
    virtual bool HandleNewCursonPostion(float x, float y);
    
    virtual void DoEventGainFocus() {}
    virtual void DoEventLoseFocus() {}
    virtual void DoEventMouseDown() {}
    virtual void DoEventMouseUp() {}
    virtual void DoEventScroll(float val) {}
    virtual void DoEventText(char text[32]) {}
    virtual void DoEventActivate() {}

    void SetFocus(bool);
    bool HasFocus() const;

    virtual void Render(){}

    void Translate(float xMin, float yMin);
    void Scale(float sx, float sy);

    void SetGlobalTranslation(float xMin, float yMin);
    void SetGlobalScale(float sx, float sy);

    void SetTranslation(float xMin, float yMin);
    void SetScale(float sx, float sy);
    void SetDepth(int);

    vec3 GetTranslation() const;
    vec3 GetScale() const;
    int GetDepth() const;

    virtual void DepthHasChanged(UIWidget* child){}

    void SetGlobalToLocal();
    mat3 GetGlobalToLocal() const;
    mat3 GetLocalToParent() const;
    bool IsInteractive() const;
    void SetIsInteractive(bool);
    void SetParent(UIWidget*);

    virtual void NewTransform()
    {
    }

  protected:
    std::string m_name;
    fnUICallback m_bindings[static_cast<size_t>(UICallback::COUNT)];
    bool m_isInteractive;
    UIWidget* m_pParent;
    vec3 m_translation;
    vec3 m_scale;
    mat3 m_T_Global_to_Local;
    bool m_hasFocus;
    int m_depth;
  };


}

#endif