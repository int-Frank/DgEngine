//@group UI

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
      Container(Widget * pParent, vec2 const position, vec2 const & size, uint32_t flags);
    public:

      static vec2 const s_minSize;

      enum Flag : uint32_t
      {
        Resizable     = (1u << 0),
        Movable       = (1u << 1),
        NoBackground  = (1u << 2),

        COUNT
      };

      static Container * Create(Widget * pParent, vec2 const position, vec2 const & size, uint32_t flags = Resizable | Movable);
      ~Container();

      void SetPosition(vec2 const &) override;
      void SetSize(vec2 const &) override;

      void SetBackgroundColour(Colour);
      //void SetHoverOnBackgroundColour(Colour);

      void Clear();
      void Add(Widget *);
      void Remove(Widget *);

      void HandleMessage(Message *);

      void Draw() override;

      WidgetState QueryState() const override;
      Widget * GetParent() const override;
      void SetParent(Widget *) override;
      vec2 GetLocalPosition() const override;
      vec2 GetSize() const override;

      bool IsContainer() const override;

      class ContainerState;

    private:

      void UpdateState(ContainerState *);
      ContainerState * m_pState;
    };
  }
}

#endif