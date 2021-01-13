//@group GUI

#ifndef GUI_SLIDER_H
#define GUI_SLIDER_H

#include "Utils.h"
#include "GUI_Widget.h"

namespace Engine
{
  namespace GUI
  {
    enum class SliderState
    {
      Normal,
      Hover,
      Grab,

      COUNT
    };

    enum class SliderElement
    {
      Lower,
      Upper,
      Caret,
      Outline,

      COUNT
    };

    class Slider : public Widget
    {
    public:

      ~Slider();

      void SetColour(SliderState, SliderElement, Colour);

      void BindHoverOn(std::function<void()> a_fn);
      void BindHoverOff(std::function<void()> a_fn);

      void Draw() override;
      WidgetState QueryState() const override;
      Widget * GetParent() const override;
      void SetParent(Widget *) override;

      class InternalState;
      virtual void NewValueClbk(float) = 0;

    protected:

      Slider(Widget * pParent, vec2 const & position, float width, float value, std::initializer_list<WidgetFlag> flags);
      void SetVal(float);

    private:

      void _HandleMessage(Message *) override;
      void UpdateState(InternalState * a_pState);

      void _SetLocalPosition(vec2 const &) override;
      void _SetSize(vec2 const &) override;
      vec2 _GetLocalPosition() override;
      vec2 _GetSize() override;

    private:

      InternalState * m_pState;
    };

    class SliderFloat : public Slider
    {
      SliderFloat(Widget * pParent, vec2 const & position, float width, float val, float minVal, float maxVal, std::initializer_list<WidgetFlag> flags);
    public:

      static SliderFloat * Create(Widget * pParent, vec2 const & position, float width, float val, float minVal, float maxVal, std::initializer_list<WidgetFlag> flags = {});

      void BindNewValue(std::function<void(float)> a_fn);

    private:

      void NewValueClbk(float) override;

      float m_minVal;
      float m_maxVal;
      float m_lastValue;
      std::function<void(float)> m_clbk_NewValue;
    };
  }
}

#endif