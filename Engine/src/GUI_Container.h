//@group GUI

#ifndef GUI_WINDOW_H
#define GUI_WINDOW_H

#include "Utils.h"
#include "GUI_Widget.h"

namespace Engine
{
  namespace GUI
  {
    class Container : public Widget
    {
      Container(Widget * pParent, vec2 const position, vec2 const & size, std::initializer_list<WidgetFlag> flags);
    public:

      static vec2 const s_minSize;

      static Container * Create(Widget * pParent, vec2 const position, vec2 const & size, std::initializer_list<WidgetFlag> flags = {WidgetFlag::Movable, WidgetFlag::Resizable});
      ~Container();

      void SetBackgroundColour(Colour);
      void SetDivBorder(float left, float top, float right, float bottom);
      //void SetHoverOnBackgroundColour(Colour);

      void Clear();
      void Add(Widget *);
      void Remove(Widget *);

      void Draw() override;

      WidgetState QueryState() const override;
      Widget * GetParent() const override;
      void SetParent(Widget *) override;
      vec2 GetContentDivPosition() override;
      vec2 GetContentDivSize() override;

      bool IsContainer() const override;

      class ContainerState;

    private:

      void _HandleMessage(Message *) override;

      void UpdateState(ContainerState *);

      void _SetLocalPosition(vec2 const &) override;
      void _SetSize(vec2 const &) override;
      vec2 _GetLocalPosition() override;
      vec2 _GetSize() override;

    private:

      ContainerState * m_pState;
    };
  }
}

#endif