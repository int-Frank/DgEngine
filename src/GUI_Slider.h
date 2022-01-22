//@group GUI

#ifndef GUI_SLIDER_H
#define GUI_SLIDER_H

#include "Utils.h"
#include "GUI.h"
#include "GUI_Widget.h"

namespace DgE
{
  namespace GUI
  {
    class SliderBase : public Widget
    {
    public:

      ~SliderBase();

      static Style::Slider const &GetDefaultStyle();
      Style::Slider const &GetStyle() const;
      void SetStyle(Style::Slider const &);

      void BindHoverOn(std::function<void()> a_fn);
      void BindHoverOff(std::function<void()> a_fn);

      void Draw() override;
      WidgetState QueryState() const override;
      Widget * GetParent() const override;
      void SetParent(Widget *) override;

      virtual void NewValueClbk(float) = 0;

      void SetLocalPosition(vec2 const&) override;
      void SetSize(vec2 const&) override;
      vec2 GetLocalPosition() override;
      vec2 GetSize() override;

    protected:

      static Style::Slider const s_style;

      SliderBase(vec2 const &position, float length, float value, Style::Slider const &style, bool vertical, std::initializer_list<WidgetFlag> flags);
      float SetNormalisedValue(float); // Set value between 0 and 1

    private:

      void _HandleMessage(Message *) override;

    private:

      class PIMPL;
      PIMPL *m_pimpl;
    };

    template<typename T>
    class Slider : public SliderBase
    {
      Slider(vec2 const & position, float length, T minVal, T maxVal, T initialVal, bool vertical, Style::Slider const &, std::initializer_list<WidgetFlag> flags);
    public:

      static Slider *Create(vec2 const &position, float length, T minVal, T maxVal, T initialVal, bool vertical, std::initializer_list<WidgetFlag> flags = {});
      static Slider *Create(vec2 const &position, float length, T minVal, T maxVal, T initialVal, bool vertical, Style::Slider const &, std::initializer_list<WidgetFlag> flags = {});

      void BindNewValue(std::function<void(T)> a_fn);
      void SetValue(T);
      T GetValue() const;

    private:

      void NewValueClbk(float) override;

      T m_minVal;
      T m_maxVal;
      T m_lastValue;
      std::function<void(T)> m_clbk_NewValue;
    };

    using SliderFloat = Slider<float>;
    using SliderInt = Slider<int32_t>;

    template<typename T>
    Slider<T>::Slider(vec2 const & a_position, float a_length,
                      T a_minVal, T a_maxVal, T a_initialVal, bool vertical, Style::Slider const & style,
                      std::initializer_list<WidgetFlag> a_flags)
      : SliderBase(a_position, a_length, 0.0f, style, vertical, a_flags)
      , m_minVal(a_minVal)
      , m_maxVal(a_maxVal)
      , m_lastValue(T(0))
    {
      if (m_minVal > m_maxVal)
        m_minVal = m_maxVal;

      if (a_initialVal < m_minVal)
        a_initialVal = m_minVal;
      else if (a_initialVal > m_maxVal)
        a_initialVal = m_maxVal;

      SetValue(a_initialVal);

      m_lastValue = a_initialVal;
    }

    template<typename T>
    Slider<T> *Slider<T>::Create(vec2 const &a_position, float a_length,
                                 T a_minVal, T a_maxVal, T a_initialVal, bool vertical, Style::Slider const &style,
                                 std::initializer_list<WidgetFlag> a_flags)
    {
      return new Slider(a_position, a_length, a_minVal, a_maxVal, a_initialVal, vertical, style, a_flags);
    }

    template<typename T>
    Slider<T> *Slider<T>::Create(vec2 const &a_position, float a_length,
                                 T a_minVal, T a_maxVal, T a_initialVal, bool vertical,
                                 std::initializer_list<WidgetFlag> a_flags)
    {
      return new Slider(a_position, a_length, a_minVal, a_maxVal, a_initialVal, vertical, s_style, a_flags);
    }

    template<typename T>
    void Slider<T>::BindNewValue(std::function<void(T)> a_fn)
    {
      m_clbk_NewValue = a_fn;
    }

    template<typename T>
    void Slider<T>::SetValue(T a_val)
    {
      float v = 0.0f;
      if (m_maxVal != m_minVal)
        v = float(a_val - m_minVal) / float(m_maxVal - m_minVal);

      NewValueClbk(SetNormalisedValue(v));
    }

    template<typename T>
    T Slider<T>::GetValue() const
    {
      return m_lastValue;
    }

    template<typename T>
    void Slider<T>::NewValueClbk(float a_val)
    {
      if (m_clbk_NewValue != nullptr)
      {
        T val = T(a_val * (m_maxVal - m_minVal) + m_minVal);
        //if (val != m_lastValue)
        {
          m_clbk_NewValue(val);
          m_lastValue = val;
        }
      }
    }
  }
}

#endif