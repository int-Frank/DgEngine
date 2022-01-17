//@group GUI

#ifndef GUI_CONTAINER_H
#define GUI_CONTAINER_H

#include <string>
#include <functional>

#include "GUI_Widget.h"

namespace DgE
{
  namespace GUI
  {
    class Container : public Widget
    {
      Container(vec2 const position, vec2 const &size);
    public:

      static Container *Create(vec2 const position, vec2 const &size);

      ~Container();

      void Draw() override;
      Widget *GetParent() const override;
      void SetParent(Widget *) override;
      WidgetState QueryState() const override;

      //bool GetContentBounds(UIAABB &result) const;

      void Clear();
      void Add(Widget *); // TODO Add check if widget already exists.
      void Remove(Widget *);
      void Move(Widget* pWgt, vec2 const&); // Will expand the container to accomodate the new position.
      bool Empty() const;

      bool IsContainer() const;

      void FitSizeToContent(vec2 const & defaultSize);
      void SetLocalPosition(vec2 const&) override;
      void SetSize(vec2 const&) override;
      vec2 GetLocalPosition() override;
      vec2 GetSize() override;

    protected:

      void _HandleMessage(Message *) override;

    private:

      class PIMPL;
      PIMPL *m_pimpl;
    };
  }
}

#endif