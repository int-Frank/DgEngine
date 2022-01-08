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

      void Clear();
      void Add(Widget *); // TODO Add check if widget already exists.
      void Remove(Widget *);

      virtual bool IsContainer() const;

    protected:

      void _HandleMessage(Message *) override;

      void _SetLocalPosition(vec2 const &) override;
      void _SetSize(vec2 const &) override;
      vec2 _GetLocalPosition() override;
      vec2 _GetSize() override;

    private:

      class PIMPL;
      PIMPL *m_pimpl;
    };
  }
}

#endif